module entity.player;
import app.config;
import aabb;
import camera;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_duck(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_time_left_rising = m_time_to_rise;

            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);
            sprite::is_hidden(m_target_sprite, true);

            reset_anim("duck");

            anim::source_x(anim("duck"), 0);
            release(key_sprint);
            sprite_is_hidden(false);
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }            
        }
        
        //console::log(class_name(), "::state_duck() ", m_time_left_rising, "\n");

        velocity_add_y(acceleration().y);

        //anim::source_x(anim("slide_ground"), 0);

        if (is_pressed(key_down)) {
            m_time_left_rising = m_time_to_rise;
        }

        if (m_time_left_rising > 0) {
            if (m_time_left_rising < m_time_to_rise) {
                set_anim("rise");
            }
            else {
                anim::source_x(anim("rise"), 0);

                if (velocity().x < -m_slide_ground_min_velocity.x or
                    velocity().x > m_slide_ground_min_velocity.x) {
                    m_is_sliding_ground = true;
                }
                if (std::abs(velocity().x) > acceleration().x) {
                    if (m_is_sliding_ground) {
                        set_anim("slide_ground");
                    } else {
                        set_anim("crawl");
                        if (velocity().x < max_velocity().x * -0.5F) {
                            velocity({ max_velocity().x * -0.5F, velocity().y });
                        } else if (velocity().x > max_velocity().x * 0.5F) {
                            velocity({ max_velocity().x * 0.5F, velocity().y });
                        }
                    }
                } else {
                    if (m_current_anim == anim("duck")) {
                        set_anim("duck");
                    } else {
                        reset_anim("duck");
                        anim::source_x(anim("duck"), anim::source(anim("duck")).w * 4);
                    }
                }
            }
            --m_time_left_rising;
        }
        if (m_time_left_rising == 0) {
            m_next_state = m_saved_state;
        }

        if (velocity().x >= -m_slide_ground_min_velocity.x and
            velocity().x <=  m_slide_ground_min_velocity.x) {
            m_is_sliding_ground = false;
        }


        if (m_is_sliding_ground) {
            deceleration_x(0.025F);
            if (m_time_sliding_ground == 0 and !sound_is_playing("skid")) {
                sound_position("skid", { (position().x + 8.0F) / app::config::extent().x / 2.0F,
                                         (position().y + 8.0F) / app::config::extent().y / 2.0F });
                sound_play("skid");
            }
            ++m_time_sliding_ground;

            if (m_time_sliding_ground % 4 == 0) {
                console::log("spawn dust\n");
                particle::spawn({ .parent = this,
                                  .type = particle::Type::dust,
                                  .position = position() + Vec2F{7.0F, 22.0F} - Vec2F{0.0F, 0.0F},
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        } else {
            deceleration_x(start_deceleration().x);
            m_time_sliding_ground = 0;
        }


        if (is_pressed(key_left) and !is_locked(key_left)) {
            if (!m_is_sliding_ground and (velocity().x > -max_velocity().x * 0.5F)) {
                velocity_add_x(-acceleration().x);
            }            
            sprite::is_leftward(m_sprite, true);
        }
        if (is_pressed(key_right) and !is_locked(key_right)) {
            if (!m_is_sliding_ground and (velocity().x < max_velocity().x * 0.5F)) {
                velocity_add_x(acceleration().x);
            }
            sprite::is_leftward(m_sprite, false);
        }

    }
}