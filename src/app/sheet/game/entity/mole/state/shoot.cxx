module entity.mole;
import camera;

namespace entity {
    void Mole::state_shoot(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_until_next_state = 50;
            m_next_state = state::Type::enter;
            m_sensed.clear();

            sprite_is_leftward(m_sensed_offset.x < position_on_level().x + 4.0F);

            reset_anim("shoot");

            sound_position("shoot", { (position().x + sprite_rect().w / 2.0F) / (app::config::extent().x / 2.0F),
                                      (position().y + sprite_rect().w / 2.0F) / (app::config::extent().y / 2.0F) });
            sound_play("shoot");

            cVec2F start_position = position_on_level() + Vec2F{ 4.0F, 0.0F };
            cVec2F end_position = m_sensed_offset;
            cVec2F v = (end_position - start_position);

            Vec2F shot_velocity = v / line::length(v) * 8.0F;
            //if (shot_velocity.y > -2.0F) {
                //shot_velocity.y = -2.0F;
            //}
            if (shot_velocity.x < 0.0F and shot_velocity.x > -2.0F) {
                shot_velocity.x = -2.0F;
            } else if (shot_velocity.x > 0.0F and shot_velocity.x < 2.0F) {
                shot_velocity.x = 2.0F;
            }
            particle::spawn({ .parent = this,
                              .type = particle::Type::rock,
                              .position = start_position - camera::position,
                              .velocity = shot_velocity,
                              .state = state::Type::idle });


            sprite_offset_y(-4.0F);
            health_is_hidden(false);

            console::log(class_name(), "::state_shoot() sensed offset: ", m_sensed_offset.x, " ", m_sensed_offset.y, "\n");
        }
        velocity_x(0.0F);


        //console::log(class_name(), "::state_shoot()\n");

        //console::log(class_name(), "::shoot() time left in state: ", m_time_left_until_next_state, "\n");

    }
}