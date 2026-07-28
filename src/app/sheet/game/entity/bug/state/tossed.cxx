module entity.bug;

namespace entity {
    void Bug::state_tossed(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;

            deceleration({});
        }
        if (m_is_on_ground) {
            console::log(class_name(), "::tossed, on ground state: ", to_string(m_saved_state), "\n");
            //m_next_state = state::Type::walk;
            //m_next_state = state::Type::upended;
            m_next_state = m_saved_state;
        } else {
            //console::log("entity::Bug toss not on ground\n");
        }
    }
}