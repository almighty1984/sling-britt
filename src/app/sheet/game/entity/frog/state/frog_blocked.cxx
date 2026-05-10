module entity.frog;

namespace entity {
    void Frog::state_blocked(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            reset_anim("hurt");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_in_state = 0;
            transform::velocity(m_transform, { sprite::is_leftward(m_sprite) ? 1.0f : -1.0f, -1.5f });

            m_is_on_ground = false;

            sound_position("block", { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
            sound_play("block");
        }
        //console::log("Frog::blocked\n");

        set_anim("hurt");
        m_tounge_start = transform::position(m_transform) + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };

        line::max_length(m_tounge_line, 32.0F - m_time_in_state * 4.0f);
        if (line::max_length(m_tounge_line) < 1.0f) {
            line::max_length(m_tounge_line, 1.0f);
        }

        line::set(m_tounge_line, m_tounge_start, m_tounge_end);

        transform::velocity_add(m_transform, { 0.0F, transform::acceleration(m_transform).y });

        ++m_time_in_state;
        if (m_is_on_ground and m_time_in_state > 100) {
            m_time_in_state = 0;
            m_next_state = state::Type::idle;
        }
    }
}