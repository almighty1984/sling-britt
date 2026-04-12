module entity.player;
import aabb;
import camera;
import console;
import health;
import particle_system;
import input;

namespace entity {
    void Player::state_slide_wall() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_saved_state = m_prev_state;
            console::log(class_name(), "::state_slide_wall() prev state: ", state::to_string(m_prev_state), "\n");
            sprite_is_hidden(false);
            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);

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
            max_velocity(m_ground_max_velocity);
        }        
        

        //console::log(class_name(), "::state_slide_wall()\n");

        if (is_pressed(key_left)) {
            //console::log(class_name(), "::state_slide_wall() pressing left\n");
            velocity_add_x(-acceleration().x);
        }
        if (is_pressed(key_right)) {
            //console::log(class_name(), "::state_slide_wall() pressing right\n");
            velocity_add_x(acceleration().x);
        }

        velocity_add_y(acceleration().y);

        if (velocity().y > 1.0F) {
            velocity_y(1.0F);
        }


        if (!sound_is_playing("slide_wall")) {
            sound_play("slide_wall");
        }


        ++m_time_in_state;
        
        //console::log(class_name(), "::collide_x() sliding wall: ", (int)m_time_sliding_wall, "\n");
        if (m_time_in_state > 5) {
            m_time_in_state = 0;
            cF32 wall_x = m_is_wall_to_left ? position().x : position().x - 8.0F;

            particle::spawn({ .parent = this,
                              .type = particle::Type::dust_wall,
                              .position = Vec2F{ wall_x, position().y + 8.0F },
                              .velocity = Vec2F{ 0.0F, 0.0F },
                              .is_leftward = sprite_is_leftward()});
        }
        




        if ((m_is_wall_to_left and !is_pressed(key_left)) or (!m_is_wall_to_left and !is_pressed(key_right)) or
            m_is_on_ground or m_is_on_slope or velocity().x < -acceleration().x * 2.0F or velocity().x > acceleration().x * 2.0F) {
            m_next_state = m_saved_state;
            console::log(class_name(), "::state_slide_wall() -> state_run()\n");
            sound_stop("slide_wall");            
        }
        else {
            set_anim("slide_wall");
            max_velocity(m_ground_max_velocity);
            m_time_left_ducking = 0;
            m_time_left_rising = 0;
            if (sound_is_stopped("slide_wall")) {
                //console::log("sliding wall\n");
                sound_play("slide_wall");
            }
            m_is_on_ground = false;            
        }

        if (is_pressed(key_jump) and !is_locked(key_jump)) {
            m_num_jumps = 0;
            m_next_state = state::Type::jump_wall;
            console::log(class_name(), "::state_slide_wall() -> state_jump_wall()\n");
            sound_stop("slide_wall");            
        }
    }
}