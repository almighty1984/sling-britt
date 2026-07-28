module entity.particle.drop;

namespace entity {
    void ParticleDrop::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            if (m_is_on_ground and !m_is_on_slope) {
                sprite_angle(90.0F);
            }
            if (m_parent) {
                m_original_parent_position_on_level = 
                is_level_geometry(m_parent->type()) ? m_parent->start_offset() : m_parent->position_on_level();

                m_original_position_on_level = position_on_level();
            }

            sprite_offset(sprite_start_offset());
            //m_is_near_wall_L = m_is_near_wall_R = false;
            m_time_in_state = 0;
            m_time_left_alive = 0;
            
            if (m_is_on_ground) {
                m_time_left_dead = m_config.time_to_be_dead();
                
            }
            //console::log(class_name(), "::state_dead() time left dead: ", m_time_left_dead, "\n");

            //if (!m_is_near_wall_L and !m_is_near_wall_R) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            //}
            //aabb::is_active(m_aabbs.front(), true);
            //aabb::is_active(m_aabbs.back(), false);

            max_velocity_y(start_max_velocity().y);

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
        }

        max_velocity_y(0.5F);

        //if (!m_is_near_wall_L and !m_is_near_wall_R) {
        velocity_y(0.0F);
        //}



            /*if (m_parent) {
                console::log("parent velocity y: ", m_parent->velocity().y, "\n");
                velocity_add_y(-m_parent->acceleration().y);
            }*/

            //console::log("velocity y: ", velocity().y, "\n");

        //console::log(class_name(), "::state_dead() ", m_time_in_state, "\n");

        if (!m_is_on_ground and m_config.weight() < 1.0F) {
            //console::log(class_name(), "::state_dead() ", m_time_left_dead, "\n");
            if (m_time_left_dead == 1) {
                m_time_left_alive = m_config.time_to_be_alive();
                m_next_state = state::Type::idle;
            }
            return;
        }

        if (m_parent) {
            cVec2F current_parent_pos_on_level =
                   is_level_geometry(m_parent->type()) ? m_parent->start_offset() : m_parent->position_on_level();

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
                case Type::clip_D_50:
                case Type::clip_LD:
                case Type::clip_RD:
                case Type::clip_ledge: {
                    //console::log(class_name(), "::state_dead() parent type: ", to_string(m_parent->type()), "\n");
                    
                    // prevent sticking out over edge
                    if (position_on_level().y < current_parent_pos_on_level.y) {
                        if (sprite_angle() == 90.0F or sprite_angle() == 270.0F) {
                            if (position_on_level().x - 1.0F <= current_parent_pos_on_level.x) {
                                position_x(current_parent_pos_on_level.x - camera::position.x);
                                //m_next_state = state::Type::wall_slide;                                
                            }
                            if (position_on_level().x + 5.0F >= current_parent_pos_on_level.x + 16.0F) {
                                position_x(current_parent_pos_on_level.x + 16.0F - 4.0F - camera::position.x);
                                //m_next_state = state::Type::wall_slide;
                            }
                        }
                    }
                    break;
                }
                case Type::water_line:
                case Type::water_line_L:
                case Type::water_line_R: {
                    position_y(m_parent->position().y - 4.0F);
                    break;
                }
            }
            if (m_parent->state() == state::Type::dead) {
                console::log(class_name(), "::state_dead() entity we stuck to died, go back to idle\n");
                m_parent = nullptr;
                m_next_state = state::Type::idle;
                m_time_left_dead = 0;
                m_time_left_alive = m_config.time_to_be_alive();
                return;
            }
        } else {
            m_original_position_on_level = {};
            m_original_parent_position_on_level = {};
        }

    }
}