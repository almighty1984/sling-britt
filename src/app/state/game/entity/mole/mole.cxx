module;
#include <cmath>
#include <sstream>

module entity.mole;
import particle_system;

namespace entity {
    bool Mole::hurt(entity::Object* culprit) {
        if (!culprit || m_state == State::swim) return false;

        if (m_time_left_hurt > 0) {
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        if (culprit->type() == Type::brick) {
            health_add_amount(-health_max());
            m_next_state = State::enter;
        }
        else if (culprit->type() == Type::particle_brick) {
            health_add_amount(-1.0f);
            m_next_state = State::enter;
        }
        else if (culprit->type() == Type::particle_down_thrust) {
            if (culprit->parent()) {
                F32 vel_x = (position().x - culprit->parent()->position().x) / 16.0F;
                if      (vel_x < -1.5F) vel_x = -1.5F;
                else if (vel_x > 1.5F) vel_x  =  1.5F;
                console::log(class_name(), "::hurt() vel_x: ", vel_x, "\n");

                velocity_x(vel_x);
            }
            health_add_amount(-1.0f);
            m_next_state = State::jump;
        }
        else if (culprit->type() == Type::particle_melee) {
            if (m_state == State::jump) {
                if (culprit->parent()) {
                    if (sprite::is_leftward(culprit->parent()->sprite_id())) {
                        velocity_x(-5.0F);
                    } else {
                        velocity_x(5.0F);
                    }
                }
                //velocity_y(-1.0F);
            }
            health_add_amount(-16.0f);
        }
        else {
            health_add_amount(-8.0f);
            m_next_state = State::enter;
        }
        sprite_is_leftward(!sprite_is_leftward());

        sound_position("melee", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
        sound_play("melee");

        return true;
    }
    void Mole::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");

            sprite::is_hidden(m_sprite_id, true);

            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this, 0.0F, 360.0F, 8, particle::Type::drop_blood, position() - Vec2F{ 16.0F, 8.0F }, velocity(), 3.0F);

            particle::spawn(this, particle::Type::health, position() + Vec2F{ 0.0F, -4.0F }, {});

            sound_position("dead", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("dead");

            if (m_parent) {
                m_parent->is_carrying(false);
                m_parent = nullptr;
            }
        }
        if (m_time_left_dead > 0 && m_time_to_be_dead != U16_MAX) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log(class_name(), "::state_dead() done being dead\n");
            }
        }
        velocity({});
        moved_velocity({});
    }
    void Mole::state_enter() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("enter");
            m_time_in_state = 0;
        }
        //console::log("entity::Mole::enter()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim_id("enter"))) {
            m_next_state = entity::State::idle;
        }
    }
    void Mole::state_exit() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("exit");
        }
        //console::log("entity::Mole::exit()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim_id("exit"))) {
            m_next_state = entity::State::shoot;
        }
    }
    void Mole::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //sprite_angle(0.0F);
            max_velocity(start_max_velocity());
            m_time_left_alive = U16_MAX;
            
            m_sensed_objects.clear();
            //m_time_left_to_spawn_sense = m_time_to_spawn_sense;
            m_time_left_to_spawn_sense = m_time_to_spawn_sense * 1;
            m_time_left_to_react = 0;
            reset_anim("idle");
            sprite_is_hidden(false);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
            if (m_prev_state == entity::State::dead) {
                health_reset();
                sprite_is_leftward(random::number(0, 1) ? true : false);
            }
        }

        ++m_time_in_state;
        if (m_time_in_state > m_time_to_turn) {
            m_time_in_state = 0;
            m_time_to_turn = 40 + random::number(0, 20);
            sprite_is_leftward(!sprite_is_leftward());
        }

        if (sprite::is_leftward(m_sprite_id)) {
            add_velocity_x(-acceleration().x);
        }
        else {
            add_velocity_x(acceleration().x);
        }

        add_velocity_y(acceleration().y);

        if (!m_is_on_ground) {
            return;

        }
        //console::log(class_name(), "::state_idle() ", m_time_left_to_spawn_sense, "\n");
        if (health_amount() > 0.0F && m_time_left_to_spawn_sense > 0) {
            --m_time_left_to_spawn_sense;
            if (m_time_left_to_spawn_sense == 0) {
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                particle::spawn_fan(this, 170.0F, 370.0F, 16, particle::Type::sense, position() + Vec2F{ 4.0F, 0.0F }, velocity(), 6.0F);

                if (std::abs(velocity().x) > 1.0F) {

                    particle::spawn({ .parent = this,
                                        .type = particle::Type::dust,
                                        .position = position() + Vec2F{4.0F, 18.0F},
                                        .velocity = Vec2F{ 0.0F, 0.0F } });
                }

            }
        }

        for (auto& i : m_sensed_objects) {
            if (!(i->velocity().x > -0.2F && i->velocity().x < 0.2F && i->velocity().y > -1.0F && i->velocity().y < 1.0F) &&
                (i->type() == Type::player || i->type() == Type::brick && i->is_tossed()) //||
                //i->type() == Type::particle_brick
                ) {
                //m_sensed_position = i->position() + Vec2F{ sprite::source_rect(i->sprite_id()).w, sprite::source_rect(i->sprite_id()).h };
                m_sensed_position = i->position();
                if (m_time_left_to_react == 0) {
                    if (i->type() == Type::brick) {
                        m_time_left_to_react = 1;
                    } else {
                        m_time_left_to_react = random::number(10, 40);
                    }
                    console::log(class_name(), "::idle() sensed: ", to_string(i->type()), " time left to react : ", m_time_left_to_react, "\n");
                }
            }
        }

        if (m_time_left_to_react > 0) {
            //console::log("entity::Mole::idle() time left to react: ", m_time_left_to_react, "\n");
            --m_time_left_to_react;
            F32 slowdown = 1.0F - (1.0F / m_time_left_to_react);
            if (slowdown < 0.0F) {
                slowdown = 0.0F;
            }
            //console::log("entity::Mole::idle() slowdown: ", slowdown, "\n");                

            velocity_x(velocity().x * slowdown);
            if (m_time_left_to_react == 0) {
                m_next_state = entity::State::exit;
            }
        }
    }
    void Mole::state_jump() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("jump");
            sprite_is_hidden(false);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
            console::log(class_name(), "::state_jump() ", start_acceleration().x, " ", start_acceleration().y, "\n");
            //velocity_x(0.0F);
            if (!m_is_first_update) {
                velocity_y(-4.0F);
            }

            m_is_on_ground = false;
            acceleration(start_acceleration());
            deceleration(start_deceleration());
            max_velocity({ 10.0F, 10.0F });
        }

        add_velocity_y(acceleration().y);

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

        cF32 degrees = radians * 180.0F / 3.1415926535F;

        sprite_angle(degrees + 90.0F);
        if (m_is_on_ground) {
            m_next_state = State::idle;
        }
    }
    void Mole::state_shoot() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_in_state = 30;
            m_next_state = State::enter;
            m_sensed_objects.clear();

            sprite_is_leftward(m_sensed_position.x < position().x + 4.0F);

            reset_anim("shoot");

            sound_position("shoot", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("shoot");

            cVec2F start_position = position() + Vec2F{ 4.0F, -2.0F };
            cVec2F end_position = m_sensed_position + Vec2F{ 8.0F, 8.0F };
            cVec2F v = (end_position - start_position);

            Vec2F shot_velocity = v / line::length(v) * 8.0F;
            if (shot_velocity.y > -3.0F) {
                shot_velocity.y = -3.0F;
            }
            if (shot_velocity.x < 0.0F && shot_velocity.x > -2.0F) {
                shot_velocity.x = -2.0F;
            } else if (shot_velocity.x > 0.0F && shot_velocity.x < 2.0F) {
                shot_velocity.x = 2.0F;
            }
            particle::spawn({ .parent = this,
                              .type = particle::Type::shot,
                              .position = start_position,
                              .velocity = shot_velocity });
        }
        velocity_x(0.0F);

        //console::log("entity::Mole::shoot() time left in state: ", m_time_left_in_state, "\n");

    }
    void Mole::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
            reset_anim("jump");

            if (velocity().x < 0.0F) {
                added_degrees = 180.0F;
            }
            else {
                added_degrees = 0.0F;
            }
        }

        acceleration({ });
        deceleration({ 0.0F, 0.1F });
        max_velocity({ 4.0F, 4.0F });

        sprite_origin({ 16.0F, 16.0F });

        cF32 diff_y = m_water_line_y - position_on_level().y;
        
        add_velocity_y(diff_y / 30.0F);

        //console::log(class_name(), "::state_swim() degrees: ", degrees(), "\n");

        sprite_angle(degrees() + added_degrees);

        sprite_is_leftward(false);
        sprite_is_upended(false);
    }
}