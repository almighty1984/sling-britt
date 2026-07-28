module entity.particle.rock;

namespace entity {
    void ParticleRock::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent == other.owner) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? (our_DR.x - other_UL.x) : -(other_DR.x - our_UL.x);

        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        //console::log(class_name(), "::collide_x() ", to_string(other_type), "\n");

        switch (other_type) {
        case Type::arch_L_1x1:
        case Type::arch_R_1x1: {
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::arch_L_2x1_0:
        case Type::arch_L_2x1_1:
        case Type::arch_R_2x1_0:
        case Type::arch_R_2x1_1: {
            velocity_x(our_velocity.x * -0.75F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::bee: {
            position_add_x(-overlap_x);
            other.owner->velocity_x(our_velocity.x * 0.3F);
            other.owner->hurt(this);
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::bee_hive: {
            position_add_x(-overlap_x);            
            other.owner->hurt(this);
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::brick: {
            position_add_x(-overlap_x);
            //other.owner->velocity_x(our_velocity.x * 0.3F);
            //other.owner->hurt(this);
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            //console::log(class_name(), "::collide_x() brick ", other.owner->velocity().x, "\n");
            break;
        }
        case Type::bug: {
            //console::log(class_name(), "::collide_x() bug\n");

            position_add_x(-overlap_x);

            other.owner->velocity_x(our_velocity.x * 0.1F);
            other.owner->hurt(this);
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            break;
        }
        case Type::clip:
        case Type::clip_L:
        case Type::clip_L_50:
        case Type::clip_LD:
        case Type::clip_R:
        case Type::clip_R_50:
        case Type::clip_RD:
        case Type::clip_ledge:
        case Type::clip_ledge_L_50:
        case Type::clip_ledge_R_50:
        case Type::clip_LR: {
            if (((other_type == Type::clip_L or other_type == Type::clip_L_50) and our_velocity.x < 0.0F) or
                ((other_type == Type::clip_R or other_type == Type::clip_R_50) and our_velocity.x > 0.0F)) {
                return;
            }
            if (other_UL.x < our_UL.x and our_velocity.x > 0.0F or
                other_DR.x > our_DR.x and our_velocity.x < 0.0F) return;

            console::log(class_name(), "::collide_x() ", to_string(other_type), " overlap_x: ", -overlap_x, "\n");
            position_add_x(-overlap_x);            
            velocity_x(velocity().x * -0.5F);
            hurt(other.owner);
            break;
        }
        case Type::frog: {
            //if (other.owner->is_hurting()) return;

            position_add_x(-overlap_x);
            other.owner->hurt(this);
            velocity_x(our_velocity.x * -1.0F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::mole: {
            if (other.owner->state() == state::Type::idle) {
                return;
            }

            position_add_x(-overlap_x);
            other.owner->hurt(this);
            velocity_x(our_velocity.x * -0.5F);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::slope_L_1x1:
        case Type::slope_R_1x1:
        case Type::slope_L_2x1_0:
        case Type::slope_L_2x1_1:
        case Type::slope_R_2x1_0:
        case Type::slope_R_2x1_1: {
            position_add_x(-overlap_x);
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
        case Type::player: {
            if (std::abs(velocity().x) < 2.0F or m_state == state::Type::dead) {
                return;
            }

            //position_add_x( -overlap_x );
            //position().y -= 2.0F;
            velocity_y(other_velocity.y - 2.0F);

            velocity_x(other_velocity.x * 1.2F);
            //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

            //other.owner->velocity().x *= 0.95F;
            other.owner->hurt(this);
            hurt(other.owner);
            velocity_x(0.0F);
            break;
        }
        case Type::train_platform: {
            break;
        }
        case Type::water_line_L:
        case Type::water_line_R:
        case Type::water_line: {
            collide_y(our, other);
            break;
        }
        }
    }
}