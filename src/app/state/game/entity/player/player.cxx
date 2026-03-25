module;
#include <fstream>
#include <sstream>
module entity.player;
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
        m_state = m_next_state = State::run;

        load_config("res/entity/player/britt.cfg");
    }

    Player::~Player() {
        console::log("Player::~Player() input\n");
        input::erase(m_input_id);
    }

    bool Player::hurt(Object* culprit) {
        if (!culprit) return false;
        if (m_time_left_hurt > 0) {
            console::log("entity::Player::hurt() still hurting: ", m_time_left_hurt, "\n");
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        if (culprit->type() == Type::particle_melee) {
            if (!culprit->parent() || culprit->parent() == this) {
                return false;
            }
            if (culprit->parent()->type() == Type::frog) {
                health::amount_add(m_health_id, -16.0f);
            }
            console::log("entity::Player::hurt() culprit: ", to_string(culprit->parent()->type()), " health: ", health::amount(m_health_id), "\n");
        } else if (culprit->type() == Type::particle_shot) {            
            health::amount_add(m_health_id, -16.0f);
            console::log("\n\nentity::Player::hurt() culprit: ", to_string(culprit->type()), " health: ", health::amount(m_health_id), "\n");
        }
        return true;
    }

    void Player::interact(Object* object) {
        if (!object || m_time_left_interacting > 0) return;

        m_time_left_interacting = m_time_to_interact;

        if (object->type() == Type::trigger && is_pressed(key_sprint) && !m_is_carrying) {
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

            sound_position("lever", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("lever");
        }
        else if (object->type() == Type::brick || object->type() == Type::bug) {
            //if (is_pressed(key_sprint)) return;

            if (object->state() == State::swim) return;

            if (!m_is_carrying && !object->parent()) {
                console::log("entity::Player::interact pick up\n");
                m_is_carrying = true;
                object->parent(this);
                object->next_state(State::carried);
                sprite::layer(object->sprite_id(), sprite::layer(m_sprite_id) + 1);
                sound_position("pick_up", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("pick_up");
            }
            else if (object->parent() && m_is_carrying) {
                console::log("Player::interact toss\n");
                if (sprite_is_leftward() && object->is_near_wall_L()) {
                    console::log("entity::Player::interact nah, near wall L\n");
                    return;
                }
                if (!sprite_is_leftward() && object->is_near_wall_R()) {
                    console::log("entity::Player::interact nah, near wall R\n");
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
                object->next_state(State::tossed);
                //object->time_left_interacting(10);

                sound_position("toss", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
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


    void Player::update() {
        for (auto& i : m_time_left_colliding_with) {
            if (i.second > 0) {
                --i.second;
            }
        }
        //console::log(class_name(), "::update() water line y: ", m_water_line_y, "\n");
        if (m_time_left_hurt > 0) {
            //console::log("entity::Player::update() time left hurt: ", m_time_left_hurt, "\n");
            --m_time_left_hurt;
        }
        if (m_time_left_interacting > 0) {
            --m_time_left_interacting;
        }
        if (m_is_sliding_wall) {
            ++m_time_sliding_wall;
        }
        else {
            m_time_sliding_wall = 0;
        }

        if (m_next_state != m_state) {
            m_prev_state = m_state;
            m_state = m_next_state;
            m_is_first_state_update = true;
        }
        switch (m_state) {
            case State::dead:  state_dead();  break;
            case State::dive:  state_dive();  break;
            case State::ledge: state_ledge(); break;
            case State::run:   state_run();   break;
            case State::swim:  state_swim();  break;
            case State::sling: state_sling(); break;
            default:                          break;
        }

        //console::log("Player::update() anim source x: ", anim::source(m_current_anim_id).x, "\n");
        sprite_source_rect(anim::source(m_current_anim_id));

        health_layer(m_start_layer);
        if (health_amount() <= 0.0f) {
            m_next_state = State::dead;
        }

        //console::log("is_carrying: ", m_is_carrying, "\n");
        //console::log("is to write save: ", m_is_to_write_save, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            //console::log("time_left_in_state: ", (int)m_time_left_in_state, "\n");
        }
        //console::log("level pos: ", position_difference(level_transform_id).x, "\n");
    }
}