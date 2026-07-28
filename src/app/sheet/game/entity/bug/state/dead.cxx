module entity.bug;

namespace entity {
    void Bug::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_config.time_to_be_dead();
            reset_anim("dead");
            //console::log("prev state: ", entity::to_string(m_prev_state), "\n");
            if (m_prev_state == state::Type::upended or m_prev_state == state::Type::bounce) {
                sprite::is_upended(m_sprite, true);
            }
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this,
                                0.0F, 360.0F, 8,
                                particle::Type::drop_blood,
                                position() - Vec2F{ 16.0F, 8.0F },
                                velocity(), 3.0F,
                                state::Type::idle);

            //particle::spawn({ this, particle::Type::health, position() , {} });
            particle::spawn(this, particle::Type::health, position(), {});

            sound_position("dead", { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
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