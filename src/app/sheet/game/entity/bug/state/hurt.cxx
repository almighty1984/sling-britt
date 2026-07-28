module entity.bug;

namespace entity {
    void Bug::state_hurt(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("hurt");            
        }
        m_parent = nullptr;
        //console::log("entity::Bug::hurt()\n");
        deceleration({});
        set_anim("hurt");
        if (anim::is_last_frame(m_current_anim)) {
            m_next_state = state::Type::upended;
        }
    }
}