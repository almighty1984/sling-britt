module entity.player;
import app.config;
import camera;
import sprite;
import particle_system;

constexpr F32 PI = 3.1415926535F;
constexpr F32 RADIAN_UP_LEFTWARD = PI * 1.5F;
constexpr F32 RADIAN_UP_RIGHTWARD  = -PI * 0.5F;
constexpr F32 RADIAN_DOWN = PI * 0.5F;
constexpr F32 RADIAN_LEFT = PI;
constexpr F32 RADIAN_RIGHT = 0.0F;

namespace entity {
    void Player::state_shoot() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            camera::is_idle_recentered = false;
            camera::is_offset_with_direction = true;

            sprite::is_hidden(m_sling_shot_sprite, false);
            sprite::is_hidden(m_sling_shot_bg_sprite, false);

            if (sprite_is_leftward()) {
                m_radians = PI;
            }
            else {
                m_radians = 0.0F;
            }
            m_rotation_speed = 0.0F;
            m_rotation_speed_limit = 0.1F;
            //m_rotation_acc = 0.005F;
            m_rotation_acc = PI / 8.0F;
            m_rotation_dec = 0.01F;
        }

        if (is_pressed(key_jump)) {
            if (m_is_on_ground and !is_locked(key_jump) and m_time_left_ducking == 0) {
                m_num_jumps = 0;
                jump();
            } else {
                if (m_time_left_holding_jump > 0 and velocity().y < 0.0F) {
                    --m_time_left_holding_jump;
                    velocity_add({ 0.0F,-0.3F });
                } else {
                    m_time_left_holding_jump = 0;
                }
            }
        }
        else {
            unlock(key_jump);
            m_is_hovering = false;
        }

        if (velocity().y > acceleration().y + (m_is_on_slope ? max_velocity().y : 1.0F)) {
            //if (velocity().y >= 1.0F and !m_is_on_slope) {
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_time_left_ducking = 0;
        }

        if (!m_is_on_ground) {
            if (velocity().y > acceleration().y) {
                if (is_pressed(key_jump) and !is_locked(key_jump)) {
                    lock(key_jump);
                    if (moved_velocity().y != 0.0F) {
                        velocity_add({ 0.0F, moved_velocity().y });
                        moved_velocity_y(0.0F);
                    }
                    m_is_hovering = true;
                    m_time_left_ducking = 0;
                    m_time_left_rising = 0;
                } else if (m_current_anim != anim("jump_spin")) {
                    reset_anim("fall");
                    anim::speed(anim("fall"), velocity().y / 10.0F);
                }
            }
        }
        
        if (m_is_hovering) {
            cF32 velocity_limit_y = 1.0F;

            if (velocity().y >= velocity_limit_y) {
                velocity_y(velocity_limit_y);
                set_anim("hover");
            }
            if (!sound_is_playing("hover")) {
                sound_position("hover", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_is_looped("hover", true);
                sound_play("hover");
            }
        } else {
            sound_stop("hover");
        }


        acceleration(start_acceleration());
        deceleration(start_deceleration());

        if (m_is_on_ground) {
            m_is_hovering = false;

            if (m_time_left_bouncing > 0) {
                //console::log(class_name(), "::state_shoot() time left bouncing: ", (int)m_time_left_bouncing, "\n");
                --m_time_left_bouncing;
                if (m_time_left_bouncing == 0 and m_is_on_ground) {
                    max_velocity(m_ground_max_velocity);
                }
            }
            if (velocity().x < -deceleration().x or velocity().x > deceleration().x) {
                //sprite_is_leftward(velocity().x < 0.0F);
                if (m_time_left_ducking > 0) {
                    set_anim("crawl");
                }
                else {
                    set_anim("walk");
                }
            } else {
                if (m_time_left_ducking > 0) {
                    set_anim("duck");
                }
                else {
                    set_anim("shoot");
                }
            }
        } else {
            deceleration({});
            acceleration_x(start_acceleration().x * 0.5F);
        }

        velocity_add_y(acceleration().y);

        if (is_pressed(key_left) and !is_locked(key_left)) {
            velocity_add_x(-acceleration().x);

            if (!is_pressed(key_sprint)) {
                sprite_is_leftward(true);
            }
        }
        if (is_pressed(key_right) and !is_locked(key_right)) {
            velocity_add_x(acceleration().x);
            if (!is_pressed(key_sprint)) {
                sprite_is_leftward(false);
            }
        }


        F32 offset_from_left{}, offset_from_right{};

        F32 radian_amount = PI / 8.0F;

        direction_x(sprite_is_leftward() ? -1.0F : 1.0F);

        if (is_pressed(key_up)) {
            if (!is_locked(key_up)) {
                lock(key_up);
                if (sprite_is_leftward()) {
                    console::log("radians: ", m_radians, " ", RADIAN_UP_LEFTWARD, "\n");
                    if (m_radians < RADIAN_UP_LEFTWARD) {
                        m_radians += radian_amount;
                    }
                } else {
                    m_radians -= radian_amount;
                }
            }
        }
        else {
            unlock(key_up);
        }
        if (is_pressed(key_down)) {
            if (!is_locked(key_down)) {
                lock(key_down);
                if (sprite_is_leftward()) {
                    m_radians -= radian_amount;
                }
                else {
                    m_radians += radian_amount;
                }
            }
        }
        else {
            unlock(key_down);
        }

        if (sprite_is_leftward()) {

            if (m_radians > RADIAN_UP_RIGHTWARD and m_radians < RADIAN_DOWN) {
                console::log("facing wrong, offset from up: ", m_radians - RADIAN_UP_RIGHTWARD, "\n");
                m_radians = RADIAN_UP_LEFTWARD - (m_radians - RADIAN_UP_RIGHTWARD);
            }
            /*offset_from_right = m_radians - RADIAN_RIGHT;
            if (m_radians > RADIAN_UP_LEFTWARD and m_radians < RADIAN_DOWN) {
                console::log("turned around\n");
                m_radians = RADIAN_RIGHT - offset_from_right;
            }*/
        }
        else {
            if (m_radians < RADIAN_UP_LEFTWARD and m_radians > RADIAN_DOWN) {
                console::log("facing wrong, offset from up: ", m_radians - RADIAN_UP_LEFTWARD, "\n");
                m_radians = RADIAN_UP_RIGHTWARD - (m_radians - RADIAN_UP_LEFTWARD);
            }

            /*offset_from_left = m_radians - RADIAN_RIGHT;

            if (m_radians < RADIAN_UP_RIGHTWARD and m_radians > RADIAN_DOWN) {
                console::log("turned around\n");
                m_radians = RADIAN_LEFT - offset_from_left;
                
            }*/
        }
        
        //if (sprite_is_leftward()) {
        //    direction_x(-1.0F);

        //    offset_from_left = m_radians - RADIAN_LEFT;
        //    if (is_pressed(key_up) and !is_locked(key_up)) {
        //        release(key_up);
        //        lock(key_up);
        //        m_rotation_speed += m_rotation_acc;
        //    }
        //    else if (is_pressed(key_down) and !is_locked(key_down)) {
        //        lock(key_down);
        //        m_rotation_speed -= m_rotation_acc;
        //    }
        //    else {
        //        if (m_rotation_speed <= -m_rotation_dec) {
        //            m_rotation_speed += m_rotation_dec;
        //        }
        //        else if (m_rotation_speed >= m_rotation_dec) {
        //            m_rotation_speed -= m_rotation_dec;
        //        }
        //        else {
        //            m_rotation_speed = 0.0F;
        //        }
        //    }            
        //    
        //    if (m_radians > RADIAN_UP_LEFTWARD and m_radians < RADIAN_DOWN) {
        //        console::log(class_name(), "::state__shoot() changed direction to left, mirror radians\n");
        //        m_radians = RADIAN_RIGHT - offset_from_left;
        //    }
        //    else {
        //        m_radians += m_rotation_speed;
        //        if (m_radians > RADIAN_UP_RIGHTWARD - 0.05F) {
        //            m_radians = RADIAN_UP_RIGHTWARD - 0.05F;
        //            m_rotation_speed = 0.0F;
        //        }
        //        if (m_radians < RADIAN_DOWN + 0.05F) {
        //            m_radians = RADIAN_DOWN + 0.05F;
        //            m_rotation_speed = 0.0F;
        //        }

        //        console::log(class_name(), "::state_shoot() radians: ", m_radians, "\n");
        //    }            
        //}
        //else if (!sprite_is_leftward()) {
        //    direction_x(1.0F);

        //    offset_from_right = m_radians - RADIAN_RIGHT;
        //    if (is_pressed(key_up) and !is_locked(key_up)) {
        //        lock(key_up);
        //        m_rotation_speed -= m_rotation_acc;
        //    }
        //    else if (is_pressed(key_down) and !is_locked(key_down)) {
        //        lock(key_down);
        //        m_rotation_speed += m_rotation_acc;
        //    }
        //    else {
        //        if (m_rotation_speed <= -m_rotation_dec) {
        //            m_rotation_speed += m_rotation_dec;
        //        }
        //        else if (m_rotation_speed >= m_rotation_dec) {
        //            m_rotation_speed -= m_rotation_dec;
        //        }
        //        else {
        //            m_rotation_speed = 0.0F;
        //        }
        //    }
        //    if (m_rotation_speed < 0.0F and m_rotation_speed < -m_rotation_speed_limit) {
        //        m_rotation_speed = -m_rotation_speed_limit;
        //    }
        //    if (m_rotation_speed > 0.0F and m_rotation_speed > m_rotation_speed_limit) {
        //        m_rotation_speed = m_rotation_speed_limit;
        //    }
        //    if (m_radians < RADIAN_UP_RIGHTWARD and m_radians > RADIAN_DOWN) {
        //        console::log(class_name(), "::state__shoot() changed direction to right, mirror radians\n");
        //        m_radians = RADIAN_LEFT - offset_from_right;
        //    }
        //    else {
        //        m_radians += m_rotation_speed;

        //        if (m_radians < RADIAN_UP_LEFTWARD) {
        //            m_radians = RADIAN_UP_LEFTWARD;
        //            m_rotation_speed = 0.0F;
        //        }
        //        if (m_radians > RADIAN_DOWN) {
        //            m_radians = RADIAN_DOWN;
        //            m_rotation_speed = 0.0F;
        //            //console::log(class_name(), "::state_shoot() radians: ", m_radians, "\n");
        //        }                
        //    }
        //}

        
        F32 degrees = m_radians * 180.0F / 3.1415926535F;
        if (degrees > 360.0F) {
            degrees -= 360.0F;
        }
        sprite::angle(m_sling_shot_sprite, degrees);
        sprite::angle(m_sling_shot_bg_sprite, sprite::angle(m_sling_shot_sprite));
        //console::log(class_name(), "::state_shoot() degrees: ", degrees, "\n");
        
        //if (focus.x != 0.0F) {
            //sprite_is_leftward(degrees <= 270.0F and degrees > 90.0F);
            sprite::is_upended(m_sling_shot_sprite, degrees <= 270.0F and degrees > 90.0F);
            sprite::is_upended(m_sling_shot_bg_sprite, sprite::is_upended(m_sling_shot_sprite));            
        //}
        //else {
        //    console::log(class_name(), "::state_shoot() focus.x == 0.0\n");
            //sprite::is_leftward(m_sling_shot_sprite, sprite_is_leftward());
            //sprite::is_leftward(m_sling_shot_bg_sprite, sprite_is_leftward());
            //sprite::is_upended(m_sling_shot_sprite, sprite_is_leftward());
            //sprite::is_upended(m_sling_shot_bg_sprite, sprite_is_leftward());
        //}

        
        


        
        if (m_time_left_melee > 0) {            
            cI32 frame = (m_time_to_melee - m_time_left_melee) * 32;
            sprite::rect_x(m_sling_shot_sprite, frame);
            --m_time_left_melee;
        }
        else {
            sprite::rect_x(m_sling_shot_sprite, 0);
        }


        if (is_pressed(key_melee)) {         
            if (!is_locked(key_melee) and m_time_left_melee == 0) {                
                lock(key_melee);
                m_time_left_melee = m_time_to_melee;

                cVec2F start_position = { position().x,
                                          position().y };

                sound_stop("shoot");
                sound_position("shoot", { start_position.x / (app::config::extent().x / 2.0F),
                                          start_position.y / (app::config::extent().y / 2.0F) });
                sound_play("shoot");

                Vec2F shot_velocity = { std::cosf(m_radians) * 8.0F, std::sinf(m_radians) * 8.0F };

                //Vec2F shot_velocity = { std::cosf(degrees / 180.0F * 3.1415926535F), std::sinf(degrees / 180.0F * 3.1415926535F) };


                particle::spawn({ .parent   = this,
                                  .type     = particle::Type::shot,
                                  .position = start_position,
                                  .velocity = shot_velocity,
                                  .state    = state::Type::idle });
            }
        }
        else {
            unlock(key_melee);
        }



        if (is_pressed(input::Key::shift)) {
            release(input::Key::shift);
            //lock(input::Key::shift);
            m_next_state = state::Type::run;
            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);

            camera::is_idle_recentered = true;
            camera::is_offset_with_direction = false;
            direction_x(0.0F);
        }
    }
}