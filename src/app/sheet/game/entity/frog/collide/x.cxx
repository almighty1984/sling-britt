module entity.frog;
import aabb;
import console;

namespace entity {
    void Frog::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner or !other.owner or is_dead()) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        Vec2F our_extent = { aabb::UR(our.id).x - aabb::UL(our.id).x,
                             aabb::DR(our.id).y - aabb::UR(our.id).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        state::Type other_state = other.owner->state();

        switch (other_type) {
            case Type::bug: {
                position_add_x(-overlap_x);
                if (other.owner->state() == state::Type::tossed) {
                    return;
                }
                velocity_x(other_velocity.x * 0.5F);
                other.owner->velocity_x(our_velocity.x * 0.25F);

                if (our_UL.x < other_UL.x) {
                    //velocity().x -= 1.0F;
                    other.owner->velocity_add({ 1.0F, 0.0F });
                    sprite::is_leftward(other.owner->sprite(), false);
                } else {
                    //velocity().x += 1.0F;
                    other.owner->velocity_add({ -1.0F, 0.0F });
                    sprite::is_leftward(other.owner->sprite(), true);
                }
                break;
            }
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50:
            case Type::clip: {
                position_add_x(-overlap_x);
                if (m_state == state::Type::swim) {
                    if (our_velocity.x < 0.0F) {
                        direction_x(1.0F);
                        velocity_x(velocity().x * -1.0F);
                        position_add({ 2.0F, 0.0F });
                    } else if (our_velocity.x > 0.0F) {
                        direction_x(-1.0F);
                        velocity_x(velocity().x * -1.0F);
                        position_add({ -2.0F, 0.0F });
                    }
                }
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                break;
            }
            case Type::clip_LD:
            case Type::clip_L:
            case Type::clip_L_50: {
                if (velocity().x + move_velocity().x <= 0.0F) return;
                if (m_state == state::Type::swim) {
                    sprite_is_leftward(true);
                }
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                break;
            }
            case Type::clip_RD:
            case Type::clip_R:
            case Type::clip_R_50: {
                if (velocity().x + move_velocity().x >= 0.0F) return;
                if (m_state == state::Type::swim) {
                    sprite_is_leftward(false);
                }
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                break;
            }
            case Type::frog: {
                position_add_x(-overlap_x);
                if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                    return;
                }
                velocity_x(other_velocity.x * 0.5F);
                other.owner->velocity_x(our_velocity.x * 0.25f);
                if (our_UL.x < other_UL.x) {
                    velocity_add({ -1.0F, 0.0F });
                    other.owner->velocity_add({ 1.0F, 0.0F });
                } else {
                    velocity_add({ 1.0F, 0.0F });
                    other.owner->velocity_add({ -1.0F, 0.0F });
                }
                break;
            }
            case Type::particle_health: {
                if (health_amount() <= 0.0F or health_is_max() or m_state == state::Type::heal or m_state == state::Type::dead) return;
                m_next_state = state::Type::heal;
                m_time_to_be_in_state = 200;
                break;
            }
            case Type::player: {
                position_add_x(-overlap_x);
                if (other.owner->state() == state::Type::sling) {
                    //cVec2F aabb_vel = aabb::velocity(our.id);
                    //console::log("aabb vel: ", aabb_vel.x, " ", aabb_vel.y, "\n");
                    //velocity(other_velocity + aabb_vel - our_velocity);

                    console::log("other force: ", other.owner->force().x, " ", other.owner->force().y, "\n");

                    velocity(other.owner->force() - our_velocity);
                    //velocity_x(-other.owner->rotation_speed() * 0.75F);
                    hurt(other.owner);

                    //m_sensed.clear();
                    return;
                }
                /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                    return;
                }*/

                //if (m_state == state::Type::swim) return;

                cF32 force_x = other_state == state::Type::duck ? 4.0F : 1.0F;

                velocity_x(other_velocity.x * 0.5F);
                other.owner->velocity_x(our_velocity.x * 0.25f);
                if (our_UL.x < other_UL.x) {
                    velocity_add({ -1.0F, 0.0F });
                    other.owner->velocity_add({ force_x, 0.0F });
                } else {
                    velocity_add({ 1.0F, 0.0F });
                    other.owner->velocity_add({ -force_x, 0.0F });
                }
                move_velocity_x(0.0F);
                break;
            }
            case Type::slope_L_1x1: {
                velocity_y(-velocity().x);
                if (velocity().x > max_velocity().x * 0.7F) {
                    velocity_x(max_velocity().x * 0.7F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                velocity_y(-velocity().x / 2.0F);
                if (velocity().x > max_velocity().x * 0.9F) {
                    velocity_x(max_velocity().x * 0.9F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_1x1: {
                velocity_y(velocity().x);
                if (velocity().x < -max_velocity().x * 0.7F) {
                    velocity_x(-max_velocity().x * 0.7F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                velocity_y(velocity().x / 2.0F);
                if (velocity().x < -max_velocity().x * 0.9F) {
                    velocity_x(-max_velocity().x * 0.9F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::train_saw: {
                position_add_x(-overlap_x);
                cVec2F aabb_vel = aabb::velocity(other.id);                
                velocity(other_velocity + aabb_vel - our_velocity);                
                if (other.owner->time_left_alive() > 0) {
                    hurt(other.owner);
                } else {
                    health_amount_add(-4.0F);
                }                
                break;
            }
        }
    }
}