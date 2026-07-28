module entity.particle.melee;

namespace entity {
    void ParticleMelee::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_config.time_to_be_dead();
        }
        //console::log(class_name(), "::state_dead() time left dead: ", m_time_left_dead, "\n");
        velocity({});
    }
}