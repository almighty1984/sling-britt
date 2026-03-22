module;
#include <cmath>
#include <sstream>
module entity.bug;
import aabb;
import console;
import sound;
import particle_system;

namespace entity {
    void Bug::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent || is_dead() || other.owner->is_dead()) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (velocity().y > 0.0F && our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }*/

        //if (velocity().y > 0.0F && our_rect.y > other_rect.h - 4.0F) return;


        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        //console::log("type: ", to_string(other_type), "\n");

        if (m_state == State::swim && m_num_jumps == 1) {
            if (is_clip(other_type) && other_velocity.y >= 0.0F && our_rect.h < m_water_line_y) {
                m_next_state = State::walk;
                m_num_jumps = 0;
            }
        }

        if (is_arch(other_type)) {
            if (m_parent || other.owner->parent() || is_hurting()) return;
            console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
            add_position_y(-overlap_y);
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        }
        else if (other_type == Type::bug) {
            if (other.owner->parent()) return;

        }
        else if (other_type == Type::brick) {
            if (m_parent || other.owner->parent()) return;
            //if (our_rect.y > other_rect.y) return;
            //if (our_rect.y > other_rect.h - 4.0F) return;

            
            if (our_rect.h < other_rect.y + 2.0F) {
                add_position_y(-overlap_y);
                console::log(class_name(), "::collide_y brick above\n");
            }
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                //other.owner->hurt(this);
                return;
            }
        }
        else if (other_type == Type::clip || other_type == Type::clip_ledge) {
            if (our_rect.y < other_rect.h) {
                if (velocity().y > 0.0F) {
                    add_position_y(-overlap_y);
                    //console::log(class_name(), "::collide_y hmmmmm\n");                
                }
            }
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
            }

            //if (!m_is_on_ground) return;
            if (other_type == Type::clip_ledge) {
                if (velocity().y > 0.0F && our_rect.y > other_rect.y) return;

                if (velocity().y <= -2.0F || velocity().y >= 2.0F) {
                    velocity_y(velocity().y * -0.9F);
                    cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                       (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    hurt(other.owner);
                    m_next_state = State::hurt;
                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");
                    return;
                }
                if (m_state == State::walk) {
                    if (velocity().x > 0.0F && other_rect.w > our_rect.w) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite_id, true);
                    }
                    else if (velocity().x < 0.0F && other_rect.x < our_rect.x) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite_id, false);
                    }
                }
            }
        }
        else if (other_type == Type::clip_U || other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            if (our_rect.y > other_rect.y) return;

            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::clip_D || other_type == Type::clip_LD || other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            add_position_y(-overlap_y);
            if (velocity().y <= -2.0F || velocity().y >= 2.0F) {
                velocity_y(velocity().y * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::hurt;
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            m_next_state = State::hurt;
        } else if (other_type == Type::frog) {
            console::log("entity::Bug::collide_y frog\n");
            if (m_state == State::tossed || (m_state == State::upended && our_rect.y < other_rect.y)) {
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::hurt;
                other.owner->hurt(this);
                //velocity().x *= -1.0f;
                velocity_y(velocity().y * -1.0f);
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }

            if (our_rect.h < other_rect.y + 4.0F) {
                add_position_y(-overlap_y);
                if (m_state == State::upended) {
                    m_next_state = State::bounce;
                }
            }
            //if (other_rect.y > our_rect.y) {            
                //if (!is_hurting()) {
                //    velocity_y(other.owner->velocity().y + velocity().y - 10.0f;
                //}
            //} else {
                //add_position_y( -overlap_y );            
            //}
        }
        else if (other_type == Type::particle_interact) {
            collide_x(our, other);
        }
        else if (other_type == Type::player) {
            if (m_state == State::upended || m_state == State::bounce) {                                
                if (other_rect.h < our_rect.y + 4.0F && other_velocity.y > 2.0F) {
                    console::log("entity::bug::collide_y() player\n");
                    m_state = State::upended;
                    m_next_state = State::bounce;              
                }
                return;
            }
            //console::log(class_name(), "::collide_y player overlap_y: ", overlap_y, "\n");
            if (m_state == State::swim) {
                //velocity().y += other_velocity.y;
                //moved_velocity_y(other_velocity.y;
                return;
            }
            if (other.owner->state() == State::sling) return;

            if (our_rect.y > other_rect.h - 2.0F/* - other_velocity.y*/) {
                
            }
            else if (our_rect.h <= other_rect.y + 4.0F) {
                return;
                cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            add_position_y(-overlap_y);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x > 0.0F) {
                //velocity_y(velocity().x / 2.0F;
            //}
            if (velocity().y > 0.0F) {
                velocity_x(velocity().y / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (is_water_line(other_type)) {
            if (m_state == State::swim && m_num_jumps == 1 && velocity().y < 0.0F) return;

            bool is_to_splash = false,
                 is_upwards   = false;

            m_num_jumps = 0;

            m_radians = other.owner->radians();
            
            if (velocity().y > 0.0F && m_state != State::swim) {
                m_next_state = State::swim;
            }

            m_water_line_y = other.owner->start_offset().y - 6.0F;

            if (position_on_level().y + 0.0F > m_water_line_y) return;

            if (velocity().y <= -2.0F) {
                is_upwards = true;
                if (!sound_is_playing("water_exit") && !sound_is_playing("water_enter")) {
                    sound_position("water_exit", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("water_exit");
                    is_to_splash = true;
                }
                m_time_in_state = 0;
                m_is_on_ground = false;
            }
            else if (velocity().y > 2.0F) {
                is_upwards = false;
                if (!sound_is_playing("water_enter") && !sound_is_playing("water_exit")) {
                    sound_position("water_enter", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("water_enter");
                    is_to_splash = true;
                }
            }

            if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
            else {
                velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }


            if (is_to_splash) {
                Vec2F pos = Vec2F{ our_rect.x, other_rect.y };
                particle::spawn_fan(this, 225.0F, 315.0F, 8,
                                    particle::Type::drop, pos + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F },
                                    Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) },
                                    is_upwards ? 2.0F : 1.5F);
            }
        }
    }
}