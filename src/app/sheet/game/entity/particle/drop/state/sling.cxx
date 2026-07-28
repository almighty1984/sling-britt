module entity.particle.drop;

namespace entity {
    void ParticleDrop::state_sling(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            //m_time_left_until_next_state = m_time_left_in_next_state;
            //m_time_left_until_next_state = 0;
        }

        if (m_parent_aabb != -1) {
            cVec2F aabb_pos = aabb::UL(m_parent_aabb);

            //console::log("aabb pos: ", aabb_pos.x, " ", aabb_pos.y, "\n");
        }

        velocity({});

        position(aabb::UL(m_parent_aabb) + Vec2F{ 2.0F, 2.0F });


        if (!m_is_on_ground and m_config.weight() < 1.0F) {
            //console::log(class_name(), "::state_sling() ", m_time_left_until_next_state, "\n");
            if (m_time_left_until_next_state == 1) {
                m_time_left_alive = m_config.time_to_be_alive();
                m_next_state = state::Type::idle;

                if (m_parent) {
                    time_left_colliding_with(m_parent->type(), 50);
                }
            }
            return;
        }

    }
}