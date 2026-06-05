module entity.particle.drop;

namespace entity {
    void ParticleDrop::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;
            m_is_near_wall_L = m_is_near_wall_R = false;
            m_time_in_state = 0;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            max_velocity_y(start_max_velocity().y);
            acceleration_y(start_acceleration().y);

            sprite_offset(sprite_start_offset());

            reset_anim("idle");
        }

        set_anim("idle");
        //velocity_add_y(acceleration().y);

        ++m_time_in_state;

        if (m_time_in_state == 4) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, aabb::name(i) == aabb::Name::body);
            }
            //console::log("angle: ", sprite::angle(m_sprite), "\n");
        }
        F32 radians = std::atan2f(velocity().y, velocity().x);
        if (radians < 0.0F) radians += (2 * PI);
        cF32 degrees = radians * 180 / PI;
        sprite_angle(degrees);
    }
}