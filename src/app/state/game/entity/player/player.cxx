module entity.player;
import app.config;
import anim;
import aabb;
import console;
import health;
import input;
import particle_system;
import sound;
import sprite;
import transform;
import types;

namespace entity {
    Player::Player() {
        console::log("Player()\n");
        m_input_id = input::make();

        m_type = Type::player;
        m_state = m_next_state = state::Type::run;

        load_config("res/entity/player/britt.cfg");
    }

    Player::~Player() {
        console::log("Player::~Player() input\n");
        input::erase(m_input_id);
    }

    bool Player::hurt(Object* culprit) {
        if (!culprit) return false;
        if (m_time_left_hurt > 0) {
            console::log(class_name(), "::hurt() still hurting: ", m_time_left_hurt, "\n");
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        Vec2F add_to_position = { 0.0F, 0.0F };

        if (culprit->type() == Type::particle_melee) {
            if (!culprit->parent() or culprit->parent() == this) {
                return false;
            }
            if (culprit->parent()->type() == Type::frog) {
                health::amount_add(m_health_id, -16.0f);

                if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
                else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
                if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
                else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
            }
            console::log(class_name(), "::hurt() culprit: ", to_string(culprit->parent()->type()), " health: ", health::amount(m_health_id), "\n");
        } else if (culprit->type() == Type::particle_shot) {            
            health::amount_add(m_health_id, -16.0f);
            console::log("\n\nentity::Player::hurt() culprit: ", to_string(culprit->type()), " health: ", health::amount(m_health_id), "\n");
        }
        position_add(add_to_position);
        return true;
    }

    void Player::interact(Object* object) {
        if (!object or m_time_left_interacting > 0) return;

        m_time_left_interacting = m_time_to_interact;

        if (object->type() == Type::trigger and is_pressed(key_sprint) and !m_is_carrying) {
            m_time_left_lever = m_time_to_lever;
            reset_anim("lever");
            sprite::is_leftward(m_sprite_id, object->is_dead());

            if (object->is_dead()) {
                object->time_left_alive(U16_MAX);
                object->time_left_dead(0);
            } else {
                object->time_left_dead(U16_MAX);
                object->time_left_alive(0);
            }

            sound_position("lever", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("lever");
        }
        else if (object->type() == Type::brick or object->type() == Type::bug) {
            //if (is_pressed(key_sprint)) return;

            if (object->state() == state::Type::swim) return;

            if (!m_is_carrying and !object->parent()) {
                console::log(class_name(), "::interact pick up\n");
                m_is_carrying = true;
                object->parent(this);
                object->next_state(state::Type::carried);
                sprite::layer(object->sprite_id(), sprite::layer(m_sprite_id) + 1);
                sound_position("pick_up", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("pick_up");
            }
            else if (object->parent() and m_is_carrying) {
                console::log("Player::interact toss\n");
                if (sprite_is_leftward() and object->is_near_wall_L()) {
                    console::log(class_name(), "::interact nah, near wall L\n");
                    return;
                }
                if (!sprite_is_leftward() and object->is_near_wall_R()) {
                    console::log(class_name(), "::interact nah, near wall R\n");
                    return;
                }
                m_is_carrying = false;
                sprite::layer(object->sprite_id(), object->start_layer());
                object->parent(nullptr);
                object->velocity(velocity() + moved_velocity());
                //object->position_add_x(object->velocity().x);
                if (sprite_is_leftward()) {
                    object->velocity_add_x(-2.0F);
                    object->position_add_x(-8.0F);
                }
                else {
                    object->velocity_add_x(2.0F);
                    object->position_add_x(8.0F);
                }
                //console::log("object->velocity().x: ", object->velocity().x, "\n");

                object->velocity_add_y(-1.5F);
                object->position_add_y(-1.0F);
                object->next_state(state::Type::tossed);
                //object->time_left_interacting(10);

                sound_position("toss", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("toss");
            }
        }
        else if (object->type() == Type::clip_ledge) {            
            m_is_climbing_ledge = true;
            m_is_sliding_wall = false;
            input::press(m_input_id, key_up);
            m_is_wall_to_left = sprite::is_leftward(m_sprite_id);
            //position().y = object->other_rect.y - 8;
            velocity_y(0.0F);
            reset_anim("ledge_climb");
        }
    }

    void Player::spawn_down_thrust(cVec2F position) {
        cVec2F dimensions = { 32.0F, 32.0F };
        cF32 speed = 3.0F;

        particle::cType type = particle::Type::down_thrust;

        cVec2F p = { position.x - dimensions.x / 2.0F, position.y - dimensions.y / 2.0F };

        particle::spawn({ .parent = this,
                          .type = type,
                          .position = p,
                          .velocity = { -speed, 0.0F } });
        particle::spawn({ .parent = this,
                          .type = type,
                          .position = p,
                          .velocity = {  speed, 0.0F } });

    }

    bool is_mouse_steer = false;

    void Player::update() {
        for (auto& i : m_time_left_colliding_with) {
            if (i.second > 0) {
                --i.second;
            }
        }
        //console::log(class_name(), "::update() water line y: ", m_water_line_y, "\n");
        if (m_time_left_hurt > 0)        --m_time_left_hurt;        
        if (m_time_left_interacting > 0) --m_time_left_interacting;
        
        if (m_is_sliding_wall) {
            ++m_time_sliding_wall;
        }
        else {
            m_time_sliding_wall = 0;
        }


        state_update();

        //console::log("Player::update() anim source x: ", anim::source(m_current_anim_id).x, "\n");
        sprite_source_rect(anim::source(m_current_anim_id));

        health_layer(m_start_layer);
        if (health_amount() <= 0.0f) {
            m_next_state = state::Type::dead;
        }

        //console::log("is_carrying: ", m_is_carrying, "\n");
        //console::log("is to write save: ", m_is_to_write_save, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            //console::log("time_left_in_state: ", (int)m_time_left_in_state, "\n");
        }
        //console::log("level pos: ", position_difference(level_transform_id).x, "\n");

        if (is_pressed(input::Key::m)) {
            release(input::Key::m);
            is_mouse_steer = !is_mouse_steer;
            release(key_up);
            release(key_down);
            release(key_right);
            release(key_left);
            release(key_melee);
            release(key_jump);
            release(key_sprint);
        }

        if (!is_mouse_steer) return;

        cF32 mouse_up = app::config::extent().y / 2.0F - 32.0F;
        cF32 mouse_down = app::config::extent().y / 2.0F + 32.0F;
        cF32 mouse_left = app::config::extent().x / 2.0F - 40.0F;
        cF32 mouse_right = app::config::extent().x / 2.0F + 40.0F;
        cVec2F sprint_limit = { 80.0F, 32.0F };

        release(key_up);
        release(key_down);
        release(key_right);
        release(key_left);
        release(key_melee);
        release(key_jump);
        release(key_sprint);

        if (input::mouse.x < mouse_left) {
            press(key_left);
            if (app::config::extent().x / 2.0F - input::mouse.x > sprint_limit.x) {
                press(key_sprint);
            }
        } else if (input::mouse.x > mouse_right) {
            press(key_right);
            if (input::mouse.x - app::config::extent().x / 2.0F > sprint_limit.x) {
                press(key_sprint);
            }
        }
        if (input::mouse.y < mouse_up) {
            press(key_up);
            if (app::config::extent().y / 2.0F - input::mouse.y > sprint_limit.y) {
                press(key_sprint);
            }
        } else if (input::mouse.y > mouse_down) {
            press(key_down);
            if (input::mouse.y - app::config::extent().y / 2.0F > sprint_limit.y) {
                press(key_sprint);
            }
        }
        if (is_pressed(input::Button::left)) {
            press(key_melee);
        }
        if (is_pressed(input::Button::right)) {
            press(key_jump);
        }
    }
}