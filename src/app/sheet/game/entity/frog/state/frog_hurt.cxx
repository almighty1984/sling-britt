module entity.frog;

namespace entity {
    void Frog::state_hurt(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("hurt");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_state = state::Type::idle;
        }

        set_anim("hurt");
        line::is_hidden(m_tounge_line, true);

        //console::log(class_name(), "::hurt\n");

        velocity_add_y(acceleration().y);

        //console::log("time left to react: ", (int)m_time_left_to_react, "\n");
        
        
        if (/*anim()->is_last_frame() and*/ m_is_on_ground and m_next_state != m_sensed_state) {
            //console::log("is last frame: ", anim()->is_last_frame(), "\n");

            for (auto& i : m_sensed_objects) {
                if (!i or i->is_dead()) continue;
                if (m_is_on_ground and (i->type() == Type::brick or
                    i->type() == Type::particle_brick or
                    i->type() == Type::particle_melee or
                    i->type() == Type::player or
                    i->type() == Type::bug)
                    ) {
                    m_time_left_to_react = 10;
                    console::log(class_name(), "::hurt() sensed: ", to_string(i->type()), "\n");
                    m_sensed_state = state::Type::melee;
                    m_sensed_position = i->position();
                    m_sensed_objects.clear();
                    break;
                }
            }
            if (m_sensed_state == state::Type::idle) {
                m_time_left_to_react = 1;
            }
        }
        if (m_time_left_to_react > 0) {
            console::log(class_name(), "::hurt() time left to react: ", m_time_left_to_react, "\n");
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = m_sensed_state;
            }
            return;
        }
    }
}