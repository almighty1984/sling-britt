module entity.particle.pebble;
import app.config;
import particle_system;

namespace entity {    
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
            move_velocity({});

            velocity_x(velocity().x * 0.5F);

            if (std::abs(our_velocity.y) >= m_play_bounce_vel) {
                is_to_play_bounce = true;
            }
            break;
        }
        case Type::clip:
        case Type::clip_ledge:
        case Type::clip_ledge_L_50:
        case Type::clip_ledge_R_50:
        case Type::clip_UD: {
            if (our_velocity.y < 0.0F and our_DR.y < other_DR.y) return;
            if (our_velocity.y > 0.0F and our_UL.y > other_UL.y) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.5F);
            if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                velocity_y(0.0F);
            }
            move_velocity({});

            velocity_x(velocity().x * 0.5F);

            if (std::abs(our_velocity.y) >= m_play_bounce_vel) {
                is_to_play_bounce = true;
            }
            break;
        }
        case Type::clip_U:
        case Type::clip_U_50:
        case Type::slope_U: {
            //if (velocity().y > 0.0F and our_UL.y > other_DR.y) return;

            position_add_y(-overlap_y);

            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }

            move_velocity({});

            velocity_x(velocity().x * 0.5F);

            if (our_velocity.y >= m_play_bounce_vel) {
                is_to_play_bounce = true;
            }
            break;
        }
        case Type::clip_D:
        case Type::clip_D_50:
        case Type::clip_LD:
        case Type::clip_RD: {
            if (velocity().y > 0.0F) return;
            //if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.5F);
            move_velocity({});

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
            m_time_left_hurt = m_config.time_to_hurt();
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
                        235.0F, 305.0F, 5,
                        particle::Type::drop_water,
                        position() + Vec2F{ -4.0F, 0.0F },
                        Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 1.0F,
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
            sound_position("bounce", { (position().x + 2.0F) / (app::config::extent().x / 2.0F),
                                       (position().y + 2.0F) / (app::config::extent().y / 2.0F) });
            sound_play("bounce");
        }
    }
}