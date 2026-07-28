module entity.particle.rock;
import particle_system;

namespace entity {
    void ParticleRock::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent == other.owner or m_state == state::Type::dead or m_next_state == state::Type::dead or other.owner->is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        switch (other_type) {
        case Type::arch_L_1x1:
        case Type::arch_R_1x1: {
            position_add_y(-overlap_y);
            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::arch_L_2x1_0:
        case Type::arch_L_2x1_1:
        case Type::arch_R_2x1_0:
        case Type::arch_R_2x1_1: {
            position_add_y(-overlap_y);
            velocity_y(our_velocity.y * -0.75F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::bee: {
            particle::spawn(this, particle::Type::hit, position() - Vec2F{ 4.0F, 4.0F }, {});
            position_add_y(-overlap_y);

            other.owner->velocity_y(our_velocity.y * 0.3F);
            other.owner->hurt(this);

            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::brick: {
            console::log(class_name(), "::collide_y() brick\n");
            particle::spawn(this, particle::Type::hit, position() - Vec2F{ 4.0F, 4.0F }, {});
            position_add_y(-overlap_y);
            //other.owner->velocity_y(our_velocity.y * 0.1F);
            //other.owner->hurt(this);
            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            break;
        }
        case Type::bug: {
            console::log(class_name(), "::collide_y() bug\n");
            particle::spawn(this, particle::Type::hit, position() - Vec2F{ 4.0F, 4.0F }, {});
            position_add_y(-overlap_y);
            other.owner->velocity_y(our_velocity.y * 0.1F);
            hurt(other.owner);
            other.owner->hurt(this);
            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
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

            move_velocity({});

            velocity_x(our_velocity.x * 0.5F);

            hurt(other.owner);
            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::clip_U:
        case Type::clip_U_50:
        case Type::slope_U: {
            if (our_velocity.y < 0.0F) return;
            //if (velocity().y > 0.0F and our_UL.y > other_DR.y) return;


            position_add_y(-overlap_y);

            if (position().y > other_UL.y) {
                position_add_y(other_UL.y - position().y - our_extent.y);
            }

            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }

            move_velocity({});

            velocity_x(velocity().x * 0.5F);

            hurt(other.owner);
            break;
        }
        case Type::clip_D:
        case Type::clip_D_50:
        case Type::clip_LD:
        case Type::clip_RD: {
            if (velocity().y > 0.0F) return;
            //if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;


            position_add_y(-overlap_y);

            move_velocity({});

            velocity_x(our_velocity.x * 0.5F);
            velocity_y(our_velocity.y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::frog: {
            //if (other.owner->is_hurting()) return;

            console::log(class_name(), "::collide_y() frog\n");

            position_add_y(-overlap_y);
            other.owner->hurt(this);
            velocity_y(velocity().y * -0.5F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::mole: {
            if (other.owner->state() == state::Type::idle or std::abs(our_velocity.y) < 2.0F) return;


            position_add_y(-overlap_y);
            other.owner->hurt(this);
            velocity_y(our_velocity.y * -1.0F);
            hurt(other.owner);
            velocity_y(0.0F);
            break;
        }
        case Type::slope_L_1x1: {

            position_add_y(-overlap_y);

            if (position().y > other_UL.y) {
                position_add_y(other_UL.y - position().y - our_extent.y);
            }
            velocity_y(std::abs(our_velocity.y) * -1.0F);
            if (velocity().y > -4.0F) {
                velocity_y(-4.0F);
            }
            sprite_angle(45.0F);

            velocity_x(our_velocity.x * 0.5F);
            hurt(other.owner);
            //velocity_y(0.0F);
            break;
        }
        case Type::slope_R_1x1: {

            position_add_y(-overlap_y);

            if (position().y > other_UL.y) {
                position_add_y(other_UL.y - position().y - our_extent.y);
            }
            velocity_y(std::abs(our_velocity.y) * -1.0F);
            if (velocity().y > -4.0F) {
                velocity_y(-4.0F);
            }
            sprite_angle(135.0F);

            velocity_x(our_velocity.x * 0.5F);
            hurt(other.owner);
            //velocity_y(0.0F);
            break;
        }
        case Type::slope_L_2x1_0:
        case Type::slope_L_2x1_1: {

            position_add_y(-overlap_y);

            if (position().y > other_UL.y) {
                position_add_y(other_UL.y - position().y - our_extent.y);
            }

            velocity_y(std::abs(our_velocity.y) * -1.0F);
            if (velocity().y > -4.0F) {
                velocity_y(-4.0F);
            }

            sprite_angle(67.5F);

            velocity_x(our_velocity.x * 0.75F);

            hurt(other.owner);
            //velocity_y(0.0F);
            break;
        }
        case Type::slope_R_2x1_0:
        case Type::slope_R_2x1_1: {

            position_add_y(-overlap_y);
            if (position().y > other_UL.y) {
                position_add_y(other_UL.y - position().y - our_extent.y);
            }

            velocity_y(std::abs(our_velocity.y) * -1.0F);
            if (velocity().y > -4.0F) {
                velocity_y(-4.0F);
            }

            sprite_angle(112.5F);

            velocity_x(our_velocity.x * 0.75F);

            hurt(other.owner);
            //velocity_y(0.0F);
            break;
        }
        case Type::player: {
            if (m_state == state::Type::dead or std::abs(other_velocity.x) < 2.0F) return;
            other.owner->hurt(this);
            hurt(other.owner);

            position_add({ 0.0F, -2.0F });
            velocity_x(other_velocity.x * 1.2F);
            velocity_y(other_velocity.y - 1.0F);
            break;
        }
        case Type::train_platform: {
            break;
        }
        case Type::water_line_L:
        case Type::water_line_R:
        case Type::water_line: {
            if (acceleration() == Vec2F{ 0.05F, 0.05F }) return;

            acceleration({ 0.05F, 0.05F });
            max_velocity({ 0.4F, 0.7F });

            if (our_velocity.y <= 0.0F) return;
            if (!sound_is_playing("water_enter")) {
                sound_position("water_enter", { position().x - app::config::extent().x / 2.0F,
                                                position().y - app::config::extent().y / 2.0F });
                sound_play("water_enter");
                particle::spawn_fan(this,
                    235.0F, 305.0F, 3,
                    particle::Type::drop_water,
                    position() + Vec2F{ -4.0F, 0.0F },
                    Vec2F{ our_velocity.x * 0.1F, -our_velocity.y * 0.1F }, 1.5F);
            }
            break;
        }
        }
    }
}