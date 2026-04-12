module entity.frog;
import app.config;
import aabb;
import console;
import particle_system;

namespace entity {
    void Frog::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner or !other.owner) return;

        aabb::cName our_name = aabb::name(our.id);

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        if (velocity().y > 0.0F and our_points.h - 4.0F - velocity().y > other_points.y) {
            return;
        }

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);

        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        if (m_state == state::Type::swim) {
            if (position_on_level().y < m_water_line_y) {
                if (is_clip(other_type) or is_slope(other_type)) {
                    m_next_state = state::Type::idle;
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
            if (other.owner->state() == state::Type::tossed) {
                return;
            }
            if (other_points.y > our_points.y) return;

            position_add_y(-overlap_y);

            if (our_points.y > other_points.y) {
                hurt(other.owner);
                m_next_state = state::Type::hurt;
                /*Vec2F our_center = Vec2F{ our_points.x + (our_points.w - our_points.x) / 2.0F,     our_points.y + (our_points.h - our_points.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_points.x + (other_points.w - other_points.x) / 2.0F, other_points.y + (other_points.h - other_points.y) / 2.0F };

                spawn(particle::Type::hit, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                hurt(other_type);*/
                return;
            }
            if (other.owner->state() == state::Type::upended or other.owner->state() == state::Type::bounce) {
                if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);
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
        else if (other_type == Type::clip or other_type == Type::clip_ledge) {
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_points.y < other_points.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
            }
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            moved_velocity_x(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            position_add_y(-overlap_y);
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            particle::spawn(this, particle::Type::hit, { other_points.x + 8.0F, other_points.h }, {});
            //, sound("melee")
        }
        else if (other_type == Type::frog) {
            if (our_points.h <= other_points.y) {
                position_add_y(-overlap_y);
                velocity_y(-4.0F);

                velocity_x(other.owner->velocity().x + other.owner->moved_velocity().x);
                velocity_add({ 0.0F, other.owner->velocity().y + other.owner->moved_velocity().y });

                Vec2F our_center = Vec2F{ our_points.x + (our_points.w - our_points.x) / 2.0F,     our_points.y + (our_points.h - our_points.y) / 2.0F };
                Vec2F other_center = Vec2F{ other_points.x + (other_points.w - other_points.x) / 2.0F, other_points.y + (other_points.h - other_points.y) / 2.0F };

                particle::spawn(this, particle::Type::hit,
                                { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                //, sound("melee")
            }
        }
        else if (other_type == Type::particle_health) {
            collide_x(our, other);
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

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
            position_add_y(-overlap_y);

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
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

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
        else if (other_type == Type::particle_shot) {
            hurt(other.owner);
        }
        else if (other_type == Type::player) {
            if (other.owner->state() == state::Type::sling) {
                collide_x(our, other);
                return;
            }
            if (our_points.y <= other_points.y) {

                position_add_y(-overlap_y);

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
                    m_next_state = state::Type::hurt;
                }

                //cVec2F our_center = Vec2F{ our_points.x + (our_points.w - our_points.x) / 2.0F,     our_points.y + (our_points.h - our_points.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_points.x + (other_points.w - other_points.x) / 2.0F, other_points.y + (other_points.h - other_points.y) / 2.0F };

                cVec2F hit_pos = { other_center.x - 8.0F, other_points.y - 12.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});

                other.owner->velocity_y(-other.owner->velocity().y * 0.9F);

                sound_position("bump_head", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("bump_head");

                //next_state(state::Type::hurt);
                //m_time_left_in_state = 200;
            }
        }
        else if (other_type == Type::water_line) {
            if (velocity().y > 0.0F and m_state != state::Type::swim) {
                m_next_state = state::Type::swim;

                m_water_line_y = other.owner->start_position().y;

            }
            if (m_state == state::Type::swim) {
                if (position().y > m_water_line_y) {
                    velocity_y(other_velocity.y);
                }
            }
        }
    }
}