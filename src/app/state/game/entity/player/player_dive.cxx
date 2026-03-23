module;
#include <cmath>
module entity.player;
import console;
import random;
import particle_system;

namespace entity {
    void Player::state_dive() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            
            sound_stop("hover");
            sound_stop("slide_wall");
        }
        //console::log(class_name(), "::state_dive() velocity: ", velocity().x, " ", velocity().y, "\n");;

        ++m_time_in_state;

        if (m_time_in_state == 40) {
            // prevent getting stuck on playing
            if (sound_is_playing("water_enter")) sound_stop("water_enter");
        }

        m_current_anim_id = anim_id("swim");

        moved_velocity({});

        Vec2F jump_acc = { 0.05F, 0.2F };

        acceleration(jump_acc);

        if (!is_pressed(input::Key::ctrl)) {            
            if (is_pressed(key_left)) {                  
                add_velocity_x(-acceleration().x);
            }
            if (is_pressed(key_right)) {            
                add_velocity_x(acceleration().x);
            }
        }

        if (velocity().y > 0.0F) {
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                }
            }
        }
        add_velocity({ 0.0F, acceleration().y });

        anim::speed(m_current_anim_id, 0.0F);
        anim::source_x(m_current_anim_id, anim::source(m_current_anim_id).w * 7);

        if (velocity().y >= 0.0F && (m_is_on_ground || is_pressed(key_jump) || is_pressed(key_melee))
            &&
            m_next_state != State::run) {
            console::log("entity::Player::state_dive() landed on ground\n");
            m_next_state = State::run;
            return;
            //next_state(State::run);
            //console::log("is_on_ground: ", m_is_on_ground, "\n");
        }



        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) {
            radians += 3.1415926535F * 2.0F;
        }
        F32 degrees = radians * 180.0F / 3.1415926535F;
        //console::log("degrees: ", degrees, "\n");
        sprite::origin(m_sprite_id, { sprite::source_rect(m_sprite_id).w / 2.0F, 48.0F });

        if (is_pressed(key_jump) && m_num_jumps == 0) {
            return;
        }

        if (std::abs(velocity().x) > acceleration().x ||
            std::abs(velocity().y) > acceleration().y ||
            is_pressed(key_left) || is_pressed(key_right) ||
            is_pressed(key_up) || is_pressed(key_down)
            ) {

            //if (degree_diff > 10.0F) {
                //sprite::add_angle(m_sprite_id, degree_diff * 0.1F);
            //} else {
            sprite_angle(degrees);
            //}
        }

        sprite_is_leftward(false);
        if (velocity().x < 0.0F) {
            sprite_is_upended(true);
        } else if (velocity().x > 0.0F) {
            sprite_is_upended(false);
        }
    }
}