module entity.particle.sense;

namespace entity {
    void ParticleSense::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!m_parent or other.owner->is_dead() or m_parent == other.owner or m_parent->is_blocked()) return;
        cVec2F other_velocity = other.owner->velocity();

        if (other_velocity.y >= 5.0F) {
            //console::log("too fast!\n");
            return;
        }
        cType other_type = other.owner->type();

        /* if (m_parent->type() == Type::mole) {
             console::log("type: ", to_string(m_parent->type()), "\n");
         }*/

        if (is_arch(other_type) or
            is_clip(other_type) or
            is_slope(other_type)) {
            m_is_to_erase = true;
            //console::log("ParticleSense::collide_x type: ", to_string(other.owner->type()), "\n");
        }
        else if (other_type == Type::brick or
                 other_type == Type::bug   or
                 other_type == Type::frog  or
                 other_type == Type::water_line
            ) {
            //console::log("vel: ", other.owner->velocity().x, " ", other.owner->velocity().y, "\n");
            m_parent->add_sensed(other.owner);
            m_is_to_erase = true;
        }
        else if (other_type == Type::particle_brick) {
            if (!(other_velocity.x > -0.2F and other_velocity.x < 0.2F and other_velocity.y > -2.0F and other_velocity.y < 2.0F)) {
                m_parent->add_sensed(other.owner);
                m_is_to_erase = true;
            }
        }
        else if (other_type == Type::particle_health) {
            m_parent->add_sensed(other.owner);
            m_is_to_erase = true;
        }
        else if (other_type == Type::particle_shot) {
            m_parent->add_sensed(other.owner);
            m_is_to_erase = true;
        }
        else if (other_type == Type::player) {
            m_parent->add_sensed(other.owner);
            m_is_to_erase = true;
        }
        else if (other_type == Type::particle_down_thrust) {
            m_is_to_erase = true;
        }
        else if (other_type == Type::particle_sense) {

        }
    }
    void ParticleSense::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}