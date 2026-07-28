module entity.particle.brick;
import app.config;
import particle_system;

namespace entity {
    void ParticleBrick::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F other_size = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        bool is_to_play_bounce = false;

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_R_1x1: {
                velocity_x(our_velocity.x * 0.5F);
                break;
            }
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                velocity_x(our_velocity.x * -0.75F);
                break;
            }
            case Type::brick: {
                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -0.5F);
                break;
            }
            case Type::bug: {
                //console::log(class_name(), "::collide_x ", to_string(other_type), "\n");
                if (is_hurting() or (std::abs(other_velocity.x) < 0.5F and std::abs(our_velocity.x) < 0.5F)) return;

                m_time_left_hurt = m_config.time_to_hurt();
                m_time_left_dead = m_config.time_to_be_dead();

                velocity_y(other_velocity.y - 2.0F);
                velocity_x(other_velocity.x * 1.2F);
                break;
            }
            case Type::clip:
            case Type::clip_L:
            case Type::clip_L_50:
            case Type::clip_R:
            case Type::clip_R_50:
            case Type::clip_LR:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50: {
                if ((our_velocity.x < 0.0F and (other_type == Type::clip_L or
                                                other_type == Type::clip_L_50 or
                                                other_type == Type::clip_ledge_L_50))
                    or
                    (our_velocity.x > 0.0F and (other_type == Type::clip_R or
                                                other_type == Type::clip_R_50 or
                                                other_type == Type::clip_ledge_R_50))) {
                    return;
                }
                if (other_UL.x < our_UL.x and velocity().x > 0.0F or
                    other_DR.x > our_DR.x and velocity().x < 0.0F) return;
                position_add_x(-overlap_x);
                velocity_x(velocity().x * -0.75F);

                if (std::abs(our_velocity.x) >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::frog: {
                if (std::abs(velocity().x) >= 3.0F and !other.owner->is_hurting()) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    if (!sound_is_playing("melee")) {
                        sound_position("melee", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                        sound_play("melee");
                    }
                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                }
                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -1.0F);

                if (std::abs(our_velocity.x) >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::mole: {
                if (is_hurting() or other.owner->is_hurting() or other.owner->state() == state::Type::swim) return;

                if (other.owner->state() == state::Type::idle) {
                    m_time_left_hurt = m_config.time_to_hurt();
                    m_time_left_dead = m_config.time_to_be_dead();
                    //m_time_in_state = 5;
                    velocity_y(other_velocity.y - 2.0F);
                    velocity_x(other_velocity.x * 1.0F);
                    return;
                }
                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -1.0F);
                break;
            }
            case Type::slope_L_1x1: {
                //position_add_x( -overlap_x );
                collide_y(our, other);
                break;
            }
            case Type::slope_R_1x1: {
                //position_add_x( -overlap_x );
                collide_y(our, other);
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                //position_add_x( -overlap_x );
                collide_y(our, other);
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                //position_add_x( -overlap_x );
                collide_y(our, other);
            }
            case Type::particle_brick: {
                //if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;

                //position_add_x( -overlap_x );
                //velocity_x(our_velocity.x * 0.4F + other_velocity.x * 0.4F;
                //other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F;

                //velocity_x(other_velocity.x * 0.8F;
                //other.owner->velocity_x(our_velocity.x * 0.8F;

                /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                    other.owner->velocity_x(velocity().x;
                } else {
                    velocity_x(other.owner->velocity().x;
                }*/
                break;
            }
            case Type::particle_down_thrust: {
                collide_y(our, other);
                break;
            }
            case Type::player: {
                if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;

                m_time_left_hurt = m_config.time_to_hurt();
                m_time_left_dead = m_config.time_to_be_dead();

                m_time_in_state = 5;

                //position_add_x( -overlap_x );
                //position().y -= 2.0F;
                velocity_y(other_velocity.y - 2.0F);

                velocity_x(other_velocity.x * 1.2F);
                //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

                //other.owner->velocity().x *= 0.95F;
                break;
            }
            case Type::train_saw: {
                position_add_x(-overlap_x);
                cVec2F aabb_vel = aabb::velocity(other.id);
                velocity(other_velocity + aabb_vel * 0.75F - our_velocity * 0.25F);
                hurt(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
        if (is_to_play_bounce) {
            F32 abs_vel_x = std::abs(our_velocity.x) / 3.0F;
            F32 pitch = 1.0F;
            if (abs_vel_x > 1.0F) {
                if (abs_vel_x > 1.2F) {
                    abs_vel_x = 1.2F;
                }
                pitch = abs_vel_x;
            }
            sound_pitch("bounce", pitch + random::number(-10, 10) / 100.0F);
            sound_position("bounce", { position().x + 2.0F - app::config::extent().x / 2.0F,
                                       position().y + 2.0F - app::config::extent().y / 2.0F });
            sound_play("bounce");
        }
    }
}