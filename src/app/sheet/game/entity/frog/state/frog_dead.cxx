module entity.frog;

namespace entity {
    void Frog::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");
            line::is_hidden(m_tounge_line, true);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            /*particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 4.0F, 0.0F },
                                velocity(), 3.0F);*/

            particle::spawn_fan(this, 170.0F, 10.0F, 10,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                velocity() + moved_velocity(), 3.0F,
                                state::Type::idle);

            particle::spawn_fan(this, 170.0F, 10.0F, 7,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                velocity() + moved_velocity(), 2.7F,
                                state::Type::idle);

            particle::spawn(this,
                            particle::Type::health,
                            position() + Vec2F{ 4.0F, 0.0F }, {});

            sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });
            sound_play("dead");

            //m_next_state = state::Type::none;
            //m_sensed_state = state::Type::none;
            m_sensed_objects.clear();
        }

        //console::log("Frog::dead() next state: ", to_string(m_next_state), "\n");

        velocity(velocity() * 0.9F);

        //velocity().x *= 0.9F;
        //velocity().y *= 0.9F;
        moved_velocity({});

        if (m_time_left_dead > 0) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log("Frog::dead() done being dead\n");
            }
        }
    }
}