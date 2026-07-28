module entity.bee;

namespace entity {
    void Bee::state_attack(cF32 dt) {
        Vec2F vel = m_sensed_offset - position();
        sound_position("attack", { position().x - app::config::extent().x / 2.0F,
                                   position().y - app::config::extent().y / 2.0F });
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            console::log(class_name(), "::state_attack()\n");
            m_time_in_state = 0;
            reset_anim("attack");
            console::log("vel vec: ", vel.x, " ", vel.y, "\n");
            max_velocity({ 10.0F, 10.0F });

            vel.normalize();
            velocity(vel * 5.0F);

            sound_play("attack");
            sound_pitch("attack", 1.0F + random::number(-2, 2) / 100.0F);
        }
        ++m_time_in_state;

        //sprite_is_leftward(velocity().x < 0.0F);
        sprite_is_leftward(false);
        sprite_is_upended(velocity().x < 0.0F);

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += PI * 2.0F;
        cF32 degrees = radians * 180.0F / PI;
        sprite_angle(degrees + 0.0F);


        if (m_time_in_state > 20) {
            m_time_in_state = 0;
            m_next_state = state::Type::idle;
        }
    }
}