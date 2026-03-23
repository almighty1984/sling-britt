module;
#include <utility>
module entity.frog;
import aabb;
import console;

namespace entity {
    void Frog::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner || !other.owner) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        Vec2F our_dimensions = { aabb::point(our.id, 1).x - aabb::point(our.id, 0).x,
                                 aabb::point(our.id, 3).y - aabb::point(our.id, 1).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        if (other_type == Type::brick) {
            //console::log("Frog::collide_x brick\n");
        }
        else if (other_type == Type::bug) {
            position_add_x(-overlap_x);
            if (other.owner->state() == State::tossed) {
                return;
            }

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25F);

            if (our_rect.x < other_rect.x) {
                //velocity().x -= 1.0F;
                other.owner->velocity_add({ 1.0F, 0.0F });
                sprite::is_leftward(other.owner->sprite_id(), false);
            } else {
                //velocity().x += 1.0F;
                other.owner->velocity_add({ -1.0F, 0.0F });
                sprite::is_leftward(other.owner->sprite_id(), true);
            }
        }
        else if (other_type == Type::clip || other_type == Type::clip_ledge) {
            position_add_x(-overlap_x);
            if (m_state == State::swim) {
                if (velocity().x < 0.0F) {
                    sprite::is_leftward(m_sprite_id, false);
                    velocity_x(velocity().x * -1.0F);
                    position_add({ 1.0F, 0.0F });
                }
                else if (velocity().x > 0.0F) {
                    sprite::is_leftward(m_sprite_id, true);
                    velocity_x(velocity().x * -1.0F);
                    position_add({ -1.0F, 0.0F });
                }
            }
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::clip_L || other_type == Type::clip_LD) {
            if (velocity().x + moved_velocity().x <= 0.0F) return;
            if (m_state == State::swim) {
                sprite::is_leftward(m_sprite_id, true);
            }
            position_add_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::clip_R || other_type == Type::clip_RD) {
            if (velocity().x + moved_velocity().x >= 0.0F) return;
            if (m_state == State::swim) {
                sprite::is_leftward(m_sprite_id, false);
            }
            position_add_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::frog) {
            position_add_x(-overlap_x);
            if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                return;
            }
            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_rect.x < other_rect.x) {
                velocity_add({ -1.0F, 0.0F });
                other.owner->velocity_add({ 1.0F, 0.0F });
            }
            else {
                velocity_add({ 1.0F, 0.0F });
                other.owner->velocity_add({ -1.0F, 0.0F });
            }
        }
        else if (other_type == Type::particle_health) {
            if (health::is_full(m_health_id) || m_state == State::heal) return;

            m_next_state = State::heal;
            m_time_left_in_next_state = 200;
        }
        else if (other_type == Type::particle_melee) { }
        else if (other_type == Type::player) {
            position_add_x(-overlap_x);
            if (other.owner->state() == State::sling) {
                velocity_x(-other.owner->rotation_speed() * 0.75F);
                hurt(other.owner);
                //m_sensed_objects.clear();
                return;
            }
            /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                return;
            }*/

            //if (m_state == State::swim) return;

            cF32 force_x = other.owner->is_ducking() ? 4.0F : 1.0F;

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_rect.x < other_rect.x) {
                velocity_add({ -1.0F, 0.0F });
                other.owner->velocity_add({ force_x, 0.0F });
            } else {
                velocity_add({ 1.0F, 0.0F });
                other.owner->velocity_add({ -force_x, 0.0F });
            }
            moved_velocity_x(0.0F);

        } else if (other_type == Type::slope_L_1x1) {
            velocity_y(-velocity().x);
            if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            velocity_y(-velocity().x / 2.0F);
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::slope_R_1x1) {
            velocity_y(velocity().x);
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            velocity_y(velocity().x / 2.0F);
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
    }
}