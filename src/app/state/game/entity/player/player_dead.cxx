module entity.player;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_down_thrusting = false;
            m_is_carrying = false;
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            sprite::is_hidden(m_sprite_id, true);

            sprite::offset(m_sprite_id, sprite::start_offset(m_sprite_id));
            sprite::angle(m_sprite_id, 0.0F);

            m_state = m_next_state = state::Type::dead;
            //reset_anim(anim_id("dead"));
            particle::spawn_fan(this, 0.0F, 360.0F, 8, particle::Type::drop_blood, position() + Vec2F{ 0.0F, -8.0F }, velocity(), 3.0F);
            particle::spawn(this, particle::Type::health, position() + Vec2F{ 4.0F, 4.0F }, {});
            m_time_left_alive = 0;
            m_time_left_dead = 200;

            console::log("\n\nentity::Player::state_dead()\n\n");

            if (sound_is_playing("hover")) {
                sound_stop("hover");
            }
        }

        velocity({});
        moved_velocity({});

        --m_time_left_dead;
        if (m_time_left_dead == 0) {
            m_next_state = state::Type::run;
            health::reset(m_health_id);
        }
    }
}