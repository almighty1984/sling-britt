module entity.bee;
import random;

namespace entity {
    void Bee::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            
            sound_stop("idle");
            m_time_left_dead = m_config.time_to_be_dead() + random::number(0, 100);
            sprite_is_hidden(true);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            Vec2F blood_vel = velocity() + move_velocity();
            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                blood_vel * 1.0F, 1.0F,
                                state::Type::idle);
            console::log(class_name(), "::state_dead() blood vel: ", blood_vel.x, " ", blood_vel.y, "\n");

            particle::spawn(this,
                            particle::Type::health,
                            position() + Vec2F{ 4.0F, 0.0F }, {});

            sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });
            sound_play("dead");

            m_sensed.clear();
        }

        velocity(velocity() * 0.9F);
        move_velocity({});

        if (m_config.time_to_be_dead() == 0 or m_config.time_to_be_dead() == U16_MAX) {
            return;
        }

        if (!m_parent or m_parent->is_dead()) return;

        if (m_time_left_dead > 0) {
            --m_time_left_dead;
        }
        //if (!is_start_in_view()) {
            if (m_time_left_dead == 0) {
                console::log(class_name(), "::state_dead() done being dead\n");

                m_next_state = m_start_state;
                position(start_position());
            }
        //}
    }
}