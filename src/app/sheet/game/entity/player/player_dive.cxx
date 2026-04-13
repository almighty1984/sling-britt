module entity.player;
import console;
import random;
import particle_system;

namespace entity {
    void Player::state_dive() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_carrying = false;
            m_is_down_thrusting = false;
            sound_stop("hover");
            sound_stop("slide_wall");

            max_velocity({ 6.0F, 6.0F });
            m_saved_state = state::Type::run;
        }
        //console::log(class_name(), "::state_dive() velocity: ", velocity().x, " ", velocity().y, "\n");;

        ++m_time_in_state;

        console::log(class_name(), "::state_dive() time in state: ", m_time_in_state, "\n");

        // prevent bumping head too fast and falling out of water
        if (m_prev_state == state::Type::swim and m_time_in_state < 3 and velocity().y > 2.0F) {
            m_next_state = state::Type::swim;
        }

        if (m_time_in_state == 40) {
            // prevent getting stuck on playing
            if (sound_is_playing("water_enter")) sound_stop("water_enter");
        }

        //console::log(class_name(), "::state_dive() time in state: ", m_time_in_state, "\n");

        m_current_anim = anim("swim");

        moved_velocity({});

        Vec2F jump_acc = { 0.05F, 0.2F };

        acceleration(jump_acc);

        if (!is_pressed(input::Key::ctrl)) {            
            if (is_pressed(key_left)) {                  
                velocity_add_x(-acceleration().x);
            }
            if (is_pressed(key_right)) {            
                velocity_add_x(acceleration().x);
            }
        }

        if (velocity().y > 0.0F) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                }
            }
        }
        velocity_add({ 0.0F, acceleration().y });

        anim::speed(m_current_anim, 0.0F);
        anim::source_x(m_current_anim, anim::source(m_current_anim).w * 7);

        if (velocity().y >= 0.0F and (m_is_on_ground or is_pressed(key_jump) or is_pressed(key_melee))
            and
            m_next_state != state::Type::run) {
            console::log(class_name(), "::state_dive() landed on ground\n");
            m_next_state = state::Type::run;
            return;
            //next_state(state::Type::run);
            //console::log("is_on_ground: ", m_is_on_ground, "\n");
        }



        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) {
            radians += 3.1415926535F * 2.0F;
        }
        F32 degrees = radians * 180.0F / 3.1415926535F;
        //console::log("degrees: ", degrees, "\n");
        sprite::origin(m_sprite, { sprite::rect(m_sprite).w / 2.0F, 48.0F });

        if (is_pressed(key_jump) and m_num_jumps == 0) {
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

        if (!is_pressed(key_jump)) {
            sprite_is_leftward(false);
            if (velocity().x < 0.0F) {
                sprite_is_upended(true);
            }
            else if (velocity().x > 0.0F) {
                sprite_is_upended(false);
            }
        }
    }
}