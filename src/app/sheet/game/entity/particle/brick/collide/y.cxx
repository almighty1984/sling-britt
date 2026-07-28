module entity.particle.brick;
import app.config;
import particle_system;

namespace entity {
    void ParticleBrick::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner or other.owner->is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        bool is_to_play_bounce = false;

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_R_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (velocity().y < 0.0F) {
                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);

                    if (our_velocity.y <= -m_play_bounce_vel) {
                        is_to_play_bounce = true;
                    }
                }
                break;
            }
            case Type::brick: {
                if (our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                move_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::bridge: {
                if (our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                move_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_UD: {
                if (our_velocity.y < 0.0F and our_DR.y < other_DR.y) return;
                if (our_velocity.y > 0.0F and our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(our_velocity.y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                move_velocity({});

                velocity_x(our_velocity.x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip_U:
            case Type::clip_U_50:
            case Type::slope_U: {
                if (our_velocity.y < 0.0F) return;
                //if (velocity().y > 0.0F and our_UL.y > other_DR.y) return;

                position_add_y(-overlap_y);

                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }

                move_velocity({});

                velocity_x(our_velocity.x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip_D:
            case Type::clip_D_50:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (our_velocity.y > 0.0F) return;
                //if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;

                position_add_y(-overlap_y);
                velocity_y(our_velocity.y * -0.5F);
                move_velocity({});

                velocity_x(our_velocity.x * 0.5F);
                if (our_velocity.y <= -m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::frog: {
                if (std::abs(velocity().y) >= 2.0F and !other.owner->is_hurting()) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    if (!sound_is_playing("melee")) {
                        sound_position("melee", { hit_pos.x - app::config::extent().x / 2.0F,
                                                  hit_pos.y - app::config::extent().y / 2.0F });
                        sound_play("melee");
                    }

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                }
                if (our_UL.y < other_UL.y) {
                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);
                }
                break;
            }
            case Type::mole: {
                if (other.owner->state() == state::Type::idle) {
                    collide_x(our, other);
                    return;
                }
                if ((velocity().y >= -2.0F and velocity().y <= 2.0F)) return;

                if (our_UL.y < other_UL.y) {
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                    sound_position("melee", { hit_pos.x - app::config::extent().x / 2.0F,
                                              hit_pos.y - app::config::extent().y / 2.0F });
                    sound_play("melee");
                    particle::spawn(this, particle::Type::hit, hit_pos, {});

                    position_add_y(-overlap_y);
                    velocity_y(our_velocity.y * -1.0F);
                }
                break;
            }
            case Type::slope_L_1x1: {
                position_add_y(-overlap_y);
                if (our_velocity.y > 1.0F) {
                    velocity_y(our_velocity.y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite_angle(45.0F);

                velocity_x(velocity().x * 0.5F);
                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::slope_R_1x1: {
                position_add_y(-overlap_y);
                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite_angle(135.0F);

                velocity_x(velocity().x * 0.5F);
                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                position_add_y(-overlap_y);
                if (our_velocity.y > 1.0F) {
                    velocity_y(our_velocity.y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite_angle(67.5F);

                velocity_x(our_velocity.x * 0.5F);
                if (our_velocity.y > m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                position_add_y(-overlap_y);
                if (our_velocity.y > 1.0F) {
                    velocity_y(our_velocity.y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }
                sprite_angle(112.5F);

                velocity_x(our_velocity.x * 0.5F);
                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::player: {
                if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;
                m_time_left_hurt = m_config.time_to_hurt();

                position_add({ 0.0F, -2.0F });
                velocity_x(other_velocity.x * 1.2F);
                velocity_y(other_velocity.y - 1.0F);
                break;
            }
            case Type::particle_down_thrust: {
                if (is_hurting() or !other.owner->parent()) return;
                m_time_left_hurt = 30;

                position_add({ 0.0F, -2.0F });
                velocity_x(other_velocity.x * 0.2F);

                cF32 diff_x = (other.owner->parent()->position().x + 4.0F) - (position().x + 2.0F);
                cF32 vel_y = (-2.0F / std::abs(diff_x)) - 2.0F;

                cF32 vel_x = 1.0F / diff_x + (diff_x < 0.0F ? -2.0F : 2.0F);
                console::log("vel_x: ", vel_x, "\n");

                velocity_y(vel_x);
                velocity_y(vel_y);
                break;
            }
            case Type::particle_brick: {
                //if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;
                ////if (other_UL.y > our_UL.y) return;

                ////position_add_y( -overlap_y );
                ////velocity_x(other_velocity.y * 0.5F;
                ////other.owner->velocity_x(our_velocity.y * 0.5F;

                ////velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                ////other.owner->velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
                //velocity_y(0.0F);

                //velocity_x(velocity().x * -0.9F);
                break;
            }
            case Type::train_saw: {
                position_add_y(-overlap_y);
                cVec2F aabb_vel = aabb::velocity(other.id);
                velocity(other_velocity + aabb_vel - our_velocity);
                hurt(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (acceleration() == Vec2F{ 0.05F, 0.05F }) return;

                acceleration({ 0.05F, 0.05F });
                max_velocity({ 0.4F, 0.7F });

                if (our_velocity.y > 0.0F) {
                    if (!sound_is_playing("water_enter")) {
                        sound_position("water_enter", { position().x - app::config::extent().x / 2.0F,
                                                        position().y - app::config::extent().y / 2.0F });
                        sound_play("water_enter");
                        particle::spawn_fan(this,
                            235.0F, 305.0F, 6,
                            particle::Type::drop_water,
                            position() + Vec2F{ -4.0F, 0.0F },
                            Vec2F{ our_velocity.x * 0.1F, -our_velocity.y * 0.1F }, 1.5F,
                            state::Type::idle);
                    }
                }
                break;
            }
        }
        if (is_to_play_bounce) {
            F32 abs_vel_y = std::abs(our_velocity.y) / 3.0F;
            F32 pitch = 1.0F;
            if (abs_vel_y > 1.0F) {
                if (abs_vel_y > 1.2F) {
                    abs_vel_y = 1.2F;
                }
                pitch = abs_vel_y;
            }
            sound_pitch("bounce", pitch + random::number(-10, 10) / 100.0F);
            sound_position("bounce", { position().x + 2.0F - app::config::extent().x / 2.0F,
                                       position().y + 2.0F - app::config::extent().y / 2.0F });
            sound_play("bounce");
        }
    }
}