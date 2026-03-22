module entity.particle.health;

namespace entity {
    void ParticleHealth::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() || m_is_to_erase || other.owner->is_dead() /* || other.owner == m_parent*/) return;

        cType other_type = other.owner->type();

        if (other_type == Type::frog) {
            if (health::is_full(other.owner->health_id())) return;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            sound_position("dead", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("dead");
        } else if (other_type == Type::player) {
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            health::add_amount(other.owner->health_id(), 1.0F);

            sound_position("dead", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("dead");
        } else if (other_type == Type::particle_melee) {
            if (!other.owner->parent()) return;
            if (other.owner->parent()->type() == Type::frog) {
                health::add_amount(other.owner->parent()->health_id(), 1.0F);
                m_time_left_alive = 7;
                m_time_left_dead = m_time_to_be_dead;
            }
        }
    }
    void ParticleHealth::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}