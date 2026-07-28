module entity.particle.melee;

namespace entity {
    void ParticleMelee::state_idle(cF32 dt) {
        sound_position("idle", { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F });
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //console::log("run this\n");
            sound_play("idle");
        }
    }
}