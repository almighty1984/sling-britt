module entity.bee;
import random;

namespace entity {
    void Bee::state_charge(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            sound_stop("idle");
            console::log(class_name(), "::state_charge()\n");
            m_time_in_state = 0;
            reset_anim("charge");
            sprite_angle(0.0F);
            sprite_is_upended(false);

            sprite_is_leftward(m_sensed_offset.x < position().x);
        }
        ++m_time_in_state;

        velocity({});

        if (m_time_in_state > 9 + random::number(0, 2)) {
            m_time_in_state = 0;
            m_next_state = state::Type::attack;
        }
    }
}