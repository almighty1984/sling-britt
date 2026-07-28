module entity.particle.melee;

namespace entity {
    void ParticleMelee::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or is_hurting() or m_is_to_erase or
            !other.owner or other.owner->is_dead() or
            !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_velocity = velocity();

        switch (other_type) {
            case Type::clip_D:
            case Type::clip_D_50:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (our_velocity.y < 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::clip_U:
            case Type::clip_U_50:
            case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (our_velocity.y > 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::particle_health: {
                collide_x(our, other);
                break;
            }
            case Type::particle_melee: {
                collide_x(our, other);
                break;
            }
            default: {
                collide_x(our, other);
                break;
            }
        }
    }
}