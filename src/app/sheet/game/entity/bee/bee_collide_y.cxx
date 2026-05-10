module entity.bee;
import app.config;
import console;
import aabb;
import particle_system;

namespace entity {
    void Bee::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if ((m_parent and other.owner == m_parent) or is_dead() or other.owner->is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();


        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1:
            case Type::brick:
            case Type::bug:
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_U:
            case Type::slope_U:
            case Type::frog:
                break;
            case Type::player: {
                if (m_state == state::Type::attack) {
                    position_add_y(-overlap_y);
                    velocity_y(-our_velocity.y * 0.75F);
                    other.owner->velocity_y(our_velocity.y);
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
            case Type::slope_R_2x1_1:
            case Type::spring_U:
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line:
                break;
        }
    }
}