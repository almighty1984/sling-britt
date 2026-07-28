module entity.bee;
import random;

namespace entity {
    void Bee::state_idle(cF32 dt) {        
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            if (m_prev_state == state::Type::none or m_prev_state == state::Type::dead) {
                m_target.x = start_position_on_level().x + (random::number(0, 1) ? -m_range.x : m_range.x);
                m_target.y = start_position_on_level().y + (random::number(0, 1) ? -m_range.y : m_range.y);                
            }

            //m_time_to_be_in_state = 60 + random::number(0, 20);
            m_time_left_until_next_state = 60 + random::number(0, 20);
            //console::log(class_name(), "::state_idle() time to be in state: ", m_time_to_be_in_state, "\n");
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
            sprite_is_hidden(false);
            sprite_angle(0.0F);
            sprite_is_upended(false);
            console::log(class_name(), "::state_idle()\n");
            m_time_in_state = 0;
            m_sensed_offset = {};
            m_time_left_to_spawn_sense = m_config.time_to_spawn_sense();
            set_anim("idle");
            m_sensed.clear();

            F32 rand_acc = 0.04F + random::number(0, 10) / 100.0F;
            console::log(class_name(), "::state_idle() rand acc: ", rand_acc, "\n");

            cVec2F acc = { rand_acc, rand_acc };
            
            acceleration(acc);

            max_velocity({ 1.0F, 1.0F });
            sound_pitch("idle", m_idle_pitch);
        }
                
        sound_position("idle", { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F });

        sound_is_looped("idle", true);
        if (!sound_is_playing("idle")) {
            sound_play("idle");
        }

        ++m_time_in_state;
        //m_radians += dt;
        //console::log("dt: ", std::asinf(m_radians), "\n");

        //console::log("time: ", m_time_in_state, "\n");

        if (position_on_level().x <= start_position_on_level().x - m_range.x) {
            m_target.x = start_position_on_level().x + m_range.x;
            //console::log("target right\n");
        } else if (position_on_level().x >= start_position_on_level().x + m_range.x) {
            m_target.x = start_position_on_level().x - m_range.x;
            //console::log("target left\n");
        }
        if (m_target.x < position_on_level().x) {
            velocity_add_x(-acceleration().x);
        } else if (m_target.x >= position_on_level().x) {
            velocity_add_x(acceleration().x);
        }

        if (position_on_level().y <= start_position_on_level().y - m_range.y) {
            m_target.y = start_position_on_level().y + m_range.y;
            //console::log("target down\n");
        } else if (position_on_level().y >= start_position_on_level().y + m_range.y) {
            m_target.y = start_position_on_level().y - m_range.y;
            //console::log("target up\n");
        }
        if (m_target.y < position_on_level().y) {
            velocity_add_y(-acceleration().y);
        } else if (m_target.y >= position_on_level().y) {
            velocity_add_y(acceleration().y);
        }
        //console::log(position_on_level().x, " ", position_on_level().y, " ", start_position_on_level().x, " ", start_position_on_level().y, "\n");

        //console::log("acc: ", acceleration().x, " ", acceleration().y, "\n");
        
        sprite_is_leftward(velocity().x < 0.0F);

        /*F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += PI * 2.0F;
        cF32 degrees = radians * 180.0F / PI;
        sprite_angle(degrees + 0.0F);
        sprite_is_upended(velocity().x < 0.0F);*/

        //return;

        if (m_time_left_to_react > 0) {
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = state::Type::charge;
            }
            return;
        }

        //if (m_time_in_state < m_time_to_be_in_state) {
        //if (m_time_left_until_next_state == 0) {
            //return;
        //}

        if (m_time_left_to_spawn_sense > 0) {
            --m_time_left_to_spawn_sense;
            if (m_time_left_to_spawn_sense == 0) {
                //console::log("Frog::idle() spawn sense\n");

                m_time_left_to_spawn_sense = m_config.time_to_spawn_sense();

                particle::spawn_fan(this, 0.0F, 360.0F, 32,
                    particle::Type::sense,
                    position() + Vec2F{ 4.0F, 0.0F }, {}, 4.0F);
            }
        }

        if (m_parent) {
            if (m_parent->is_hurting() and m_parent->parent()) {
                console::log(class_name(), "::state_idle() parent hurting\n");
                add_sensed(m_parent->parent());
            }
        }

        if (m_sensed.empty()) return;
        for (auto& i : m_sensed) {
            if (!i or i->is_dead() or i->state() == state::Type::swim) continue;
            if (i->type() == Type::player) {
                //m_sensed_state = state::Type::attack;
                m_sensed_offset = i->position() + Vec2F{ 8.0F, 8.0F };

                //if (m_time_left_to_react == 0) {
                    //m_time_left_to_react = 1;
                //}

                m_time_left_to_react = random::number(1, 40);
                m_sensed.clear();
                console::log(class_name(), "::state_idle() sensed player ", (int)m_time_left_to_react, "\n");
                return;
            }
        }
    }
}