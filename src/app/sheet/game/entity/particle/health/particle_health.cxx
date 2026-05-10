module entity.particle.health;
import app.config;

namespace entity {
    void ParticleHealth::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;
        }
        //console::log(class_name(), "::state_dead() time left dead: ", m_time_left_dead, "\n");
        velocity({});
    }
    void ParticleHealth::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or other.owner->is_dead() /* or other.owner == m_parent*/) return;

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        bool is_to_play_sound = false;

        switch (other.owner->type()) {
            case Type::frog: {
                if (health::is_max(other.owner->health_id())) return;
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;
                break;
            }
            case Type::mole: {
                console::log(class_name(), "::collide_x() mole\n");
                if (other.owner->state() != state::Type::enter || health::is_max(other.owner->health_id())) return;

                health::amount_add(other.owner->health_id(), 16.0F);

                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;
                is_to_play_sound = true;
                break;
            }
            case Type::player: {
                //console::log("layer: ", sprite_layer(), "\n");
                if (health::is_max(other.owner->health_id())) return;

                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;

                health::amount_add(other.owner->health_id(), 64.0F);

                is_to_play_sound = true;
                break;
            }
            case Type::particle_melee: {
                if (!other.owner->parent()) return;
                if (other.owner->parent()->type() == Type::frog) {
                    //health::amount_add(other.owner->parent()->health_id(), 1.0F);
                    velocity(-other_velocity / 2.0F);
                    m_time_left_alive = 6;
                    m_time_left_dead = m_time_to_be_dead;
                    is_to_play_sound = true;
                }
                break;
            }
        }
        if (is_to_play_sound) {
            sound_position("dead", { (position().x + 8.0F) / (app::config::extent().x / 2.0F),
                                         (position().y + 8.0F) / (app::config::extent().y / 2.0F) });
            sound_play("dead");
        }
    }
    void ParticleHealth::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}