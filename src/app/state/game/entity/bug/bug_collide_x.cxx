module;
#include <cmath>
#include <sstream>
module entity.bug;
import aabb;
import console;
import sound;
import particle_system;

namespace entity {
    void Bug::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent || is_dead() || !other.owner || other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::Name other_name = aabb::name(other.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };


        if (is_clip(other_type) ||
            (other_type == Type::brick && !other.owner->is_tossed())) {
            //console::log(to_string(other_type), " has parent\n");
            if (our_rect.w > other_rect.w) {
                m_is_near_wall_L = true;
            }
            if (our_rect.x < other_rect.x) {
                m_is_near_wall_R = true;
            }
        }
        if (m_parent) {
            return;
        }

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);
        //cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        if (is_arch(other_type)) {
            if (m_parent || other.owner->parent() || is_hurting()) return;
            console::log(class_name(), "::collide_x ", to_string(other_type), "\n");
            position_add_x(-overlap_x);
            if (std::abs(our_velocity.x - other_velocity.x) >= 2.0F) {

                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        }
        else if (other_type == Type::bug) {
            if (other.owner->parent()) return;

            if (m_state == State::upended || m_state == State::bounce) {
                if (other_velocity.y > 1.0F) {
                    m_state = State::upended;
                    m_next_state = State::bounce;
                    return;
                }
            }
            position_add_x(-overlap_x);
            velocity_x(velocity().x * -1.0F);
            moved_velocity_x(0.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));
        }
        else if (other_type == Type::brick) {
            if (sprite::is_leftward(m_sprite_id) && velocity().x < 0.0F) {
                return;
            }
            if (!sprite::is_leftward(m_sprite_id) && velocity().x > 0.0F) {
                return;
            }
            time_left_colliding_with(other_type, 4);

            //if (m_parent || other.owner->parent()) return;
            console::log(class_name(), "::collide_x ", to_string(other_type), "\n");

            position_add_x(-overlap_x);
            
            //aabb::time_left_colliding_x(our.id, 100);

            sprite::is_leftward(m_sprite_id, velocity().x > 0.0F);

            if (std::abs(other_velocity.x) >= 1.0F && other.owner->state() == State::carried) {
                velocity_x(other_velocity.x);
                velocity_y(-2.0F);
            }

            /*if (m_is_near_wall_L) {
                velocity_x(velocity().x * -1.0F);
            }
            else if (m_is_near_wall_R) {
                velocity_x(velocity().x * -1.0F);
            }*/
            //if (std::abs(our_velocity.x - other_velocity.x) >= 2.0F) {

            //    cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
            //                       (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

            //    velocity_x(other_velocity.x);
            //    other.owner->velocity_x(our_velocity.x);
            //    spawn(particle::Type::hit, hit_pos, velocity());
            //    hurt(other.owner);
            //    //other.owner->hurt(this);
            //    return;
            //}
            //
            //velocity_x(velocity().x * -0.9F);
            //moved_velocity_x(0.0F);
        }
        else if (other_type == Type::clip || other_type == Type::clip_ledge) {
            if (our_rect.h > other_rect.y && our_rect.h - other_rect.y < 2.0F) return;
            //if ((velocity().x < 0.0F && !m_is_near_wall_L) ||
            //    (velocity().x > 0.0F && !m_is_near_wall_R)) {
            position_add_x(-overlap_x);
            //}

            moved_velocity_x(0.0F);
            //if (m_time_left_turning > 0 || !m_is_on_ground) return;

            if (m_state == State::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                m_force.x = 0.0F;
                velocity_x(velocity().x * -1.0F);
                if (our_rect.x < other_rect.x) {
                    if (velocity().x > -1.0F) {
                        velocity_x(-1.0F);
                    }
                }
                else if (our_rect.w > other_rect.w) {
                    if (velocity().x < 1.0F) {
                        velocity_x(1.0F);
                    }
                }
                return;
            }
            
            if (m_state == State::tossed && std::abs(velocity().x) > 2.0F) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::hurt;

                return;
            }
            if (m_state == State::upended) {
                console::log(class_name(), "::collide_x velocity ", velocity().x, "\n");
                if (velocity().x < 0.0F) {
                    m_is_near_wall_L = true;
                }
                else if (velocity().x > 0.0F) {
                    m_is_near_wall_R = true;
                }
                velocity_x(0.0F);
            }
            if (m_state == State::walk) {
                if (velocity().x < 0.0F) {
                    if (our_rect.x > other_rect.x) {
                        velocity_x(1.0F);
                        //position().x += 2.0F;
                        //m_time_left_turning = 30;
                    }
                }
                else if (velocity().x > 0.0F) {

                    if (our_rect.x < other_rect.x) {
                        velocity_x(-1.0F);
                        //position().x -= 2.0F;
                        //m_time_left_turning = 30;
                    }
                }
                sprite::is_leftward(m_sprite_id, velocity().x < 0.0F);
            }
        }
        else if (other_type == Type::clip_L || other_type == Type::clip_LD) {
            if (velocity().x + moved_velocity().x < 0.0F) return;
            position_add_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == State::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                                                
                velocity_x(velocity().x * -1.0F);

                m_force.x = 0.0F;
                if (velocity().x > -1.0F) {
                    velocity_x(-1.0F);
                }

                return;
            }

            if (m_state == State::tossed ||
                m_state == State::upended && (velocity().x > 1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                        (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::hurt;
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            velocity_x(std::abs(velocity().x) * -1.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

        }
        else if (other_type == Type::clip_R || other_type == Type::clip_RD) {
            if (velocity().x + moved_velocity().x > 0.0F) return;

            position_add_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == State::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                
                m_force.x = 0.0F;

                velocity_x(velocity().x * -1.0F);
                m_force.x = 0.0F;
                if (velocity().x < 1.0F) {
                    velocity_x(1.0F);
                }
                return;
            }

            if (m_state == State::tossed ||
                m_state == State::upended && (velocity().x < -1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = State::hurt;

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            velocity_x(std::abs(velocity().x) * 1.0F);
            sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));

        }
        else if (other_type == Type::frog) {
            position_add_x(-overlap_x);
            if (m_state == State::tossed) {
                cVec2F hit_pos = { (our_rect.x < other_rect.x ? other_rect.x : other_rect.w) - 8.0F,
                                   (our_rect.y < other_rect.y ? other_rect.y : other_rect.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                other.owner->hurt(this);
                m_next_state = State::hurt;

                //velocity().y *= -1.0f;

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
        }
        else if (other_type == Type::mole) {
            if (m_state == State::swim && other.owner->state() == State::swim) {                

                position_add_x(-overlap_x);

                velocity_x(-other_velocity.x * 0.5F);
                //other.owner->velocity_x(our_velocity.x * 0.5F);

                force_x(force().x - overlap_x);

                /*if (force().x < 0.0F && other.owner->is_near_wall_L()) {
                    force_x(10.0F);
                    console::log("near L\n");
                }
                else if (force().x > 0.0F && other.owner->is_near_wall_R()) {
                    force_x(-10.0F);
                    console::log("near R\n");
                }
                else {
                    force_x(-other_velocity.x * 1.0F);
                    other.owner->force_x(our_velocity.x * 1.0F);
                }*/
            }
        }
        else if (other_type == Type::particle_interact) { }
        else if (other_type == Type::player) {
            //console::log("entity::Bug::collide_x player overlap_x: ", overlap_x, "\n");
            /*if (other_velocity.y > 3.0F) {
                collide_y(our, other);
                return;
            }*/
            
            if (m_state == State::tossed) return;
            //console::log("aabb name: ", aabb::to_string(other_name), "\n");

            if (other_rect.h < our_rect.y + 4.0F) return;

            /*if (!m_is_near_wall_L && !m_is_near_wall_R) {
                position_add_x(-overlap_x);
                velocity_x(other_velocity.x);
            } else {
                if (m_is_near_wall_L) {
                    other.owner->position_add({ 2.0F, 0.0F });
                    other.owner->velocity_x(2.0F);
                } else if (m_is_near_wall_R) {
                    other.owner->position_add({ -2.0F, 0.0F });
                    other.owner->velocity_x(-2.0F);
                }
            }*/

            if (other.owner->state() == State::sling) {
                velocity_x(-other.owner->rotation_speed() * 0.5F);
                hurt(other.owner);
                return;
            }

            position_add_x(-overlap_x);
            
            if (m_state == State::upended || m_state == State::bounce) {
                velocity_x(other_velocity.x);
                other.owner->position_add_x(overlap_x);                
            }
            else if (m_state == State::walk) {
                bool prev_is_left = sprite::is_leftward(m_sprite_id);

                cVec2F vel_sum = our_velocity + other_velocity;

                bool is_left = -vel_sum.x < 0.0F;

                console::log(class_name(), "::collide_x() player velocity sum: ", vel_sum.x, "\n");

                if (vel_sum.x <= 0.1F && !sprite::is_leftward(m_sprite_id)) {
                    sprite::is_leftward(m_sprite_id, true);
                    velocity_x(-1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                }
                else if (vel_sum.x >= -0.1F && sprite::is_leftward(m_sprite_id)) {
                    sprite::is_leftward(m_sprite_id, false);
                    velocity_x(1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                }
                else {
                    if (prev_is_left && velocity().x > 0.0F || !prev_is_left && velocity().x < 0.0F) {
                        sprite::is_leftward(m_sprite_id, !sprite::is_leftward(m_sprite_id));
                    }                    
                    other.owner->velocity_x(other.owner->velocity().x * -0.9F + our_velocity.x * 2.0F);
                    other.owner->velocity_y(-1.0F);
                }
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            //if (our_rect.h > other_rect.h) return;

            //velocity_y(-velocity().x;
            position_add({ 0.0F, -velocity().x });
            /*if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (our_rect.h > other_rect.h) return;

            //velocity_y(velocity().x);
            position_add({ 0.0F, velocity().x });
            /*if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            moved_velocity({});
            if (velocity().x > 0.0F) {
                velocity_y(-velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            moved_velocity({});
            if (velocity().x < 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::water_line) {
            collide_y(our, other);
        }
    }
}