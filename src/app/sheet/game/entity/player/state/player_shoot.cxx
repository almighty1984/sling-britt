module entity.player;
import app.config;
import camera;
import sprite;
import particle_system;

constexpr F32 RADIAN_UP_LEFTWARD = PI * 1.5F;
constexpr F32 RADIAN_UP_RIGHTWARD  = -PI * 0.5F;
constexpr F32 RADIAN_DOWN = PI * 0.5F;
constexpr F32 RADIAN_LEFT = PI;
constexpr F32 RADIAN_RIGHT = 0.0F;

namespace entity {
    void Player::state_shoot(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            sprite_texture("res/texture/entity/player_britt_carry.png");

            m_saved_state = m_state;

            camera::is_idle_recentered = false;
            camera::is_offset_with_direction = true;

            m_rotation_speed = 0.0F;
            m_rotation_speed_limit = 0.1F;
            //m_rotation_acc = 0.005F;
            m_rotation_acc = PI / 8.0F;
            m_rotation_dec = 0.01F;

            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }

            //m_radians = sprite_is_leftward() ? PI : 0.0F;

            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_left_skidding = 0;


            sprite::is_hidden(m_sling_shot_sprite, false);
            sprite::is_hidden(m_sling_shot_bg_sprite, false);
            sprite::is_hidden(m_target_sprite, false);
        }

        if (is_pressed(input::Key::t)) {
            release(input::Key::t);
            console::log("texture path: ", sprite_texture_path(), "\n");
        }

        sprite::rect(m_target_sprite, anim::source(m_target_anim));


        if (is_pressed(key_jump)) {
            if (m_is_on_ground and !is_locked(key_jump)) {
                if (m_num_jumps > 1) {
                    //m_next_state = state::Type::run;
                    //m_saved_state = state::Type::shoot;
                    sprite::is_hidden(m_sling_shot_sprite, true);
                    sprite::is_hidden(m_sling_shot_bg_sprite, true);
                    sprite::is_hidden(m_target_sprite, true);
                }
                jump();
            } else {
                if (m_time_left_holding_jump > 0 and velocity().y < 0.0F) {
                    --m_time_left_holding_jump;
                    velocity_add({ 0.0F,-0.3F });
                } else {
                    m_time_left_holding_jump = 0;
                }
            }
        } else {
            unlock(key_jump);
            m_is_hovering = false;
        }



        if (velocity().y > acceleration().y + (m_is_on_slope ? max_velocity().y : 1.0F)) {
            //if (velocity().y >= 1.0F and !m_is_on_slope) {
            m_is_on_ground = false;
            m_is_on_slope = false;
        }



        


        acceleration(start_acceleration());
        deceleration(start_deceleration());


        velocity_add_y(acceleration().y);

        //max_velocity(m_is_on_ground ? m_ground_max_velocity : m_sling_max_velocity);


        if (m_is_on_ground) {
            if (m_time_left_jump_again > 0) {
                --m_time_left_jump_again;
            } else {
                m_num_jumps = 0;
            }

            m_is_hovering = false;

            is_pressed(key_sprint) ? set_anim("run") : set_anim("walk");

            if (m_time_left_bouncing > 0) {
                //console::log(class_name(), "::state_shoot() time left bouncing: ", (int)m_time_left_bouncing, "\n");
                --m_time_left_bouncing;
                if (m_time_left_bouncing == 0 and m_is_on_ground) {
                    max_velocity(m_ground_max_velocity);
                }
            }

            if (velocity().x < -deceleration().x or velocity().x > deceleration().x) {
                //sprite_is_leftward(velocity().x < 0.0F);
                set_anim("walk");
            } else {
                set_anim("idle");
            }


            //m_velocity_at_jump = 0.0F;

            if (m_time_left_jump_again > 0) {
                m_time_left_jump_again -= 1;
            } else {
                m_num_jumps = 0;
            }

            if (is_pressed(key_sprint)) {
                max_velocity_x(m_ground_max_velocity.x * 1.2F);
            } else {
                max_velocity_x(m_ground_max_velocity.x);
            }
        } else if (!m_is_on_ground) {
            if (velocity().y > acceleration().y) {
                if (is_pressed(key_jump) and !is_locked(key_jump)) {
                    lock(key_jump);
                    if (moved_velocity().y != 0.0F) {
                        velocity_add({ 0.0F, moved_velocity().y });
                        moved_velocity_y(0.0F);
                    }
                    m_is_hovering = true;
                    m_time_left_rising = 0;
                } else if (m_current_anim != anim("jump_spin")) {
                    reset_anim("fall");
                    anim::speed(anim("fall"), velocity().y / 10.0F);
                }
            }
            deceleration({});
            acceleration_x(start_acceleration().x * 0.5F);
        }
    

        if (is_pressed(key_left) and !is_locked(key_left)) {
            if (velocity().x > -max_velocity().x * (is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                velocity_add_x(-acceleration().x);
            }
            if (!is_locked(key_shoot)) {
                sprite_is_leftward(true);
            }            
        }
        if (is_pressed(key_right) and !is_locked(key_right)) {
            if (velocity().x < max_velocity().x * (is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                velocity_add_x(acceleration().x);
            }
            if (!is_locked(key_shoot)) {
                sprite_is_leftward(false);
            }            
        }

        if (m_is_hovering) {
            cF32 velocity_limit_y = 1.0F;

            if (velocity().y >= velocity_limit_y) {
                velocity_y(velocity_limit_y);
                set_anim("hover");
            }
            if (!sound_is_playing("hover")) {
                sound_position("hover", { position().x - app::config::extent().x / 2.0F,
                                          position().y - app::config::extent().y / 2.0F });
                sound_is_looped("hover", true);
                sound_play("hover");
            }
        } else {
            sound_stop("hover");
        }


        F32 offset_from_left{}, offset_from_right{};

        F32 radian_amount = PI / 8.0F;

        direction_x(sprite_is_leftward() ? -1.0F : 1.0F);

        m_rotation_acc = 0.01F;
        m_rotation_dec = 0.005F;
        m_rotation_speed_limit = is_pressed(key_sprint) ? 0.02F : 0.1F;

        if (is_pressed(key_up)) {
            if (!is_locked(key_up)) {
                //lock(key_up);
                if (sprite_is_leftward()) {
                    //console::log("radians: ", m_radians, " ", RADIAN_UP_LEFTWARD, "\n");
                    if (m_radians < RADIAN_UP_LEFTWARD) {
                        m_rotation_speed += m_rotation_acc;
                    }                    
                } else {
                    if (m_radians > RADIAN_UP_RIGHTWARD) {                        
                        m_rotation_speed -= m_rotation_acc;
                    }
                    
                }
            }
        }
        //else {
            //unlock(key_up);
        //}
        if (is_pressed(key_down)) {
            if (!is_locked(key_down)) {
                //lock(key_down);
                if (sprite_is_leftward()) {
                    if (m_radians > RADIAN_DOWN) {
                        m_rotation_speed -= m_rotation_acc;
                    }
                }
                else {
                    if (m_radians < RADIAN_DOWN) {
                        m_rotation_speed += m_rotation_acc;
                    }
                }
            }
        }
        //else {
            //unlock(key_down);
        //}

        /*if (m_rotation_speed > 0.0F) {
            m_rotation_speed -= m_rotation_dec;
        }*/



        if (m_rotation_speed < 0.0F) {
            if (m_rotation_speed < -m_rotation_speed_limit) {
                m_rotation_speed = -m_rotation_speed_limit;
            }
            else {
                m_rotation_speed += m_rotation_dec;
            }
        }
        else if (m_rotation_speed > 0.0F) {
            if (m_rotation_speed > m_rotation_speed_limit) {
                m_rotation_speed = m_rotation_speed_limit;
            } else {
                m_rotation_speed -= m_rotation_dec;
            }
        }

        
        if (!sprite_is_leftward() and m_radians < RADIAN_UP_RIGHTWARD) {
            m_radians = RADIAN_UP_RIGHTWARD;
            m_rotation_speed = 0.0F;
        }
        if (sprite_is_leftward() and m_radians > RADIAN_UP_LEFTWARD) {
            m_radians = RADIAN_UP_LEFTWARD;
            m_rotation_speed = 0.0F;
        }
        if (m_rotation_speed > -m_rotation_dec and m_rotation_speed < m_rotation_dec) {
            m_rotation_speed = 0.0F;
        }

        //console::log(class_name(), "::state_shoot() rotation speed: ", m_rotation_speed, "\n");

        m_radians += m_rotation_speed;

        


        // turn around
        if (sprite_is_leftward()) {            
            if (m_radians >= RADIAN_UP_RIGHTWARD and m_radians <= RADIAN_DOWN) {
                console::log(class_name(), "::state_shoot() facing wrong, offset from up: ", m_radians - RADIAN_UP_RIGHTWARD, "\n");
                m_radians = RADIAN_UP_LEFTWARD - (m_radians - RADIAN_UP_RIGHTWARD);

                m_rotation_speed *= -1.0F;
            }            
        }
        else {
            if (m_radians <= RADIAN_UP_LEFTWARD and m_radians >= RADIAN_DOWN) {
                console::log(class_name(), "::state_shoot() facing wrong, offset from up: ", m_radians - RADIAN_UP_LEFTWARD, "\n");
                m_radians = RADIAN_UP_RIGHTWARD - (m_radians - RADIAN_UP_LEFTWARD);

                m_rotation_speed *= -1.0F;
            }
        }
        

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
            /*if (m_time_left_melee == 0) {
                unlock(key_melee);
            }*/
        }
        else {
            sprite::rect_x(m_sling_shot_sprite, 0);
        }

        F32 shot_radians = m_radians + (sprite_is_leftward() ? 0.02F : -0.02F);

        Vec2F shot_normal = { std::cosf(shot_radians), std::sinf(shot_radians) };

        sprite::offset(m_target_sprite, shot_normal * 64.0F);


        if (m_is_on_ground or m_is_hovering) {
            sprite::is_hidden(m_sling_shot_sprite, false);
            sprite::is_hidden(m_sling_shot_bg_sprite, false);
            sprite::is_hidden(m_target_sprite, false);
        }

        if (is_pressed(key_melee) and !sprite::is_hidden(m_sling_shot_sprite)) {            
            if (!is_locked(key_melee) and m_time_left_melee == 0) {
                lock(key_melee);
                m_time_left_melee = m_time_to_melee;
                cF32 speed = 7.0F;

                Vec2F shot_velocity = velocity() * 0.25F + moved_velocity() * 0.25F + shot_normal * speed;

                cVec2F start_position = { position() + Vec2F{4.0F, 4.0F} + shot_velocity};

                sound_stop("shoot");
                sound_position("shoot", { start_position.x / (app::config::extent().x / 2.0F),
                                          start_position.y / (app::config::extent().y / 2.0F) });
                sound_play("shoot");

                particle::spawn({ .parent = this,
                                  .type = particle::Type::rock,
                                  .position = start_position,
                                  .velocity = shot_velocity,
                                  .state = state::Type::idle });

                //cF32 start_angle = m_radians * (180.0F / PI) - 45.0F;
                //cF32 end_angle = start_angle + 90.0F;
                //particle::spawn_fan(this, start_angle, end_angle, 6, particle::Type::rock, start_position, shot_velocity, 2.0F);
            }
        }
        else {
            unlock(key_melee);
        }

        if (m_current_anim == anim("run") or m_current_anim == anim("walk")) {
            if (velocity().x < 0.0F) {
                anim::speed(m_current_anim, velocity().x / -5.0F);
            } else {
                anim::speed(m_current_anim, velocity().x / 5.0F);
            }
            if (anim::speed(m_current_anim) < 0.2F) {
                anim::speed(m_current_anim, 0.2F);
            }
        }


        if (is_pressed(key_shoot)) {
            if (!is_locked(key_shoot)) {
                release(key_shoot);
                //lock(key_shoot);
                m_next_state = state::Type::run;
                sprite::is_hidden(m_sling_shot_sprite, true);
                sprite::is_hidden(m_sling_shot_bg_sprite, true);
                sprite::is_hidden(m_target_sprite, true);

                camera::is_idle_recentered = true;
                camera::is_offset_with_direction = false;
                direction_x(0.0F);
            }
        } else {
            unlock(key_shoot);
        }
    }
}