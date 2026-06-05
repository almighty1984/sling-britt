module entity.brick;
import app.config;
import aabb;
import particle_system;

namespace entity {
    void Brick::state_carried(cF32 dt) {
        if (!m_parent) {
            m_next_state = state::Type::idle;
            return;
        }
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        m_is_on_ground = false;

        //console::log(class_name(), "::carried is near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
        //console::log(class_name(), "::carried ", velocity().x, " ", velocity().y, "\n");

        velocity(m_parent->velocity());
        move_velocity({});
        cF32 carry_offset_x = 10.0F;
        if (sprite::is_leftward(m_parent->sprite())) {
            if (position().x < m_parent->position().x - carry_offset_x) {
                position_x(m_parent->position().x - carry_offset_x);
            }
            if (position().x > m_parent->position().x - carry_offset_x) {
                cF32 diff_x = (m_parent->position().x - carry_offset_x) - position().x;
                velocity_add_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x - carry_offset_x);
            }
        } else {
            if (position().x > m_parent->position().x + carry_offset_x) {
                position_x(m_parent->position().x + carry_offset_x);
            }

            if (position().x < m_parent->position().x + carry_offset_x) {
                cF32 diff_x = (m_parent->position().x + carry_offset_x) - position().x;
                velocity_add_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x + carry_offset_x);
            }
        }

        sprite_is_leftward(position().x + 16.0F < m_parent->position().x + 8.0F);

        position_y(m_parent->position().y + 2.0F);
        /*if (m_parent->is_ducking()) {
            position_add_y(4.0F);
        }*/
        if (m_parent->state() == state::Type::duck or !m_parent->is_carrying()) {
            m_parent->is_carrying(false);
            velocity({ m_parent->velocity().x, velocity().y });
            if (sprite::is_leftward(m_sprite)) {
                velocity_add_x(-0.5F);
                position_add_x(-8.0F);
            } else {
                velocity_add_x(0.5F);
                position_add_x(8.0F);
            }
            velocity_y(-1.0F);
            m_parent = nullptr;
            m_is_first_state_update = true;
            m_next_state = state::Type::idle;
        }
    }
    void Brick::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_config.time_to_be_dead();
            m_time_left_alive = 0;
            console::log(class_name(), "::state_dead()\n");
            if (m_parent) {
                m_parent->is_carrying(false);
                m_parent = nullptr;
            }
            reset_anim("dead");

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sprite_is_hidden(true);
            //particle::spawn(this, particle::Type::hit, position(), {});

            console::log(class_name(), "::dead velocity.x ", velocity().x, "\n");
        }
        velocity({});
        move_velocity({});        
        deceleration(start_deceleration());

        set_anim("dead");

        //console::log(class_name(), "::state_dead() start position: ", start_position().x, " ", start_position().y, "\n");

        if (m_time_left_dead > 0 and m_config.time_to_be_dead() != U16_MAX) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log(class_name(), "::dead done being dead\n");                
            }
        }
    }
    void Brick::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
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


        

        if (m_parent and m_parent->state() == state::Type::idle) {
            //console::log("inputs: ", m_input_objects.size(), "\n");

            //if (position().y + 16.0F < m_parent->position().y + 2.0F) {
                //move_velocity({});
                velocity_x(m_parent->velocity().x);
                velocity_y(m_parent->velocity().y);
                //deceleration({ 0.0F, 0.0F });
            //}
            if (position().y + 16.0F > m_parent->position().y + 2.0F) {
                console::log(class_name(), "::state_idle() parent: null\n");
                m_parent = nullptr;
                m_is_on_ground = false;                
                //deceleration(start_deceleration());
                //m_input_objects.clear();
            }
            //if (velocity().y > acceleration().y) {
                //m_is_on_ground = false;
            //}
        }

        

        // Inputs are objects stacked on top
        for (auto it = m_input_objects.begin(); it != m_input_objects.end(); ++it) {
            if (!*it) continue;

            if (//(*it)->position().y + 16.0F < position().y or
                (*it)->position().y + 16.0F > position().y + 2.0F or
                (*it)->position().x >= position().x + 16.0F or
                (*it)->position().x + 16.0F <= position().x
                ) {
                //console::log(class_name(), "::state_idle() brick erase input\n");
                it = m_input_objects.erase(it);
                if (it == m_input_objects.end()) {
                    break;
                }
            }
        }
        /*if (m_is_on_ground) {
            deceleration_x(0.2F);
        }*/

        //++m_time_in_state;
        deceleration_x(0.02F);
        if (m_is_on_ground) {
            //if (m_time_in_state > 2) {
                //m_time_in_state = 0;
            force_y((m_input_objects.size() + 1.0F) / 4.0F);

            deceleration_x(force().y);

            //console::log("force y: ", force().y, "\n");
            //if (velocity().y > acceleration().y) {
            //m_input_objects.clear();
            //}

            //velocity_x(velocity().x * (1.0F - force().y));
        //}
        //if (num_inputs() == 0) {            
            //deceleration(start_deceleration());
        //}
        }

    }
    void Brick::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        acceleration({ 0.05F, 0.05F });
        max_velocity({ 1.0F, 1.0F });
    }
    void Brick::state_tossed(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;

            deceleration({ 0.0F, 0.0F });
            m_parent = nullptr;
        }
        if (m_is_on_ground) {
            deceleration({ 0.1F, 0.0F });
            m_next_state = state::Type::idle;
        } else {
            //console::log(class_name(), " toss not on ground\n");
        }
    }
}