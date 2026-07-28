module entity.bug;

namespace entity {
    void Bug::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed.clear();
            m_is_on_ground = false;
            m_parent = nullptr;

            reset_anim("upended");            
        }
        //console::log("num jumps: ", (int)m_num_jumps, "\n");

        //console::log(class_name(), "::state_swim() ", velocity().x, "\n");

        //anim::speed(m_current_anim, 0.0F);

        acceleration({ });
        deceleration({ 0.0F, 0.1F });
        max_velocity({ 4.0F, 4.0F });

        cF32 diff_y = m_water_line_y - (position_on_level().y + 8.0F);
        velocity_add_y(diff_y / 30.0F);
        velocity_y(velocity().y * 0.99F);

        sprite_angle(degrees() + 270.0F);
    }
}