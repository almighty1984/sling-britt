module entity.frog;

namespace entity {
    void Frog::state_heal(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("stunned");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_objects.clear();
            m_time_left_in_state = m_time_left_in_next_state;
            m_time_left_in_next_state = 0;

            line::max_length(m_tounge_line, line::length(m_tounge_line));
        }
        velocity_add_y(acceleration().y);

        console::log(class_name(), "::state_heal() health: ", health_amount(), " ", m_time_left_in_state, "\n");
        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            health::amount_add(m_health_id, 0.1F);

            cVec2F tounge_vector = m_tounge_end - m_tounge_start;
            cF32 tounge_length = line::length(tounge_vector);

            if (tounge_length > 4.0F) {
                cVec2F tounge_normalized = tounge_vector / tounge_length;
                m_tounge_end -= tounge_normalized * 4.0F;
                line::set(m_tounge_line, m_tounge_start, m_tounge_end);
            } else {
                line::is_hidden(m_tounge_line, true);
            }
        }
        if (m_time_left_in_state == 0 or health::is_max(m_health_id)) {
            m_next_state = state::Type::idle;
        }
    }
}