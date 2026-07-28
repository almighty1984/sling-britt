module entity.mole;

namespace entity {
    void Mole::state_enter(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("enter");
            m_time_in_state = 0;

            sprite_offset_y(-8.0F);

            health_is_hidden(false);
        }
        //console::log(class_name(), "::enter()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim("enter"))) {
            m_next_state = state::Type::idle;
        }
    }
}