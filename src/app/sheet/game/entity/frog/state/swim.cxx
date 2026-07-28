module entity.frog;
import random;

namespace entity {
    void Frog::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed.clear();
            m_is_on_ground = false;
            m_parent = nullptr;
            reset_anim("idle");
            line::is_hidden(m_tounge_line, true);

            direction_x(random::number(0, 1) ? -1.0F : 1.0F);
        }
        //console::log("num jumps: ", (int)m_num_jumps, "\n");

        //console::log(class_name(), "::state_swim() ", velocity().x, "\n");

        //anim::speed(m_current_anim, 0.0F);
        sprite_is_upended(false);
        if (direction().x < 0.0F and velocity().x > -0.2F) {
            velocity_add_x(-0.05F);            
        } else if (direction().x > 0.0F and velocity().x < 0.2F){
            velocity_add_x(0.05F);
        }
        sprite_is_leftward(direction().x < 0.0F);

        
        //deceleration({});
        //acceleration(start_acceleration());
        deceleration({ 0.001F, 0.1F });
        max_velocity({ 4.0F, 4.0F });

        cF32 diff_y = m_water_line_y - (position_on_level().y + 8.0F);
        velocity_add_y(diff_y / 30.0F);
        velocity_y(velocity().y * 0.99F);

        sprite_angle(degrees() + 270.0F);
        //sprite_angle_add(1.0F);

        //console::log(class_name(), "::state_swim() angle: ", sprite_angle(), "\n");
    }
}