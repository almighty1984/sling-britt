module entity.mole;

namespace entity {
    void Mole::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_config.time_to_be_dead();
            reset_anim("dead");
            sprite_is_hidden(true);
            health_is_hidden(true);

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            cVec2F blood_vel = velocity() + move_velocity();

            console::log(class_name(), "::state_dead() blood vel: ", blood_vel.x, " ", blood_vel.y, "\n");

            particle::spawn_fan(this, 0.0F, 360.0F, 10,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                blood_vel, 3.0F,
                                state::Type::idle);

            particle::spawn_fan(this, 0.0F, 360.0F, 7,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                blood_vel, 2.7F,
                                state::Type::idle);

            particle::spawn(this, particle::Type::health, position() + Vec2F{ 4.0F, 0.0F }, {});

            sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });
            sound_play("dead");

            if (m_parent) {
                m_parent->is_carrying(false);
                m_parent = nullptr;
            }
        }        
        velocity({});
        move_velocity({});

        if (m_config.time_to_be_dead() == 0 or m_config.time_to_be_dead() == U16_MAX) {
            return;
        }
        if (m_time_left_dead > 0) {
            --m_time_left_dead;
        }
        return;
        if (!is_start_in_view()) {
            if (m_time_left_dead == 0) {
                console::log(class_name(), "::state_dead() done being dead\n");

                m_next_state = m_start_state;
                position(start_position());
            }
        }
    }
}