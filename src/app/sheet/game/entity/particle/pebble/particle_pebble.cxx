module entity.particle.pebble;
import app.config;
import particle_system;

namespace entity {
    void ParticlePebble::collide_x(aabb::cInfo our, aabb::cInfo other) {
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
                velocity_x(velocity().x * 0.5F);
                break;
            }
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                velocity_x(velocity().x * -0.75F);
                break;
            }
            case Type::brick:
            case Type::bug: {
                //console::log(class_name(), "::collide_x ", to_string(other_type), "\n");
                if (is_hurting() or (std::abs(other_velocity.x) < 0.5F and std::abs(our_velocity.x) < 0.5F)) return;

                m_time_left_hurt = m_time_to_hurt;
                m_time_left_dead = m_time_to_be_dead;

                velocity_y(other_velocity.y - 2.0F);
                velocity_x(other_velocity.x * 1.2F);
                break;
            }
            case Type::clip:
            case Type::clip_L:
            case Type::clip_R:
            case Type::clip_ledge:
            case Type::clip_LR: {
                if (other_type == Type::clip_L and velocity().x < 0.0F or
                    other_type == Type::clip_R and velocity().x > 0.0F) return;

                if (other_UL.x < our_UL.x and velocity().x > 0.0F or
                    other_DR.x > our_DR.x and velocity().x < 0.0F) return;
                position_add_x(-overlap_x);
                velocity_x(velocity().x * -0.75F);

                if (std::abs(our_velocity.x) >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip_U:
            case Type::slope_U: {
                collide_y(our, other);
                break;
            }
            case Type::frog: {
                if (std::abs(velocity().x) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    sound_position("melee", { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
                    sound_play("melee");

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                }
                position_add_x(-overlap_x);
                velocity_x(velocity().x * -1.0F);
                break;
            }
            case Type::mole: {
                if (is_hurting() or other.owner->is_hurting() or other.owner->state() == state::Type::swim or (velocity().x >= -1.0F and velocity().x <= 1.0F)) return;

                if (other.owner->state() == state::Type::idle) {
                    m_time_left_hurt = m_time_to_hurt;
                    velocity_y(other_velocity.y - 1.0F);
                    velocity_x(other_velocity.x * 0.5F);
                    return;
                }

                position_add_x(-overlap_x);
                velocity_x(velocity().x * -1.0F);

                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                break;
            }
            case Type::slope_L_1x1:
            case Type::slope_R_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                //position_add_x(-overlap_x);
                collide_y(our, other);
                break;
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
            case Type::particle_pebble: {
                //position_add_x(-overlap_x);
                //velocity_x(other_velocity.x);
                //m_time_left_dead = 100;
                break;
            }
            case Type::player: {
                if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;

                m_time_left_hurt = m_time_to_hurt;
                m_time_left_dead = m_time_to_be_dead;

                //position_add_x( -overlap_x );
                //position().y -= 2.0F;
                velocity_y(other_velocity.y - 2.0F);

                velocity_x(other_velocity.x * 1.2F);
                //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

                //other.owner->velocity().x *= 0.95F;
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
            sound_position("bounce", { (position().x + 2.0F) / (app::config::extent().x / 2.0F),
                                       (position().y + 2.0F) / (app::config::extent().y / 2.0F) });
            sound_play("bounce");
        }
    }
    void ParticlePebble::collide_y(aabb::cInfo our, aabb::cInfo other) {
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
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
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
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (std::abs(our_velocity.y) >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_UD: {
                if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;
                if (velocity().y > 0.0F and our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (std::abs(our_velocity.y) >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip_U:
            case Type::slope_U: {
                //if (velocity().y > 0.0F and our_UL.y > other_DR.y) return;

                position_add_y(-overlap_y);

                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }

                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (velocity().y > 0.0F) return;
                //if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y <= -m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::frog: {
                if (std::abs(velocity().y) >= 2.0F) {
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                }
                if (our_UL.y < other_UL.y) {
                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);
                }
                break;
            }
            case Type::mole: {
                if (other.owner->state() == state::Type::idle or (velocity().y >= -2.0F and velocity().y <= 2.0F)) return;
                if (our_UL.y < other_UL.y) {
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");
                    particle::spawn(this, particle::Type::hit, hit_pos, {});

                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);
                }
                break;
            }
            case Type::slope_L_1x1: {
                position_add_y(-overlap_y);
                velocity_y(std::abs(velocity().y) * -0.5F);
                if (std::abs(velocity().y) < 0.5F) {
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
                velocity_y(std::abs(velocity().y) * -0.5F);
                if (std::abs(velocity().y) < 0.5F) {
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
                velocity_y(std::abs(velocity().y) * -0.5F);
                if (std::abs(velocity().y) < 0.5F) {
                    velocity_y(0.0F);
                }
                sprite_angle(67.5F);
                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                position_add_y(-overlap_y);
                velocity_y(std::abs(velocity().y) * -0.5F);
                if (std::abs(velocity().y) < 0.5F) {
                    velocity_y(0.0F);
                }
                sprite_angle(112.5F);
                velocity_x(velocity().x * 0.5F);

                if (our_velocity.y >= m_play_bounce_vel) {
                    is_to_play_bounce = true;
                }
            }
            case Type::player: {
                if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;
                m_time_left_hurt = m_time_to_hurt;
                position_add({ 0.0F, -2.0F });
                velocity_x(other_velocity.x * 1.2F);
                velocity_y(other_velocity.y - 1.0F);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (acceleration() == Vec2F{ 0.05F, 0.05F }) return;

                acceleration({ 0.05F, 0.05F });
                max_velocity({ 0.4F, 0.7F });

                if (velocity().y > 0.0F) {
                    if (!sound_is_playing("water_enter")) {
                        sound_position("water_enter", { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
                        sound_play("water_enter");
                        particle::spawn_fan(this,
                            235.0F, 305.0F, 3,
                            particle::Type::drop_water,
                            position() + Vec2F{ -4.0F, 0.0F },
                            Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 1.5F);
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
            sound_position("bounce", { (position().x + 2.0F) / (app::config::extent().x / 2.0F),
                                       (position().y + 2.0F) / (app::config::extent().y / 2.0F) });
            sound_play("bounce");
        }
    }
}