module entity.player;
import console;
import random;
import particle_system;

//U8 prev_num_jumps = 0;

F32 swim_speed = 0.0F, prev_swim_speed = 0.0F;

namespace entity {
    void Player::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //m_num_jumps = 0;
            
            sound_stop("hover");
            sound_stop("slide_wall");

            reset_anim("swim");

            max_velocity({ 6.0F, 6.0F });

            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);
        }
        
        //console::log(class_name(), "::state_swim() velocity: ", velocity().x, " ", velocity().y, "\n");;

        //if (m_num_jumps != prev_num_jumps) {
        //    m_time_in_state = 0;
        //    prev_num_jumps = m_num_jumps;
        //    //console::log("reset time in state\n");
        //}
        //console::log("swim ", (int)m_num_jumps, "\n");
        ++m_time_in_state;
        //console::log("m_time_in_state: ", m_time_in_state, "\n");

        if (m_time_in_state == 40) {
            // prevent getting stuck on playing
            if (sound_is_playing("water_enter")) sound_stop("water_enter");
            if (sound_is_playing("water_exit"))  sound_stop("water_exit");
        }


        moved_velocity({});

        //max_velocity({ 4.0F, 4.0F });

        //max_velocity(is_pressed(key_sprint) or m_num_jumps == 1 ? Vec2F{ 4.0F, 4.0F } : Vec2F{ 3.0F, 3.0F });


        if (velocity().x != 0.0F and velocity().y != 0.0F) {
            prev_swim_speed = swim_speed;
            swim_speed = std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
        }
        else if (velocity().x == 0.0F) {
            swim_speed = velocity().y;
        }
        else if (velocity().y == 0.0F) {
            swim_speed = velocity().x;
        }
        else {
            //swim_speed = prev_swim_speed;
        }
        


        //if (m_time_in_state > 5) {
            //max_velocity(is_pressed(key_sprint) or m_num_jumps == 1 ? Vec2F{ 3.0F, 3.0F } : Vec2F{ 2.0F, 2.0F });
        //}

        //console::log(class_name(), "::swim() position on level: ", position_on_level().y, "\n");        
        
                
        deceleration({ 0.02F, 0.02F });

        cF32 swim_speed_limit = is_pressed(key_sprint) ? 4.0F : 2.5F;
        //cF32 swim_speed_limit = 3.0F;
                
        //console::log(class_name(), "::state_swim() speed: ", swim_speed, "\n");

        Vec2F swim_acc = { 0.2F, 0.2F };

        // steer slower at high speed
        if (std::abs(swim_speed) > swim_speed_limit) {
            swim_acc *= 0.5F;
            
                //cF32 water_inertia = is_pressed(key_sprint) ? 0.95F : 0.9F;
                cF32 water_inertia = 0.95F;

            //    //if (std::abs(velocity().x) > 4.0F) {
                velocity_x(velocity().x * water_inertia);
            //    //swim_acc.x = 0.0F;
            ////}
            ////if (std::abs(velocity().y) > 4.0F) {
                velocity_y(velocity().y * water_inertia);
            //    //swim_acc.y = 0.0F;
            ////}
            
        }

        acceleration(swim_acc);
        
        if (!is_pressed(input::Key::ctrl)) {
            if (is_pressed(key_up) and m_time_in_state > 4) {
                velocity_add_y(-acceleration().y);
            }
            if (is_pressed(key_down)) {
                velocity_add_y(acceleration().y);
            }
            if (is_pressed(key_left)) {
                //sprite::is_leftward(m_sprite, true);        
                velocity_add_x(-acceleration().x);
            }
            if (is_pressed(key_right)) {
                //sprite::is_leftward(m_sprite, false);
                velocity_add_x(acceleration().x);
            }
        }

        //console::log(class_name(), "::swim is_on_ground: ", m_is_on_ground, "\n");
        
        for (auto& i : m_aabbs) {
            aabb::is_active(i, false);
            if (aabb::name(i) == aabb::Name::body_swim) {
                aabb::is_active(i, true);
            }
        }
        cF32 hypotenuse = std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
        anim::speed(m_current_anim, hypotenuse / 5.0F);
        if (anim::speed(m_current_anim) < 0.1F) {
            anim::speed(m_current_anim, 0.1F);
        }
        cU16 largest_vel = std::abs(velocity().x) > std::abs(velocity().y) ? std::abs(velocity().x) : std::abs(velocity().y);
        //console::log(class_name(), "::swim largest_vel: ", largest_vel, "\n");

        if (m_time_in_state > 200 - largest_vel * 10) {
            //cU32 speed = (I32)std::sqrtf(velocity().x * velocity().x + velocity().y * velocity().y);
            //console::log("speed: ", speed, "\n");
            m_time_in_state = random::number(0, 10);
            cF32 vel_x = sprite::is_upended(m_sprite) ? -0.5F : 0.5F;

            particle::spawn({ .parent   = this,
                                .type     = particle::Type::bubble,
                                .position = position() + Vec2F{sprite::is_upended(m_sprite) ? -6.0F : 6.0F, 0.0F},
                                .velocity = Vec2F{velocity().x / 10.0F + vel_x, -0.5F},
                                .state    = state::Type::idle
                }
            );
        }


        

        m_radians = std::atan2(velocity().y, velocity().x);
        if (m_radians < 0.0F) {
            m_radians += 3.1415926535F * 2.0F;
        }

        if (velocity().x == 0.0F) {
            if (sprite_is_upended() and m_radians == 0.0F) {
                m_radians = 3.1415926535F;
            }
        }

        F32 degrees = m_radians * 180.0F / 3.1415926535F;
        //console::log("degrees: ", degrees, "\n");
        sprite_origin({ sprite_rect().w / 2.0F, 48.0F });

        if (is_pressed(key_jump)) {
            return;
        }

        if (std::abs(velocity().x) > acceleration().x or
            std::abs(velocity().y) > acceleration().y or
            is_pressed(key_left) or is_pressed(key_right) or
            is_pressed(key_up) or is_pressed(key_down)
            ) {

            //if (degree_diff > 10.0F) {
                //sprite::angle_add(m_sprite, degree_diff * 0.1F);
            //} else {
            sprite_angle(degrees);
            //}
        }

        sprite_is_leftward(false);
        if (velocity().x < 0.0F) {
            sprite_is_upended(true);
        }
        else if (velocity().x > 0.0F) {
            sprite_is_upended(false);
        }

        // rotate back when idle
        if (!is_pressed(key_up) and !is_pressed(key_down) and !is_pressed(key_left) and !is_pressed(key_right)) {
            if (sprite_is_upended()) {
                cF32 from_target_dir = std::abs(180.0F - sprite_angle());
                //console::log("from_target_dir: ", from_target_dir, "\n");

                if (sprite_angle() >= 90.0F and sprite_angle() < 180.0F) {
                    sprite_angle(sprite_angle() + 1.0F * from_target_dir * 0.1F);
                } else if (sprite_angle() <= 270.0F and sprite_angle() > 180.0F) {
                    sprite_angle(sprite_angle() - 1.0F * from_target_dir * 0.1F);
                }
            } else {
                cF32 target_dir = sprite_angle() >= 270.0F and sprite_angle() <= 360.0F ? 360.0F : 0.0F;

                cF32 from_target_dir = std::abs(target_dir - sprite_angle());

                //console::log("from_target_dir: ", from_target_dir, "\n");

                if (sprite_angle() >= 270.0F and sprite_angle() < 360.0F) {
                    sprite_angle(sprite_angle() + 1.0F * from_target_dir * 0.1F);
                } else if (sprite_angle() <= 90.0F and sprite_angle() > 0.0F) {
                    sprite_angle(sprite_angle() - 1.0F * from_target_dir * 0.1F);
                }
            }
        }
        if (position_on_level().y < m_water_line_y) {
            m_next_state = state::Type::dive;
        }
        /*if (m_next_state == state::Type::run) {
            if (m_time_left_in_state > 0) --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_is_first_state_update = true;
                unlock(key_melee);
                m_is_ducking = false;
                m_time_left_melee = 0;
                m_state = state::Type::run;
                sprite_is_leftward(sprite_is_upended());
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }
        }*/

        //console::log(class_name(), "::state_swim() velocity: ", velocity().x, "\n");
    }
}