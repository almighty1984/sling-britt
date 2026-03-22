module entity.brick;
import particle_system;
import aabb;

namespace entity {
    void Brick::state_carried() {
        if (!m_parent) {
            m_next_state = entity::State::idle;
            return;
        }
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        m_is_on_ground = false;

        //console::log("entity::Brick::carried is near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
        //console::log("entity::Brick::carried ", velocity().x, " ", velocity().y, "\n");

        velocity(m_parent->velocity());
        moved_velocity({});
        cF32 carry_offset_x = 10.0F;
        if (sprite::is_leftward(m_parent->sprite_id())) {
            if (position().x < m_parent->position().x - carry_offset_x) {
                position_x(m_parent->position().x - carry_offset_x);
            }
            if (position().x > m_parent->position().x - carry_offset_x) {
                cF32 diff_x = (m_parent->position().x - carry_offset_x) - position().x;
                add_velocity_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x - carry_offset_x);
            }
        } else {
            if (position().x > m_parent->position().x + carry_offset_x) {
                position_x(m_parent->position().x + carry_offset_x);
            }

            if (position().x < m_parent->position().x + carry_offset_x) {
                cF32 diff_x = (m_parent->position().x + carry_offset_x) - position().x;
                add_velocity_x(diff_x / 4.0F);
            } else {
                position_x(m_parent->position().x + carry_offset_x);
            }
        }

        sprite::is_leftward(m_sprite_id, position().x + 16.0F < m_parent->position().x + 8.0F);

        position_y(m_parent->position().y + 4.0F);
        /*if (m_parent->is_ducking()) {
            add_position_y(4.0F);
        }*/
        if (m_parent->is_ducking() || !m_parent->is_carrying()) {
            m_parent->is_carrying(false);
            velocity({ m_parent->velocity().x, velocity().y });
            if (sprite::is_leftward(m_sprite_id)) {
                add_velocity_x(-0.5F);
                add_position_x(-8.0F);
            } else {
                add_velocity_x(0.5F);
                add_position_x(8.0F);
            }
            velocity_y(-1.0F);
            m_parent = nullptr;
            m_is_first_state_update = true;
            m_next_state = entity::State::idle;
        }
    }
    void Brick::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            m_time_left_alive = 0;
            console::log("entity::Brick::state_dead()\n");
            if (m_parent) {
                m_parent->is_carrying(false);
                m_parent = nullptr;
            }
            reset_anim("dead");

            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            sprite_is_hidden(true);
            sound_position("dead", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("dead");
            particle::spawn(this, particle::Type::hit, position(), {});

            console::log("entity::Brick::dead velocity.x ", velocity().x, "\n");

            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::brick,
                                position() + Vec2F{ 6.0F, -4.0F },
                                velocity() / 4.0F, 2.0F,
                                entity::State::idle);
        }
        velocity({});
        moved_velocity({});

        set_anim("dead");

        //console::log("entity::Brick::state_dead() start position: ", start_position().x, " ", start_position().y, "\n");

        if (m_time_left_dead > 0 && m_time_to_be_dead != U16_MAX) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log("entity::Brick::dead done being dead\n");
                acceleration(start_acceleration());
                max_velocity(start_max_velocity());
            }
        }
    }
    void Brick::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_alive = U16_MAX;

            m_parent = nullptr;

            sprite::is_hidden(m_sprite_id, false);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
        }
        if (m_is_on_ground) {
            deceleration({ 0.2F, 0.0F });
        }
        ++m_time_in_state;
    }
    void Brick::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        acceleration({ 0.05F, 0.05F });
        max_velocity({ 1.0F, 1.0F });
    }
    void Brick::state_tossed() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;

            deceleration({ 0.0F, 0.0F });
        }
        if (m_is_on_ground) {
            deceleration({ 0.1F, 0.0F });
            m_next_state = entity::State::idle;
        } else {
            //console::log("entity::Brick toss not on ground\n");
        }
    }
}