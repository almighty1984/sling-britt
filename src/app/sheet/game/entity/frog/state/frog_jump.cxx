module entity.frog;

namespace entity {
    void Frog::state_jump(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_sensed_objects.clear();

            cF32 diff_x = m_sensed_position.x - position().x;
            cF32 diff_y = m_sensed_position.y - position().y;

            //console::log("diff_y: ", diff_y, "\n");
            max_velocity(start_max_velocity());

            velocity_x(diff_x / 64.0F);
            velocity_y(diff_y / 10.0F);

            position_add_y(-2.0F);

            if (velocity().y < -4.0F) {
                velocity_y(-4.0F);
            } else if (velocity().y > -2.0F) {
                velocity_y(-2.0F);
            }
            m_is_on_ground = false;

            reset_anim("jump");
        }

        line::is_hidden(m_tounge_line, true);

        set_anim("jump");

        velocity_add_y(acceleration().y);

        if (m_is_on_ground and velocity().y > 0.0F) {
            m_next_state = state::Type::idle;
            reset_anim("idle");
        }
    }
}