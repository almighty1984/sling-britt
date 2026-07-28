module entity.particle.drop;

namespace entity {    
    void ParticleDrop::state_wall_slide(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //console::log(class_name(), "::state_wall_slide()\n");
            m_is_on_ground = false;
            m_time_in_state = 0;
            m_time_left_alive = m_config.time_to_be_alive();

            for (auto& i : m_aabbs) {
                aabb::is_active(i, aabb::name(i) == aabb::Name::body_wall);
            }
        }
        set_anim("idle");
        sprite_angle(90.0F);
        if (m_time_in_state % 10 == 0) {
            max_velocity_y((m_inputs.size() + 1) * m_config.weight() * random::real_number(0.2, 0.3));
        }
        if (max_velocity().y > 5.0F) {
            max_velocity_y(5.0F);
        }

        //if (m_is_near_wall_L) {
        //    console::log("wall L\n");
        //}
        //if (m_is_near_wall_R) {
        //    console::log("wall R\n");
        //}

        velocity_x(0.0F);

        if (m_parent) {
            //move_velocity(m_parent->velocity());
            //if (m_parent->type() == Type::brick) {
                //console::log("parent position x: ", m_parent->position().x, "\n");

            cVec2F parent_pos_on_level = (is_level_geometry(m_parent->type()) ? m_parent->start_offset() : m_parent->position_on_level() + m_parent->velocity());

            cVec2F parent_diff = parent_pos_on_level - m_original_parent_position_on_level;

            /*if (parent_diff.y > 16.0F) {
                m_next_state = state::Type::idle;
                return;
            }*/
            if (m_original_parent_position_on_level.x != 0.0F) {
                //console::log(class_name(), "::state_wall_slide() parent: ", to_string(m_parent->type()), "\n");                
                //console::log(class_name(), "::state_wall_slide() parent position diff x: ", parent_diff_x, "\n");                
                F32 offset_x = 0.0F;
                if (m_is_near_wall_L) {
                    offset_x = -1.0F;
                }
                if (m_is_near_wall_R) {
                    offset_x = 1.0F;
                }
                position_x(m_original_position_on_level.x + parent_diff.x + offset_x - camera::position.x + m_parent->velocity().x);

                sprite_is_hidden(false);
            }

            if (m_original_parent_position_on_level.y != 0.0F) {
                //console::log(class_name(), "::state_dead() parent position diff y: ", parent_diff.y, "\n");                
                //m_original_position_on_level.y += velocity().y;

                if (m_parent->type() == Type::brick) {
                    //m_original_position_on_level.y += m_parent->velocity().y;
                    m_original_position_on_level.y += velocity().y;
                    //position_y(m_original_position_on_level.y + parent_diff.y - camera::position.y + m_parent->velocity().y + velocity().y);
                    position_y(m_original_position_on_level.y + parent_diff.y - camera::position.y);

                    //TODO check if too big diff y, then go to idle
                }

                //move_velocity_y(m_parent->velocity().y);
                //position_y(m_original_position_on_level.y + parent_diff.y - camera::position.y);
                //move_velocity_y(m_parent->velocity().y);
            } 
            if (m_parent->state() == state::Type::dead) {
                console::log(class_name(), "::state_wall_slide() brick died, go back to idle\n");
                m_parent = nullptr;
                m_next_state = state::Type::idle;
            }

            //console::log(class_name(), "::state_wall_slide() parent: ", to_string(m_parent->type()), "\n");

            if (velocity().y > 0.0F and position_on_level().y > m_wall_drop_on_level_y and m_time_in_state > 5) {
                //console::log(class_name(), "::state_wall_slide() drop y: ", m_wall_drop_on_level_y, "\n");
                m_parent = nullptr;
                //velocity_y(0.0F);
                m_next_state = state::Type::idle;
            }
        }
        else {
            //m_original_position_on_level = {};
            //m_original_parent_position_on_level = {};

            /*if (m_is_near_wall_L) {
                position_x(m_on_level_wall_x - 1.0F - camera::position.x - 0.0F);
            } else if (m_is_near_wall_R) {
                position_x(m_on_level_wall_x - 4.0F - camera::position.x + 0.0F);
            }*/
        }

        ++m_time_in_state;
    }
}