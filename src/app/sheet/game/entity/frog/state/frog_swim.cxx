module entity.frog;

namespace entity {
    void Frog::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
            //reset_anim(m_anims["swim"]);

            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body_swim) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
        }
        line::is_hidden(m_tounge_line, true);
        set_anim("hurt");

        deceleration({ 0.0F, 0.0F });

        if (position_on_level().y + 16 < m_water_line_y) {
            console::log("frog swim water line y: ", m_water_line_y, "\n");
            //transform()->max_velocity = transform()->start_max_velocity;
            max_velocity({ 0.0F, 4.0F });
        } else {
            max_velocity({ 0.0F, 2.0F });
            velocity_x(sprite::is_leftward(m_sprite) ? -0.8F : 0.8F);
        }

        velocity_y(velocity().y * 0.9F);
        //moved_velocity({});

        if (position_on_level().y > m_water_line_y) {
            velocity_add({ 0.0F, -0.3F });
        } else {
            velocity_add({ 0.0F, 0.3F });
        }
    }
}