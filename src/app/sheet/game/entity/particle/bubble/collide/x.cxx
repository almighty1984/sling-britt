module entity.particle.bubble;

namespace entity {
    void ParticleBubble::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead()) return;

        //collide_y(our, other);
        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);


        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        switch (other_type) {
            case Type::clip:
            case Type::clip_L:
            case Type::clip_L_50:
            case Type::clip_R:
            case Type::clip_R_50:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50: {
                if (((other_type == Type::clip_L or other_type == Type::clip_L_50 or other_type == Type::clip_LD) and velocity().x < 0.0F) or
                    ((other_type == Type::clip_R or other_type == Type::clip_R_50 or other_type == Type::clip_RD) and velocity().x > 0.0F)) {
                    return;
                }
                m_time_in_state = 0;
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                direction_x(0.0F);
                break;
            }
            case Type::particle_bubble: {
                position_add_x(-overlap_x);
                //cF32 vel_x = velocity().x;
                //velocity_x((other.owner->velocity().x + vel_x) / 2.0F;
                //other.owner->velocity_x((other.owner->velocity().x + vel_x) / 2.0F;
                if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                    other.owner->velocity_x(velocity().x);
                } else {
                    velocity_x(other.owner->velocity().x);
                }
                direction_x(0.0F);
                break;
            }
        }
    }
}