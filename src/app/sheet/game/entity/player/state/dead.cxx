module entity.player;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_down_thrusting = false;
            m_is_carrying = false;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sprite_is_hidden(true);
            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);
            sprite::is_hidden(m_target_sprite, true);

            sprite_offset(sprite_start_offset());
            sprite_angle(0.0F);

            sprite_is_upended(false);

            m_state = m_next_state = state::Type::dead;
            //reset_anim(anim("dead"));
            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 0.0F, -8.0F },
                                velocity(), 2.0F,
                                state::Type::idle);
            particle::spawn_fan(this, 0.0F, 360.0F, 7,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 0.0F, -8.0F },
                                velocity() * 0.5F, 1.0F,
                                state::Type::idle);
            particle::spawn_fan(this, 0.0F, 360.0F, 4,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 0.0F, -8.0F },
                                {}, 1.0F,
                                state::Type::idle);
            particle::spawn(this, particle::Type::health, position() + Vec2F{ 4.0F, 4.0F }, {});
            m_time_left_alive = 0;
            m_time_left_dead = 100;

            console::log(class_name(), "::state_dead() velocity: ", velocity().x, " ", velocity().y, "\n");

            if (sound_is_playing("hover")) {
                sound_stop("hover");
            }
            reset_anim("dead");
        }

        velocity({});
        move_velocity({});

        console::log(class_name(), "::state_dead() ", m_time_left_dead, "\n");

        if (m_time_left_dead == 0) {
            m_time_left_alive = m_config.time_to_be_alive();
            m_next_state = state::Type::run;
            health::reset(m_health_id);
        }
    }
}