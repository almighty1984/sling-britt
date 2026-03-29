module entity.particle.interact;

namespace entity {
    void ParticleInteract::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!m_parent or m_parent == other.owner) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        if (other_type == Type::clip_ledge) {
            if (!m_parent->is_on_ground() or our_rect.h < other_rect.h or our_rect.y > other_rect.y) return;
            m_parent->interact(other.owner);
        }
        else if (other_type == Type::trigger) {
            if (!m_parent->is_on_ground()) return;
            //m_is_to_erase = true;
            //console::log("ParticleInteract::collide_x: ", to_string(other_type), "\n");
            m_parent->interact(other.owner);
        }
        else if (other_type == Type::brick or other_type == Type::bug) {
            m_is_to_erase = true;
            m_parent->interact(other.owner);
        }
    }
    void ParticleInteract::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}