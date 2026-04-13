module entity.player;
import aabb;
import camera;
import console;
import health;
import particle_system;
import input;

namespace entity {
    void Player::state_run() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            sprite_is_hidden(false);
            sprite::is_hidden(m_sling_shot_sprite, true);
            sprite::is_hidden(m_sling_shot_bg_sprite, true);

            m_water_line_y = 0.0F;
            if (m_prev_state != state::Type::run) {
                m_prev_state = state::Type::run;
                m_next_state = state::Type::run;
                console::log(class_name(), "::state_run()\n");
            }
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }

            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
        }
        if (!m_is_on_ground and !m_is_carrying and !m_is_climbing_ledge and !m_is_down_thrusting) {
            if (is_pressed(key_up)) {
                release(key_up);
                m_next_state = state::Type::dive;                
                return;
            }
        }

        direction_x(sprite_is_leftward() ? -1.0F : 1.0F);
        camera::is_idle_recentered = !m_is_carrying;
        camera::is_offset_with_direction = m_is_carrying;


        if (is_pressed(input::Key::shift)) {
            release(input::Key::shift);
            if (m_current_anim == anim("jump_spin")) {
                set_anim("jump");
            }
            m_next_state = state::Type::shoot;
            return;            
        }


        if (is_pressed(input::Key::h)) {
            release(input::Key::h);
            health::amount_add(m_health_id, -8.0f);
            console::log(class_name(), " heatlh: ", health::amount(m_health_id), "\n");
        }
        if (is_pressed(input::Key::j)) {
            release(input::Key::j);
            health::amount_add(m_health_id, 8.0f);
            //console::log("heath: ", health::get(m_health_id)->amount, "\n");
        }
        if (is_pressed(input::Key::b)) {
            release(input::Key::b);
            particle::spawn_fan(this, 180.0F, 0.0F, 4,
                                particle::Type::drop_blood,
                                position() + Vec2F{ 6.0F, -4.0F },
                                velocity() + moved_velocity(), 3.0F);
        }

        //console::log("is carrying: ", m_is_carrying, "\n");

        if (is_pressed(key_melee) and !m_is_carrying and m_time_left_ducking == 0 and !is_locked(key_jump)) {
            if (!is_locked(key_melee)) {
                //console::log(class_name(), "::run() attack melee\n");
                lock(key_melee);
                m_time_left_melee = m_time_to_melee;
                reset_anim("melee");

            }
        } else {
            unlock(key_melee);
        }

        if (m_time_left_melee > 0) {
            if (m_time_left_melee == m_time_to_melee) {
                particle::spawn({ .parent = this,
                                           .type = particle::Type::melee,
                                           .position = position() + Vec2F{ sprite::is_leftward(m_sprite) ? -16.0F : 16.0F, 0.0F},
                                           .velocity = velocity() + Vec2F{ sprite::is_leftward(m_sprite) ? -2.0F : 2.0F, 0.0F },                    
                                           .state = state::Type::idle
                    });
            }
            --m_time_left_melee;

            if (m_is_on_ground) {
                velocity_x(velocity().x * 0.9F);
            }
        }

        //console::log("player position: ", position().x, " ", position().y, "\n");

        if (m_time_left_bouncing > 0) {
            //console::log("time left bouncing: ", (int)m_time_left_bouncing, "\n");
            --m_time_left_bouncing;
            if (m_time_left_bouncing == 0 and m_is_on_ground) {
                max_velocity(m_ground_max_velocity);
            }
        }


        if (m_time_left_until_down_thrust > 0) {
            --m_time_left_until_down_thrust;
            velocity({ 0.0F, 0.0F });
            if (m_time_left_until_down_thrust > 0) {
                return;
            }
            else if (!m_is_down_thrusting) {
                m_is_down_thrusting = true;
                max_velocity_y(10.0F);
                velocity_y(10.0F);
            }
        }

        if (m_is_down_thrusting) {
            if (m_is_on_ground) {
                m_is_down_thrusting = false;
                m_time_left_hitting_ground = m_time_to_hit_ground;
                reset_anim("hit_ground");
                sound_position("hit_ground", { (position().x +  8.0F) / app::config::extent().x / 2.0F,
                                               (position().y + 24.0F) / app::config::extent().y / 2.0F });
                sound_play("hit_ground");
            } else {
                set_anim("down_thrust");
                return;
            }
        }

        if (m_time_left_hitting_ground > 0) {
            if (m_time_left_hitting_ground == m_time_to_hit_ground) {
                particle::spawn_fan(this, 180.0F, 0.0F, 8, particle::Type::drop_blood, position() + Vec2F{ 0.0F, 4.0F }, {}, 5.0F);
                particle::spawn_fan(this, 180.0F, 0.0F, 8, particle::Type::drop_blood, position() + Vec2F{ 0.0F, 4.0F }, {}, 4.0F);

                spawn_down_thrust(position() + Vec2F{ 8.0f, 8.0f });

                //camera::init_shake(12, { 16, 24 }, { 24, 32 }, 0.05F);
                camera::init_shake(320, 2, { 24, 32 }, { 64, 64 }, 0.01F);                
            }
            --m_time_left_hitting_ground;
            velocity_x(0.0F);
            //velocity_y(0.0F);
            return;
        }


        if (m_is_on_ground and m_time_left_ducking == 0 and m_time_left_rising == 0 and
            !is_pressed(key_left) and !is_pressed(key_right) and m_time_left_melee == 0
            //velocity().x >= -acceleration().x and velocity().x <= acceleration().x
            ) {
            set_anim("idle");
        }

        m_is_sliding_ground = false;
        if (m_is_on_ground and m_time_left_melee == 0 and m_time_left_skidding == 0) {
            if (velocity().x >= -acceleration().x and velocity().x <= acceleration().x) {
                if (m_time_left_ducking == 0 and m_time_left_rising == 0) {
                    set_anim("idle");
                }
                if (m_time_left_ducking > 0) {
                    set_anim("duck");
                }
            } else {
                if (m_time_left_ducking > 0) {
                    if (m_time_left_rising > 0) {
                        if (!m_is_carrying and
                            velocity().x < -m_slide_ground_max_velocity.x or
                            velocity().x > m_slide_ground_max_velocity.x) {
                            m_is_sliding_ground = true;
                        }
                        if (m_is_sliding_ground) {
                            set_anim("slide_ground");
                        } else {
                            anim::source_x(anim("duck"), anim::source(anim("duck")).w * 4);
                            set_anim("crawl");
                            if (velocity().x < max_velocity().x * -0.5F) {
                                velocity({ max_velocity().x * -0.5F, velocity().y });
                            }
                            else if (velocity().x > max_velocity().x * 0.5F) {
                                velocity({ max_velocity().x * 0.5F, velocity().y });
                            }
                        }
                    }
                } else {
                    anim::source_x(anim("slide_ground"), 0);
                    //console::log("run walk\n");
                    is_pressed(key_sprint) ? set_anim("run") : set_anim("walk");
                }
            }
        }

        if (is_pressed(key_left) and !is_locked(key_left)) {
            if (!m_is_sliding_ground and (velocity().x > -m_ground_max_velocity.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                //console::log("max_velocity().x: ", max_velocity().x, "\n");
                //console::log("acceleration().x: ", acceleration().x, "\n");
                //console::log("m_ground_max_velocity.x: ", m_ground_max_velocity.x, "\n");
                velocity_add_x(-acceleration().x);
            }

            if (m_is_on_ground and m_time_left_ducking == 0) {
                if (velocity().x >= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (!sprite::is_leftward(m_sprite)) {
                        anim::source_x(anim("skid"), 0);
                    }
                }
                if (velocity().x < -2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }            
            sprite::is_leftward(m_sprite, true);            
        }
        if (is_pressed(key_right) and !is_locked(key_right)) {
            if (!m_is_sliding_ground and (velocity().x < m_ground_max_velocity.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                velocity_add_x(acceleration().x);
            }
            if (m_is_on_ground and m_time_left_ducking == 0) {
                if (velocity().x <= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (sprite::is_leftward(m_sprite)) {
                        anim::source_x(anim("skid"), 0);
                    }
                }
                if (velocity().x > 2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }
            sprite::is_leftward(m_sprite, false);            
        }
        if (m_time_left_skidding > 0 and m_time_left_melee == 0) {
            if (m_time_left_skidding == m_time_to_skid and !sound_is_playing("skid")) {
                sound_position("skid", { position().x / (app::config::extent().x / 2.0F),
                                         position().y / (app::config::extent().y / 2.0F) });
                sound_play("skid");
            }
            --m_time_left_skidding;
            set_anim("skid");
            //console::log(class_name(), "::run m_time_left_skidding: ", m_time_left_skidding, "\n");
        }
        else {
            anim::source_x(anim("skid"), 0);
        }

        acceleration(start_acceleration());
        deceleration(start_deceleration());

        if (m_is_on_ground) {
            max_velocity(m_ground_max_velocity);

            if (m_num_jumps == 3) {
                m_num_jumps = 0;
            }

            if (m_is_sliding_ground) {
                deceleration_x(0.025f);

                if (m_time_sliding_ground == 0 and !sound_is_playing("skid")) {
                    sound_position("skid", { (position().x + 8.0F) / app::config::extent().x / 2.0F,
                                             (position().y + 8.0F) / app::config::extent().y / 2.0F });
                    sound_play("skid");
                }

                ++m_time_sliding_ground;
            } else {
                deceleration_x(0.1F);
                m_time_sliding_ground = 0;
            }
            acceleration_x(0.2F);
        } else {
            //transform::max_velocity(m_transform, m_air_velocity_limit);
            deceleration_x(0.0F);
            acceleration_x(0.1F);
            m_time_sliding_ground = 0;
        }

        velocity_add({ 0.0F, acceleration().y });
        //moved_velocity().y += acceleration().y;

        if (moved_velocity().y < 0.0F) {
            moved_velocity_add_y(acceleration().y);
        }


        //console::log("moved y: ", moved_velocity().y, "\n");


        if (m_is_on_ground) {
            //m_velocity_at_jump = 0.0F;
            m_is_hovering = false;
            if (m_time_left_jump_again > 0) {
                m_time_left_jump_again -= 1;
            } else {
                m_num_jumps = 0;
            }

            if (is_pressed(key_sprint)) {
                max_velocity_x(m_ground_max_velocity.x * 1.2F);
            } else {
                max_velocity_x(m_ground_max_velocity.x);
            }
            if (m_is_carrying) {
                max_velocity_x(max_velocity().x * 0.9F);
            }
        }


        if (velocity().y > acceleration().y + (m_is_on_slope ? max_velocity().y : 1.0F)) {
            //if (velocity().y >= 1.0F and !m_is_on_slope) {
            m_is_on_ground = false;
            m_is_on_slope = false;
            //lock(key_down);
            //m_time_left_ducking = 0;
        }
        if (m_is_on_slope and velocity().y >= max_velocity().y) {
            m_is_on_slope = false;
        }
        if (m_is_on_ground or velocity().y > 2.0F) {
            m_is_climbing_ledge = false;
        }


        if (!m_is_on_ground and m_time_left_ducking == 0 and !m_is_climbing_ledge and m_time_left_melee == 0) {
            if (velocity().y > acceleration().y) {
                if (is_pressed(key_jump) and !is_locked(key_jump)) {
                    lock(key_jump);
                    if (moved_velocity().y != 0.0F) {
                        velocity_add({ 0.0F, moved_velocity().y });
                        moved_velocity_y(0.0F);
                    }
                    m_is_hovering = true;
                    m_time_left_ducking = 0;
                    m_time_left_rising = 0;
                }
                else if (m_current_anim != anim("jump_spin")) {
                    reset_anim("fall");
                    anim::speed(anim("fall"), velocity().y / 10.0F);
                }
            }
        }

        if (is_pressed(key_down) and !is_locked(key_down)) {
            if (m_is_carrying) {
                console::log(class_name(), "::state_run() drop carried\n");
                m_is_carrying = false;
                lock(key_down);
            }
            else if (m_is_on_ground) {
                m_time_left_ducking = m_time_to_duck;
                release(key_sprint);
            }
            else {
                if (m_time_left_ducking == 0 and m_time_left_rising == 0) {
                    console::log(class_name(), "::state_run() down thrust ", m_time_left_ducking, " ", m_time_left_rising, "\n");
                    release(key_down);
                    release(key_jump);
                    m_time_left_until_down_thrust = m_time_until_down_thrust;
                    reset_anim("down_thrust");
                    if (sound_is_playing("hover")) {
                        sound_stop("hover");
                    }
                    sound_position("down_thrust", { (position().x + 8.0F) / app::config::extent().x / 2.0F, (position().y + 8.0F) / app::config::extent().y / 2.0F });
                    sound_play("down_thrust");
                }
            }
        }

        if (m_time_left_ducking == 0) {
            if (is_pressed(key_up) and !is_locked(key_up)) {
                release(key_up);
                particle::spawn( { .parent   = this,
                                   .type     = particle::Type::interact,
                                   .position = position() + Vec2F{ -8.0f, 4.0f },
                                   .velocity = velocity() + moved_velocity(),
                                   .state    = is_pressed(key_sprint) ? state::Type::run : state::Type::none });
            }
        }

        if (m_time_left_lever > 0) {
            --m_time_left_lever;
            set_anim("lever");
            velocity({ 0.0F,0.0F });
        }



        /*if (id == 0) {
            console::log("position: ", position().x, " ", position().y, "\n");
        }*/


        if (is_pressed(key_jump)) {
            if (m_is_on_ground and !is_locked(key_jump) and m_time_left_ducking == 0) {
                jump();
            } else {
                if (m_time_left_holding_jump > 0 and velocity().y < 0.0F) {
                    --m_time_left_holding_jump;
                    velocity_add({ 0.0F,-0.3F });
                    //console::log("Player holding jump: ", m_time_left_holding_jump, "\n");
                } else {
                    //release(key_jump);
                    m_time_left_holding_jump = 0;
                }
            }
        } else {
            unlock(key_jump);
            m_is_hovering = false;
        }

        if (m_current_anim == anim("run") or m_current_anim == anim("walk")) {
            if (velocity().x < 0.0F) {
                anim::speed(m_current_anim, velocity().x / -5.0F);
            }
            else {
                anim::speed(m_current_anim, velocity().x / 5.0F);
            }
            if (anim::speed(m_current_anim) < 0.2F) {
                anim::speed(m_current_anim, 0.2F);
            }
        }


        if (m_is_hovering) {
            cF32 velocity_limit_y = m_is_carrying ? 1.5F : 1.0F;

            if (velocity().y >= velocity_limit_y) {
                velocity_y(velocity_limit_y);
                set_anim("hover");
            }
            if (!sound_is_playing("hover")) {
                sound_position("hover", { position().x / (app::config::extent().x / 2.0F),
                                          position().y / (app::config::extent().y / 2.0F) });
                sound_is_looped("hover", true);
                sound_play("hover");
            }
        } else {
            sound_stop("hover");
        }


        

        if (anim::source(anim("jump_spin")).x >= anim::source(anim("jump_spin")).w * 4) {
            anim::first_frame(anim("jump_spin"), 4);
        }
        else {
            anim::first_frame(anim("jump_spin"), 0);
        }




        //console::log(start::to_string(m_next_level), "\n");

        if (is_pressed(input::Key::num_5)) {
            release(input::Key::num_5);
            m_prev_state = m_state;
            m_next_state = state::Type::swim;
            m_time_left_in_state = 20;
        }


        /*if (m_velocity_at_jump < 0.0F and velocity().x < m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }
        if (m_velocity_at_jump > 0.0F and velocity().x > m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }*/

        //console::log("sling_position: ", (position().x - m_sling_position.x), "\n");

        if (m_time_left_ducking > 0) {
            //console::log("Player::run() time_left_ducking: ", (int)m_time_left_ducking, "\n");
            if (!m_is_sliding_ground) {
                if (!is_pressed(key_left) and !is_pressed(key_right)) {
                    m_time_left_ducking > 0 ? set_anim("duck") : reset_anim("duck");
                } else {
                    set_anim("crawl");
                }
            }            
            anim::source_x(anim("rise"), 0);
            m_time_left_rising = m_time_to_rise;
        }

        if (m_time_left_ducking == 0) {
            anim::source_x(anim("duck"), 0);
            if (m_time_left_rising > 0) {                
                set_anim("rise");
            }
        }

        if (m_next_state == state::Type::swim) {
            if (m_time_left_in_state > 0) --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_state = state::Type::swim;
                m_is_first_state_update = true;
                sprite::is_upended(m_sprite, sprite::is_leftward(m_sprite));
                sprite::is_leftward(m_sprite) ? sprite::angle(m_sprite, 180.0F) : sprite::angle(m_sprite, 0.0F);
            }
        }
        if (m_next_state == state::Type::sling and m_time_left_in_state == 0) {
            m_state = state::Type::sling;
            m_is_first_state_update = true;
        }
    }
}