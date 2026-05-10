module entity.frog;

namespace entity {
    void Frog::state_stunned(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();

            m_sensed_position = {};
            reset_anim("stunned");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_left_in_state = m_time_to_be_in_state;
        }
        set_anim("stunned");

        line::is_hidden(m_tounge_line, true);

        velocity_add_y(acceleration().y);

        console::log(class_name(), "::state_stunned() time left: ", m_time_left_in_state, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
        }
        if (m_is_on_ground and m_time_left_in_state == 0) {
            m_time_in_state = 0;
            m_next_state = state::Type::idle;
        }
    }
}