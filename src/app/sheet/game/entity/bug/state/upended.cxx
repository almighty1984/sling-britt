module entity.bug;

namespace entity {
    void Bug::state_upended(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        set_anim("upended");

        if (m_is_on_ground) {
            deceleration({ 0.2F, 0.0F });
        } else {
            deceleration({ 0.0F, 0.0F });
        }
        m_parent = nullptr;

        if (m_time_left_until_next_state > 0) {
            --m_time_left_until_next_state;
            if (m_time_left_until_next_state == 0) {
                m_next_state = state::Type::walk;
            }
        }
    }
}