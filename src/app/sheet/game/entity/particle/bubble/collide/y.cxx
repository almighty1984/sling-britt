module entity.particle.bubble;

namespace entity {
    void ParticleBubble::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        switch (other_type) {
        case Type::arch_L_1x1:
        case Type::arch_L_2x1_0:
        case Type::arch_L_2x1_1:
        case Type::arch_R_1x1:
        case Type::arch_R_2x1_0:
        case Type::arch_R_2x1_1: {
            if (velocity().y > 0.0F) return;
            F32 amount = 0.0F;
            switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1: amount = 1.0F; break;
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: amount = -1.0F; break;
            }
            velocity_x(velocity().y * amount);
            position_add_y(-overlap_y);

            direction_x(0.0F);
            break;
        }
        case Type::clip:
        case Type::clip_U:
        case Type::clip_U_50:
        case Type::clip_D:
        case Type::clip_D_50:
        case Type::clip_L:
        case Type::clip_L_50:
        case Type::clip_R:
        case Type::clip_R_50:
        case Type::clip_LD:
        case Type::clip_RD:
        case Type::clip_ledge:
        case Type::clip_ledge_L_50:
        case Type::clip_ledge_R_50: {
            position_add_y(-overlap_y);

            m_time_in_state = 0;

            direction_x(0.0F);
            //if (our_UL.x < other_UL.x + (other_DR.x - other_UL.x) / 2) {

            /*if (velocity().y < 0.0F) {
                velocity_x(velocity().y);
                velocity_y(0.0F);
            }*/
            break;
        }
        case Type::water_line_L:
        case Type::water_line_R:
        case Type::water_line: {
            m_time_left_alive = 0;
            m_time_left_dead = m_config.time_to_be_dead();
            other.owner->velocity_add_y(our.owner->velocity().y / 4.0F);
            break;
        }
        }
    }
}