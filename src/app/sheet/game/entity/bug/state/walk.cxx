module entity.bug;

namespace entity {
    void Bug::state_walk(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            acceleration(start_acceleration());
            deceleration(start_deceleration());

            sprite_is_hidden(false);
            sprite_is_upended(false);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
            if (m_prev_state == state::Type::dead) {
                health_reset();
            }
            if (m_prev_state == state::Type::none or m_prev_state == state::Type::dead) {
                sprite_is_leftward(random::number(0, 1) ? true : false);
            }
            sprite_angle(0.0F);
        }
        ++m_time_in_state;
        m_parent = nullptr;
        deceleration({});
        if (m_is_on_ground) {
            //console::log("entity::Bug walk on ground\n");
            move_velocity({});
            velocity({ sprite_is_leftward() ? -0.5F : 0.5F, velocity().y });
        } else {
            //console::log("entity::Bug walk not on ground\n");
        }
        m_current_anim = anim("idle");
    }
}