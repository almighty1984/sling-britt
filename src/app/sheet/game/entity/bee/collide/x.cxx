module entity.bee;
import console;
import aabb;

namespace entity {
    void Bee::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner == m_parent or is_dead() or other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        Vec2F our_extent = { aabb::UR(our.id).x - aabb::UL(our.id).x,
                             aabb::DL(our.id).y - aabb::UL(our.id).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F our_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();

        state::cType other_state = other.owner->state();

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1:
            case Type::brick:            
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50:
            case Type::clip_L:
            case Type::clip_L_50:
            case Type::clip_LD:
            case Type::clip_R:
            case Type::clip_R_50:
            case Type::clip_RD: {
                break;
            }
            case Type::bug:
            case Type::frog:
            case Type::mole: {
                if (other_state == state::Type::swim) return;
                if (other_type == Type::mole and other_state == state::Type::idle) return;
                if (m_state == state::Type::attack) {
                    position_add_x(-overlap_x);
                    velocity_x(-our_velocity.x * 0.75F);
                    other.owner->velocity_x(our_velocity.x);
                    other.owner->hurt(this);
                    m_next_state = state::Type::idle;
                    return;
                }
                break;
            }
            case Type::player: {
                if (other.owner->state() == state::Type::sling) {
                    hurt(other.owner);        
                    return;
                }
                if (m_state == state::Type::attack) {
                    position_add_x(-overlap_x);
                    velocity_x(-our_velocity.x * 0.75F);
                    other.owner->velocity_x(our_velocity.x);
                    other.owner->hurt(this);
                    m_next_state = state::Type::idle;
                    return;
                }
                break;
            }
            case Type::slope_L_1x1:
            case Type::slope_R_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                collide_y(our, other);
                break;
            }
            case Type::spring_U:
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
    }
}