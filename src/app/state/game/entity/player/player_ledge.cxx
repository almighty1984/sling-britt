module entity.player;
import aabb;
import camera;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_ledge() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_ducking = false;
            m_time_left_rising = 0;
            m_time_left_ducking = 0;
            sound_stop("slide_wall");
            sound_stop("hover");
            sprite_is_hidden(false);
            m_water_line_y = 0.0F;
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            reset_anim("ledge_grab");
        }

        m_is_on_ground = false;
        m_num_jumps = 0;

        set_anim("ledge_grab");

        sprite::is_leftward(m_sprite_id, m_is_wall_to_left);

        deceleration_x(0.0F);
        acceleration_x(0.0F);
        velocity_x(0.0F);
        velocity_y(0.0F);

        if (is_pressed(key_up) && !is_locked(key_up)) {
            release(key_up);
            sound_position("ledge_climb", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("ledge_climb");

            m_is_climbing_ledge = true;
            velocity_y(-2.8F);
            reset_anim("ledge_climb");
            ++m_num_jumps;
            if (m_num_jumps > 2) {
                m_num_jumps = 2;
            }
            m_time_left_jump_again = m_time_to_jump_again;
            //m_time_left_ducking = m_time_to_duck;

            m_next_state = State::run;
        }
        if (is_pressed(key_down) && !is_locked(key_down)) {
            release(key_down);
            m_next_state = State::run;
            velocity_y(0.5F);
        }
        if (is_pressed(key_jump)) {
            m_time_left_jumping_wall = m_time_to_jump_wall;            
            m_next_state = State::run;
        }
    }
}