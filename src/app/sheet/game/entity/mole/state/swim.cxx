module entity.mole;

namespace entity {
    void Mole::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed.clear();
            m_is_on_ground = false;
            reset_anim("jump");

            if (velocity().x < 0.0F) {
                m_added_degrees = 180.0F;
            } else {
                m_added_degrees = 0.0F;
            }

            sprite_offset({ -8.0F, -8.0F });

            health_is_hidden(true);
        }

        acceleration({ });
        deceleration({ 0.0F, 0.1F });
        max_velocity({ 4.0F, 4.0F });

        cF32 diff_y = m_water_line_y - (position_on_level().y + 8.0F);

        velocity_add_y(diff_y / 30.0F);
        velocity_y(velocity().y * 0.99F);

        //console::log(class_name(), "::state_swim() degrees: ", degrees(), "\n");

        sprite_angle(degrees() + m_added_degrees);

        //sprite_angle_add(1.0F);

        sprite_is_leftward(false);
        sprite_is_upended(false);

        //console::log(class_name(), "::state_swim()\n");
    }
}