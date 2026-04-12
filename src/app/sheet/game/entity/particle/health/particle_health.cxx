module entity.particle.health;
import app.config;

namespace entity {
    void ParticleHealth::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or other.owner->is_dead() /* or other.owner == m_parent*/) return;

        cType other_type = other.owner->type();

        if (other_type == Type::frog) {
            if (health::is_max(other.owner->health_id())) return;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            sound_position("dead", { (position().x + 8.0F) / (app::config::extent().x / 2.0F),
                                     (position().y + 8.0F) / (app::config::extent().y / 2.0F) });
            sound_play("dead");
        }
        else if (other_type == Type::mole) {
            console::log(class_name(), "::collide_x() mole\n");
            if (other.owner->state() != state::Type::enter || health::is_max(other.owner->health_id())) return;

            health::amount_add(other.owner->health_id(), 16.0F);

            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;
            sound_position("dead", { (position().x + 8.0F) / (app::config::extent().x / 2.0F),
                                     (position().y + 8.0F) / (app::config::extent().y / 2.0F) });
            sound_play("dead");
        }
        else if (other_type == Type::player) {
            if (health::is_max(other.owner->health_id())) return;

            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            health::amount_add(other.owner->health_id(), 1.0F);

            sound_position("dead", { position().x / (app::config::extent().x / 2.0F),
                                     position().y / (app::config::extent().y / 2.0F) } );
            sound_play("dead");
        }
        else if (other_type == Type::particle_melee) {
            if (!other.owner->parent()) return;
            if (other.owner->parent()->type() == Type::frog) {
                health::amount_add(other.owner->parent()->health_id(), 1.0F);
                m_time_left_alive = 7;
                m_time_left_dead = m_time_to_be_dead;
            }
        }
    }
    void ParticleHealth::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}