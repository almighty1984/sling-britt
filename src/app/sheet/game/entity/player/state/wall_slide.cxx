module entity.player;
import aabb;
import camera;
import console;
import health;
import particle_system;
import input;

namespace entity {
    void Player::state_wall_slide(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;


            if (sound_is_playing("hover")) {
                sound_stop("hover");
            }

            //m_saved_state = m_prev_state == state::Type::dive ? state::Type::run : m_prev_state;
            
            console::log(class_name(), "::state_wall_slide() saved state: ", state::to_string(m_saved_state), "\n");
            sprite_is_hidden(false);
            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);
            sprite::is_hidden(m_target_sprite, true);

            m_water_line_y = 0.0F;            
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            max_velocity(m_ground_max_velocity);

            sprite_texture("res/texture/entity/player/britt.png");
        }
        
        //console::log(class_name(), "::state_wall_slide()\n");

        if (is_pressed(key_left)) {
            //console::log(class_name(), "::state_wall_slide() pressing left\n");
            velocity_add_x(-acceleration().x);
        }
        if (is_pressed(key_right)) {
            //console::log(class_name(), "::state_wall_slide() pressing right\n");
            velocity_add_x(acceleration().x);
        }

        velocity_add_y(acceleration().y);

        if (velocity().y > 1.0F) {
            velocity_y(1.0F);
        }

        if (!sound_is_playing("wall_slide")) {
            sound_play("wall_slide");
        }

        m_num_jumps = 0;
        /*if (m_time_left_jump_again > 0) {
            --m_time_left_jump_again;
        } else {
            m_num_jumps = 0;
        }*/


        ++m_time_in_state;
        
        //console::log(class_name(), "::collide_x() sliding wall: ", (int)m_time_sliding_wall, "\n");
        if (m_time_in_state > 5) {
            m_time_in_state = 0;
            F32 wall_x{};
            if      (m_is_near_wall_L) wall_x = position().x;
            else if (m_is_near_wall_R) wall_x = position().x - 8.0F;

            particle::spawn({ .parent = this,
                              .type = particle::Type::dust_wall,
                              .position = Vec2F{ wall_x, position().y + 8.0F },
                              .velocity = Vec2F{ 0.0F, 0.0F },
                              .is_leftward = sprite_is_leftward()});
        }
        

        sprite_is_leftward(!m_is_near_wall_L);


        if ((m_is_near_wall_L and !is_pressed(key_left)) or (m_is_near_wall_R and !is_pressed(key_right)) or
            m_is_on_ground or m_is_on_slope or velocity().x < -acceleration().x * 2.0F or velocity().x > acceleration().x * 2.0F) {
            m_next_state = m_saved_state;
            console::log(class_name(), "::state_wall_slide() -> state_run()\n");
            sound_stop("wall_slide");            
        }
        else {
            set_anim("wall_slide");
            max_velocity(m_ground_max_velocity);
            m_time_left_rising = 0;
            if (sound_is_stopped("wall_slide")) {
                //console::log("sliding wall\n");
                sound_play("wall_slide");
            }
            m_is_on_ground = false; 
        }

        

        if (is_pressed(key_jump) and !is_locked(key_jump)) {
            //m_num_jumps = 0;
            m_next_state = state::Type::wall_jump;
            console::log(class_name(), "::state_wall_slide() -> state_wall_jump()\n");
            sound_stop("wall_slide");            
        }
    }
}