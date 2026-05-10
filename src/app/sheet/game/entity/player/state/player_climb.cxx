module entity.player;
import app.config;
import aabb;
import camera;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_climb(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            sprite_is_hidden(false);
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }
            sound_position("climb", { position().x - app::config::extent().x / 2.0F,
                                      position().y - app::config::extent().y / 2.0F });
            sound_play("climb");

            velocity_y(-2.8);
            reset_anim("climb");
            ++m_num_jumps;
            if (m_num_jumps > 2) {
                m_num_jumps = 2;
            }

            deceleration(start_deceleration());
            acceleration(start_acceleration());
        }
        if (is_pressed(key_left)) {            
            velocity_add_x(-acceleration().x);
        }
        if (is_pressed(key_right)) {            
            velocity_add_x(acceleration().x);
        }

        velocity_add_y(acceleration().y);

        if (velocity().y > acceleration().y) {
            m_next_state = m_saved_state;
        }
    }
}