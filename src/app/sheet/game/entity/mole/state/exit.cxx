module entity.mole;

namespace entity {
    void Mole::state_exit(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("exit");
            sprite_offset_y(-8.0F);

            health_is_hidden(false);
        }
        //console::log(class_name(), "::exit()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim("exit"))) {
            if (m_sensed_offset.x == 0.0F and m_sensed_offset.y == 0.0F) {
                m_next_state = state::Type::enter;
            } else {
                m_next_state = state::Type::shoot;
            }
        }
    }
}