module entity.particle.down_thrust;

namespace entity {
    void ParticleDownThrust::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        velocity({});
        //console::log(class_name(), "::state_dead()\n");
    }
}