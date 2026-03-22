module;
#include <cmath>
#include <sstream>
module entity.mole;
import console;
import aabb;
import particle_system;

namespace entity {
    void Mole::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() || !other.owner || other.owner->is_dead()) return;
        
        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        entity::cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        if (time_left_colliding_with(other_type) > 0) return;

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);


        /*if (is_slope(other_type) && other_type != Type::slope_U) {
            if (our_rect.h - 4.0F < other_rect.h) return;
        }*/

        if (other_type == Type::brick) {
            if (m_state != State::idle && other.owner->state() == State::tossed) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::clip_U || other_type == Type::clip_ledge || other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;

            if (other_type == Type::clip_ledge) {
                if (other_rect.x < our_rect.x) {
                    sprite_is_leftward(false);
                } else if (other_rect.w > our_rect.w) {
                    sprite_is_leftward(true);
                }
            }
            sprite_angle(0.0F);
        }
        else if (other_type == Type::particle_brick) {
            if (m_state != State::idle && !(other_velocity.y > -2.0F && other_velocity.y < 2.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::player) {
            if (m_state == State::exit && !sound_is_playing("bounce")) {
                sound_position("bounce", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("bounce");
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            add_position_y(-overlap_y);
            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(45.0F - 90.0F);
        }
        else if (other_type == Type::slope_R_1x1) {
            add_position_y(-overlap_y);
            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(135.0F - 90.0F);
        }
        else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(67.5F - 90.0F);
        }
        else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(112.5F - 90.0F);
        }
        else if (is_water_line(other_type)) {
            m_radians = other.owner->radians();
            m_water_line_y = other.owner->start_offset().y - 4.0F;
            
            bool is_to_splash = false,
                 is_upwards   = false;
            if (velocity().y < 0.0F && m_next_state == State::jump && m_state != State::jump) {
                time_left_colliding_with(Type::water_line, 10);

                m_next_state = State::jump;
                is_upwards = true;
                if (velocity().y < 0.0F && !sound_is_playing("water_exit")) {
                    sound_position("water_exit", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("water_exit");
                    is_to_splash = true;
                }
            }
            else if (velocity().y > 0.0F && m_next_state != State::swim) {
                m_next_state = State::swim;
                if (velocity().y > 0.0F && !sound_is_playing("water_enter")) {
                    //sound::position(sound_id("water_enter"), { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    //sound::play(sound_id("water_enter"));
                    sound_position("water_enter", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("water_enter");
                    is_to_splash = true;
                }
            }
            if (is_to_splash) {
                console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                Vec2F pos = Vec2F{ our_rect.x, other_rect.y };
                particle::spawn_fan(this, 225.0F, 315.0F, 8,
                                    particle::Type::drop,
                                    pos + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F },
                                    Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) }, 2.0F);
            }

            if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
            else {
                velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
        }
    }
}