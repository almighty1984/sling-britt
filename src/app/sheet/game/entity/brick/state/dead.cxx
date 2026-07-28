module entity.brick;

namespace entity {
    void Brick::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::brick,
                                position() + Vec2F{ 6.0F, -4.0F },
                                velocity() * 1.0F, 1.0F,
                                state::Type::idle);

            m_time_left_dead = m_config.time_to_be_dead();
            m_time_left_alive = 0;
            console::log(class_name(), "::state_dead()\n");
            if (m_parent) {
                m_parent->is_carrying(false);
                //m_parent = nullptr;
            }
            reset_anim("dead");

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sprite_is_hidden(true);
            //particle::spawn(this, particle::Type::hit, position(), {});

            console::log(class_name(), "::dead velocity.x ", velocity().x, "\n");
        }
        velocity({});
        move_velocity({});
        deceleration(start_deceleration());

        set_anim("dead");

        //console::log(class_name(), "::state_dead() start position: ", start_position().x, " ", start_position().y, "\n");

        
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