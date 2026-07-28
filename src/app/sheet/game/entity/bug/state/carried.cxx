module entity.bug;

namespace entity {
    void Bug::state_carried(cF32 dt) {
        if (!m_parent) {
            m_next_state = state::Type::walk;
            return;
        }
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_saved_state = m_prev_state;
            m_is_near_wall_L = m_is_near_wall_R = false;
        }
        m_is_on_ground = false;

        console::log("entity::Bug::carried ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");


        velocity(m_parent->velocity());
        move_velocity({});
        if (sprite::is_leftward(m_parent->sprite())) {
            if (position().x < m_parent->position().x - 12.0F) {
                position_x(m_parent->position().x - 12.0F);
            }
            if (position().x > m_parent->position().x - 12.0F) {
                cF32 diff_x = (m_parent->position().x - 12.0F) - position().x;
                velocity_add_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x - 12.0F);
            }
        } else {
            if (position().x > m_parent->position().x + 12.0F) {
                position_x(m_parent->position().x + 12.0F);
            }

            if (position().x < m_parent->position().x + 12.0F) {
                cF32 diff_x = (m_parent->position().x + 12.0F) - position().x;
                velocity_add_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x + 12.0F);
            }
        }

        sprite::is_leftward(m_sprite, position().x + 16.0F < m_parent->position().x + 8.0F);

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
            velocity({ velocity().x, -1.0F });

            if (m_parent->is_on_ground()) {
                m_next_state = m_prev_state;
            } else {
                if (m_prev_state == state::Type::walk) {
                    m_next_state = state::Type::upended;
                } else if (m_prev_state == state::Type::upended) {
                    m_next_state = state::Type::walk;
                }
            }

            m_parent = nullptr;
            m_is_first_state_update = true;
        }
    }
}