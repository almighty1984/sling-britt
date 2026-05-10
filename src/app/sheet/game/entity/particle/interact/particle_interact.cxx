module entity.particle.interact;

namespace entity {
    void ParticleInteract::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!m_parent or m_parent == other.owner) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        switch (other_type) {
            case Type::clip_ledge: {
                if (m_parent->state() != state::Type::ledge) return;
                //if (!m_parent->is_on_ground() or our_DR.y < other_DR.y or our_UL.y > other_UL.y) return;
                m_parent->interact(other.owner);
                break;
            }
            case Type::trigger: {
                if (!m_parent->is_on_ground()) return;
                //m_is_to_erase = true;
                //console::log("ParticleInteract::collide_x: ", to_string(other_type), "\n");
                m_parent->interact(other.owner);
                break;
            }
            case Type::brick:
            case Type::bug: {
                m_is_to_erase = true;
                m_parent->interact(other.owner);
                break;
            }
        }
    }
    void ParticleInteract::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}