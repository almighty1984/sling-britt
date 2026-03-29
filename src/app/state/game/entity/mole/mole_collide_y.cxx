module entity.mole;
import app.config;
import console;
import aabb;
import particle_system;

namespace entity {
    void Mole::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or !other.owner or other.owner->is_dead()) return;
        
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


        /*if (is_slope(other_type) and other_type != Type::slope_U) {
            if (our_rect.h - 4.0F < other_rect.h) return;
        }*/

        if (other_type == Type::brick) {
            if (m_state != state::Type::idle and other.owner->state() == state::Type::tossed) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::clip_U or other_type == Type::clip_ledge or other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
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
            if (m_state != state::Type::idle and !(other_velocity.y > -2.0F and other_velocity.y < 2.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::particle_health) {
            collide_x(our, other);
        }
        else if (other_type == Type::player) {
            if (m_state == state::Type::exit and !sound_is_playing("bounce")) {
                sound_position("bounce", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("bounce");
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            position_add_y(-overlap_y);
            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(45.0F - 90.0F);
        }
        else if (other_type == Type::slope_R_1x1) {
            position_add_y(-overlap_y);
            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(135.0F - 90.0F);
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(67.5F - 90.0F);
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            position_add_y(-overlap_y);
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
            if (velocity().y < 0.0F and m_next_state == state::Type::jump and m_state != state::Type::jump) {
                time_left_colliding_with(Type::water_line, 10);

                m_next_state = state::Type::jump;
                is_upwards = true;
                if (velocity().y < 0.0F and !sound_is_playing("water_exit")) {
                    sound_position("water_exit", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_exit");
                    is_to_splash = true;
                }
            }
            else if (velocity().y > 0.0F and m_next_state != state::Type::swim) {
                m_next_state = state::Type::swim;
                if (velocity().y > 0.0F and !sound_is_playing("water_enter")) {
                    //sound::position(sound_id("water_enter"), { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    //sound::play(sound_id("water_enter"));
                    sound_position("water_enter", { position().x / (app::config::extent().y / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_enter");
                    is_to_splash = true;
                }
            }
            if (is_to_splash) {
                console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                cVec2F pos = Vec2F{ our_rect.x, other_rect.y } + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F };
                cVec2F vel = Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) };

                particle::splash_water(this, pos, vel, 2.0F);
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