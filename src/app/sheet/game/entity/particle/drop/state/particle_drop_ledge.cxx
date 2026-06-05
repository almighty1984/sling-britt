module entity.particle.drop;
import anim;

namespace entity {
    void ParticleDrop::state_ledge(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //m_is_near_wall_L = m_is_near_wall_R = false;
            m_time_in_state = 0;
            //m_time_left_alive = m_config.time_to_be_alive();
            //m_time_left_dead = 0;

            if (m_parent) {
                m_original_parent_position_on_level =
                    is_level_geometry(m_parent->type()) ? m_parent->start_offset() : m_parent->position_on_level();

                m_original_position_on_level = position_on_level();
            }

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }

            F32 abs_vel_y = velocity().y / 1.0F;
            F32 pitch = 1.0F;
            if (abs_vel_y > 1.0F) {
                if (abs_vel_y > 1.5F) {
                    abs_vel_y = 1.5F;
                }
                pitch = abs_vel_y;
            }
            sound_pitch("dead", pitch + random::number(-10, 10) / 100.0F);
            sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });
            sound_play("dead");

            reset_anim("dead");
            m_time_left_alive = m_config.time_to_be_alive();
            m_time_left_dead = 0;

            //anim::first_frame(m_current_anim, 0);
        }

        set_anim("dead");
        //console::log(class_name(), "::state_ledge() ", anim::source(m_current_anim).x, "\n");
        max_velocity_y(0.5F);
        velocity_y(0.0F);

        ++m_time_in_state;
        
        if (m_time_in_state > m_time_to_be_in_state) {
            m_time_in_state = 0;
            m_next_state = state::Type::slide_wall;

            //sprite_is_hidden(true); // FIXME: prevent flashing the wrong anim, seems to work but...
            /*if (m_is_near_wall_L) {
                m_original_position_on_level.x -= 2.0F;
            }*/
            return;
            //console::log("sprite_angle(): ", sprite_angle(), "\n");
        }


        if (m_parent) {
            cVec2F current_parent_pos_on_level = is_level_geometry(m_parent->type()) ? m_parent->start_offset() :
                m_parent->position_on_level() + m_parent->velocity();

            cVec2F parent_diff = current_parent_pos_on_level - m_original_parent_position_on_level;

            if (m_original_parent_position_on_level.x != 0.0F) {
                position_x(m_original_position_on_level.x + parent_diff.x - camera::position.x);
            }
            if (m_original_parent_position_on_level.y != 0.0F) {
                position_y(m_original_position_on_level.y + parent_diff.y - camera::position.y);
            }
            switch (m_parent->type()) {
            case Type::brick:
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_ledge: {
                //console::log(class_name(), "::state_dead() parent type: ", to_string(m_parent->type()), "\n"); 
                
                /*if (position_on_level().y < current_parent_pos_on_level.y) {
                    position_y(current_parent_pos_on_level.y - camera::position.y);
                }
                if (position_on_level().y + 0.0F > current_parent_pos_on_level.y + 16.0F) {
                    position_y(current_parent_pos_on_level.y + 16.0F - 0.0F - camera::position.y);
                }*/

                break;
            }
            case Type::water_line:
            case Type::water_line_L:
            case Type::water_line_R: {
                position_y(m_parent->position().y - 0.0F);
                break;
            }
            }
            if (m_parent->state() == state::Type::dead) {
                console::log(class_name(), "::state_dead() brick died, go back to idle\n");
                m_parent = nullptr;
                m_next_state = state::Type::idle;
                m_time_left_dead = 0;
                m_time_left_alive = m_config.time_to_be_alive();
                return;
            }
        }

    }
}