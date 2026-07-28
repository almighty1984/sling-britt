module entity.frog;

namespace entity {
    void Frog::state_dive(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("jump");
            sprite_is_hidden(false);
            /*for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }*/
            console::log(class_name(), "::state_dive() ", start_acceleration().x, " ", start_acceleration().y, "\n");
            
            m_is_on_ground = false;
            acceleration(start_acceleration());
            //deceleration(start_deceleration()
            acceleration_x(0.0F);
            deceleration({});
            max_velocity({ 10.0F, 10.0F });

            health_is_hidden(false);
        }
        console::log(class_name(), "::state_dive() velocity: ", velocity().x, "\n");

        velocity_add_y(acceleration().y);

        //F32 radians = std::atan2(velocity().y, velocity().x);
        //if (radians < 0.0F) radians += PI * 2.0F;
        //
        //cF32 degrees = radians * 180.0F / PI;

        //sprite_angle(degrees + 90.0F);
        if (m_is_on_ground) {
            m_next_state = state::Type::idle;
        }
    }
}