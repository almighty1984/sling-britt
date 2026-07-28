module entity.particle.rock;
import particle_system;

namespace entity {
    void ParticleRock::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_config.time_to_be_dead();

            /*for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }*/
            sprite_is_hidden(true);

            //m_radians += (2 * PI);

            m_radians = std::atan2f(velocity().y, velocity().x);
            if (m_radians < 0.0F) m_radians += (2 * PI);

            cF32 angle = m_radians * (180 / PI);

            console::log(class_name(), "::state_dead() angle: ", angle, "\n");

            cF32 start_angle = angle - 45.0F;
            cF32 end_angle = angle + 45.0F;
            cF32 speed = 1.0F;
            particle::spawn_fan(this,
                start_angle, end_angle, 3,
                particle::Type::pebble,
                position(), velocity() * 0.1F, speed,
                state::Type::idle);

            //int num_to_spawn = 8;
            //F32 radian_offset = -(PI / num_to_spawn);

            /*for (int i = 0; i < num_to_spawn; i++) {
                particle::spawn(this, particle::Type::pebble, position(), Vec2F{ std::cosf(m_radians + radian_offset) * speed, std::sinf(m_radians + radian_offset) * speed });
                radian_offset += (PI / num_to_spawn);
            }*/
            //state::Type::idle
        }
        velocity({});
        if (m_parent and is_water_line(m_parent->type())) {
            position_y(m_parent->position().y - 8.0F);
        }
    }
}