module entity.bug;

namespace entity {
    void Bug::state_dive(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("upended");
            sprite_is_hidden(false);
            
            console::log(class_name(), "::state_dive() ", start_acceleration().x, " ", start_acceleration().y, "\n");

            m_is_on_ground = false;
            acceleration({ 0.0F, 0.1F });
            //deceleration(start_deceleration()
            acceleration_x(0.0F);
            deceleration({});
            max_velocity({ 10.0F, 10.0F });

            health_is_hidden(false);
        }
        console::log(class_name(), "::state_dive() velocity: ", velocity().x, "\n");

        velocity_add_y(acceleration().y);

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += PI * 2.0F;
        
        cF32 degrees = radians * 180.0F / PI;


        sprite_is_leftward(velocity().x > 0.0F);

        sprite_angle(degrees + 90.0F/* + (velocity().x < 0.0F ? 180.0F : 0.0F)*/);

        if (m_is_on_ground) {
            m_next_state = state::Type::idle;
        }
    }
}