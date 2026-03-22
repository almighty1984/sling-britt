module;
#include <utility>

module entity.frog;
import aabb;
import console;
import particle_system;

namespace entity {
    void Frog::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner || !other.owner) return;

        aabb::cName our_name = aabb::name(our.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        if (velocity().y > 0.0F && our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        if (m_state == State::swim) {
            if (position_on_level().y < m_water_line_y) {
                if (is_clip(other_type) || is_slope(other_type)) {
                    m_next_state = State::idle;
                    m_water_line_y = 0;
                }
            }
        }
        if (other_type == Type::brick) {

            /*console::log("Frog::collide_y brick\n");
            if (other_velocity.y >= other.transform()->max_velocity.y) {

            }*/
        }
        else if (other_type == Type::bug) {
            //console::log("Frog::collide_y bug\n");
            if (other.owner->state() == State::tossed) {
                return;
            }
            if (other_rect.y > our_rect.y) return;

            add_position_y(-overlap_y);

            if (our_rect.y > other_rect.y) {
                hurt(other.owner);
                m_next_state = State::hurt;
                /*Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                spawn(particle::Type::hit, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                hurt(other_type);*/
                return;
            }
            if (other.owner->state() == State::upended || other.owner->state() == State::bounce) {
                if (velocity().y < 0.0F) return;
                add_position_y(-overlap_y);
                max_velocity_y(10.0F);
                velocity_y(-5.0F - velocity().y / 4.0F);
                m_is_on_ground = false;
                m_is_on_slope = false;
                //m_time_left_bouncing = m_time_to_bounce;
                return;
            }
            moved_velocity(other_velocity);
            //velocity_x(velocity().x * 0.5F);
            velocity_y(0.0F);
            m_is_on_ground = true;
        }
        else if (other_type == Type::clip || other_type == Type::clip_ledge) {
            add_position_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
            }
        }
        else if (other_type == Type::clip_U || other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            moved_velocity_x(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::clip_D || other_type == Type::clip_LD || other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            add_position_y(-overlap_y);
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            particle::spawn(this, particle::Type::hit, { other_rect.x + 8.0F, other_rect.h }, {});
            //, sound_id("melee")
        }
        else if (other_type == Type::frog) {
            if (our_rect.h <= other_rect.y) {
                add_position_y(-overlap_y);
                velocity_y(-4.0F);

                velocity_x(other.owner->velocity().x + other.owner->moved_velocity().x);
                add_velocity({ 0.0F, other.owner->velocity().y + other.owner->moved_velocity().y });

                Vec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                particle::spawn(this, particle::Type::hit,
                                { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                //, sound_id("melee")
            }
        }
        else if (other_type == Type::particle_health) {
            collide_x(our, other);
        }
        else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x < 0.0F) {
            velocity_y(velocity().x / -2.0F);
            //}        
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x > 0.0F) {
            velocity_y(velocity().x);
            //}
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            //if (velocity().y < 0.0F) return;
            add_position_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (velocity().x > 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::player) {
            if (other.owner->state() == State::sling) {
                collide_x(our, other);
                return;
            }
            if (our_rect.y <= other_rect.y) {

                add_position_y(-overlap_y);

                cI32 rand_num = random::number(-1, 1);

                //console::log("rand_num: ", rand_num, "\n");

                velocity_x(other_velocity.x * 0.5F + our_velocity.x * 0.5F + (F32)rand_num);
                velocity_y(other_velocity.y * 0.5F - 3.0F + (F32)rand_num);

                /*if (other.owner->transform()) {
                    velocity_x(other.owner->velocity().x + other.owner->moved_velocity().x;
                    velocity().y += (other.owner->velocity().y + other.owner->moved_velocity().y);
                }*/

                if (other_velocity.y < 0.0F) {
                    hurt(other.owner);
                    m_next_state = State::hurt;
                }

                //cVec2F our_center = Vec2F{ our_rect.x + (our_rect.w - our_rect.x) / 2.0F,     our_rect.y + (our_rect.h - our_rect.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_rect.x + (other_rect.w - other_rect.x) / 2.0F, other_rect.y + (other_rect.h - other_rect.y) / 2.0F };

                cVec2F hit_pos = { other_center.x - 8.0F, other_rect.y - 12.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});

                other.owner->velocity_y(-other.owner->velocity().y * 0.9F);

                sound_position("bump_head", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("bump_head");

                //next_state(State::hurt);
                //m_time_left_in_state = 200;
            }
        }
        else if (other_type == Type::water_line) {
            if (velocity().y > 0.0F && m_state != State::swim) {
                m_next_state = State::swim;

                m_water_line_y = other.owner->start_position().y;

            }
            if (m_state == State::swim) {
                if (position().y > m_water_line_y) {
                    velocity_y(other_velocity.y);
                }
            }
        }
    }
}