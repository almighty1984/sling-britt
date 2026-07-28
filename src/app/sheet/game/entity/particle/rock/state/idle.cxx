module entity.particle.rock;
import particle_system;

namespace entity {
    void ParticleRock::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sound_play("toss");
        }

        sound_position("toss", { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F });


        m_radians = std::atan2f(velocity().y, velocity().x);
        if (m_radians < 0.0F) m_radians += (2 * PI);

        cF32 angle = m_radians * (180.0F / PI);

        //console::log("angle: ", angle, "\n");

        //if (m_time_in_state <= 4) {
        ++m_time_in_state;
        //}
        if (m_time_in_state == 2) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
        }


        if (m_time_in_state > 2) {
            m_time_in_state = 2;
            if (velocity().x < -2.0F or velocity().x > 2.0F or
                velocity().y < -2.0F or velocity().y > 2.0F) {
                particle::spawn(this, particle::Type::trail_smoke, position() - Vec2F{ 4.0F, 4.0F } - velocity(), {});
            }
        }

        m_radians = std::atan2f(velocity().y, velocity().x);
        if (m_radians < 0.0F) m_radians += (2 * PI);

        cF32 degrees = m_radians * (180 / PI);

        sprite::angle(m_sprite, degrees);
    }
}