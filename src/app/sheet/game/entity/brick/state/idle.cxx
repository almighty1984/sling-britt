module entity.brick;

namespace entity {
    void Brick::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_weight = m_config.weight();
            m_time_in_state = 0;
            m_time_left_alive = U16_MAX;

            m_parent = nullptr;

            sprite_is_hidden(false);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
            velocity({});
            move_velocity({});
            acceleration(start_acceleration());
            max_velocity(start_max_velocity());
        }

        /*if (move_velocity().x < 0.0F) {
            move_velocity_x(move_velocity().x + deceleration().x);
        }
        else if (move_velocity().x > 0.0F) {
            move_velocity_x(move_velocity().x - deceleration().x);
        }*/


        //move_velocity({});


        if (m_parent and m_parent->state() == state::Type::idle) {
            //console::log("inputs: ", m_inputs.size(), "\n");

            //if (position().y + 16.0F < m_parent->position().y + 2.0F) {
                //move_velocity({});
            velocity_x(m_parent->velocity().x);
            velocity_y(m_parent->velocity().y);
            //deceleration({ 0.0F, 0.0F });
        //}
            if (position().y + 16.0F > m_parent->position().y + 2.0F or
                position().y + 16.0F < m_parent->position().y - 2.0F) {
                console::log(class_name(), "::state_idle() parent: null\n");
                m_parent = nullptr;
                m_is_on_ground = false;
                //deceleration(start_deceleration());
                //m_inputs.clear();
            }
            //if (velocity().y > acceleration().y) {
                //m_is_on_ground = false;
            //}
        }


        //console::log(class_name(), "::state_idle() num inputs: ", m_inputs.size(), "\n");

        // Inputs are objects stacked on top
        //for (auto it = m_inputs.begin(); it != m_inputs.end(); ++it) {
        //    if (!*it) continue;

        //    if ((*it)->position().y + 16.0F < position().y - 2.0F or
        //        (*it)->position().y + 16.0F > position().y + 2.0F or
        //        (*it)->position().x >= position().x + 16.0F or
        //        (*it)->position().x + 16.0F <= position().x
        //        ) {
        //        //console::log(class_name(), "::state_idle() brick erase input\n");
        //        it = m_inputs.erase(it);
        //        if (it == m_inputs.end()) {
        //            break;
        //        }
        //    }
        //}
        /*if (m_is_on_ground) {
            deceleration_x(0.2F);
        }*/


        ++m_time_in_state;
        //deceleration_x(0.2F);
        if (m_is_on_ground) {
            //if (m_time_in_state > 2) {
                //m_time_in_state = 0;
            force_y((m_inputs.size() + 1.0F) * 1.0F);

            //deceleration_x(force().y);

            //console::log("force y: ", force().y, "\n");
            //if (velocity().y > acceleration().y) {
            //m_inputs.clear();
            //}

            //velocity_x(velocity().x * (1.0F - force().y));
        //}
        //if (num_inputs() == 0) {            
            //deceleration(start_deceleration());
        //}
        }
        if (m_time_in_state > 2) {
            m_time_in_state = 0;
            m_inputs.clear();
        }
    }
}