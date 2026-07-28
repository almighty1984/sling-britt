module entity.brick;

namespace entity {
    void Brick::state_carried(cF32 dt) {
        if (!m_parent) {
            m_next_state = state::Type::idle;
            return;
        }
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_weight = 0.0F;
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
        if (m_parent->state() == state::Type::duck) {
            position_add_y(4.0F);
        }
        if (/*m_parent->state() == state::Type::duck or*/ !m_parent->is_carrying()) {
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
}