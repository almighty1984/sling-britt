module entity.player;
import aabb;
import camera;
import console;
import health;
import particle_system;

namespace entity {
    void Player::state_run() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            sprite_is_hidden(false);
            m_water_line_y = 0.0F;
            if (m_prev_state != State::run) {
                m_prev_state = State::run;
                m_next_state = State::run;
                console::log("entity::Player::run\n");
            }
            if (sprite_angle() != 0.0F) {
                sprite_is_leftward(velocity().x < 0.0F);
                sprite_is_upended(false);
                sprite_angle(0.0F);
            }
        }

        if (!m_is_on_ground && !m_is_carrying && !m_is_sliding_wall && !m_is_climbing_ledge) {
            if (is_pressed(key_up)) {
                release(key_up);
                m_next_state = State::dive;                
                return;
            }
        }

        for (auto& i : m_aabb_ids) {
            if (aabb::name(i) == aabb::Name::body) {
                aabb::is_active(i, true);
            } else {
                aabb::is_active(i, false);
            }
        }

        if (is_pressed(input::Key::ctrl)) {
            lock(key_up);
            lock(key_down);
            lock(key_left);
            lock(key_right);
        } else {
            if (!is_pressed(key_up))    unlock(key_up);
            if (!is_pressed(key_down))  unlock(key_down);
            if (!is_pressed(key_left))  unlock(key_left);
            if (!is_pressed(key_right)) unlock(key_right);
        }

        if (is_pressed(input::Key::h)) {
            release(input::Key::h);
            health::add_amount(m_health_id, -8.0f);
            console::log("heath: ", health::amount(m_health_id), "\n");
        }
        if (is_pressed(input::Key::j)) {
            release(input::Key::j);
            health::add_amount(m_health_id, 8.0f);
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

        if (is_pressed(key_melee) && !m_is_carrying && !m_is_ducking && !is_locked(key_jump)) {
            if (!is_locked(key_melee)) {
                //console::log("entity::Player::run() attack melee\n");
                lock(key_melee);
                m_time_left_melee = m_time_to_melee;
                reset_anim("melee");

                sound_position("melee", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("melee");
            }
        } else {
            unlock(key_melee);
        }

        if (m_time_left_melee > 0) {
            if (m_time_left_melee == m_time_to_melee) {
                particle::spawn({ .parent = this,
                                           .type = particle::Type::melee,
                                           .position = position() + Vec2F{ sprite::is_leftward(m_sprite_id) ? -16.0F : 16.0F, 0.0F},
                                           .velocity = velocity() + Vec2F{ sprite::is_leftward(m_sprite_id) ? -2.0F : 2.0F, 0.0F}
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
            if (m_time_left_bouncing == 0 && m_is_on_ground) {
                max_velocity(m_ground_velocity_limit);
            }
        }


        if (m_time_left_until_down_thrust > 0) {
            --m_time_left_until_down_thrust;
            velocity({ 0.0F, 0.0F });
            if (m_time_left_until_down_thrust > 0) {
                return;
            } else if (!m_is_down_thrusting) {
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

                sound_position("hit_ground", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
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

                camera::init_shake(12, { 16, 24 }, { 24, 32 }, 0.05F);
            }
            --m_time_left_hitting_ground;
            velocity_x(0.0F);
            //velocity_y(0.0F);
            return;
        }


        if (m_is_on_ground && !m_is_ducking && m_time_left_rising == 0 &&
            !is_pressed(key_left) && !is_pressed(key_right) && m_time_left_melee == 0
            //velocity().x >= -acceleration().x && velocity().x <= acceleration().x
            ) {
            set_anim("idle");
        }

        m_is_sliding_ground = false;
        if (m_is_on_ground && m_time_left_melee == 0 && m_time_left_skidding == 0) {
            if (velocity().x >= -acceleration().x && velocity().x <= acceleration().x) {
                if (!m_is_ducking && m_time_left_rising == 0) {
                    set_anim("idle");
                }
                if (m_is_ducking) {
                    set_anim("duck");
                }
            } else {
                if (m_is_ducking) {
                    if (m_time_left_rising == m_time_to_rise) {
                        if (!m_is_carrying &&
                            velocity().x < -m_slide_ground_velocity_limit.x ||
                            velocity().x > m_slide_ground_velocity_limit.x) {
                            m_is_sliding_ground = true;
                        }

                        if (m_is_sliding_ground) {
                            set_anim("slide_ground");
                        } else {
                            anim::source_x(anim_id("duck"), anim::source(anim_id("duck")).w * 4);
                            set_anim("crawl");
                            if (velocity().x < max_velocity().x * -0.5F) {
                                velocity({ max_velocity().x * -0.5F, velocity().y });
                            } else if (velocity().x > max_velocity().x * 0.5F) {
                                velocity({ max_velocity().x * 0.5F, velocity().y });
                            }
                        }
                    }
                } else {
                    anim::source_x(anim_id("slide_ground"), 0);
                    //console::log("run walk\n");
                    is_pressed(key_sprint) ? set_anim("run") : set_anim("walk");
                }
            }
        }

        if (is_pressed(key_left) && !is_locked(key_left)) {
            if (!m_is_sliding_ground && (velocity().x > -m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                //console::log("max_velocity().x: ", max_velocity().x, "\n");
                //console::log("acceleration().x: ", acceleration().x, "\n");
                //console::log("m_ground_velocity_limit.x: ", m_ground_velocity_limit.x, "\n");
                add_velocity_x(-acceleration().x);
            }

            if (m_is_on_ground && !m_is_ducking) {
                if (velocity().x >= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (!sprite::is_leftward(m_sprite_id)) {
                        anim::source_x(anim_id("skid"), 0);
                    }
                }
                if (velocity().x < -2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }
            if (!m_is_sliding_wall && m_time_left_jumping_wall == 0) {
                sprite::is_leftward(m_sprite_id, true);
            }
        }
        if (is_pressed(key_right) && !is_locked(key_right)) {
            if (!m_is_sliding_ground && (velocity().x < m_ground_velocity_limit.x * is_pressed(key_sprint) ? 1.2F : 1.0F)) {
                add_velocity({ acceleration().x, 0.0F });
            }
            if (m_is_on_ground && !m_is_ducking) {
                if (velocity().x <= 0.0F) {
                    m_time_left_skidding = m_time_to_skid;
                    if (sprite::is_leftward(m_sprite_id)) {
                        anim::source_x(anim_id("skid"), 0);
                    }
                }
                if (velocity().x > 2.0F) {
                    m_time_left_skidding = 0.0F;
                }
            }
            if (!m_is_sliding_wall && m_time_left_jumping_wall == 0) {
                sprite::is_leftward(m_sprite_id, false);
            }
        }
        if (m_time_left_skidding > 0 && m_time_left_melee == 0 && !m_is_sliding_wall) {
            if (m_time_left_skidding == m_time_to_skid) {
                sound_position("skid", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("skid");
            }
            --m_time_left_skidding;
            set_anim("skid");
            //console::log("entity::Player::run m_time_left_skidding: ", m_time_left_skidding, "\n");
        } else {
            anim::source_x(anim_id("skid"), 0);
        }

        acceleration({ 0.2F,  0.2F });
        deceleration({ 0.04F, 0.04F });

        if (m_is_on_ground) {
            max_velocity(m_ground_velocity_limit);

            if (m_num_jumps == 3) {
                m_num_jumps = 0;
            }

            if (m_is_sliding_ground) {
                deceleration_x(0.025f);
            } else {
                deceleration_x(0.1F);
            }
            acceleration_x(0.2F);
        } else {
            //transform::max_velocity(m_transform_id, m_air_velocity_limit);
            deceleration_x(0.0F);
            acceleration_x(0.1F);
        }

        add_velocity({ 0.0F, acceleration().y });
        //moved_velocity().y += acceleration().y;

        if (moved_velocity().y < 0.0F) {
            add_moved_velocity_y(acceleration().y);
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
                max_velocity_x(m_ground_velocity_limit.x * 1.2F);
            } else {
                max_velocity_x(m_ground_velocity_limit.x);
            }
            if (m_is_carrying) {
                max_velocity_x(max_velocity().x * 0.9F);
            }
        }


        if (velocity().y > acceleration().y + (m_is_on_slope ? max_velocity().y : 1.0F)) {
            //if (velocity().y >= 1.0F && !m_is_on_slope) {
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_is_ducking = false;
        }
        if (m_is_on_slope && velocity().y >= max_velocity().y) {
            m_is_on_slope = false;
        }
        if (m_is_on_ground || velocity().y > 2.0F) {
            m_is_climbing_ledge = false;
        }


        if (!m_is_on_ground && !m_is_ducking && !m_is_sliding_wall && !m_is_climbing_ledge && m_time_left_melee == 0) {
            if (velocity().y > acceleration().y) {
                if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall == 0.0F) {
                    lock(key_jump);
                    if (moved_velocity().y != 0.0F) {
                        add_velocity({ 0.0F, moved_velocity().y });
                        moved_velocity_y(0.0F);
                    }
                    m_is_hovering = true;
                    m_time_left_ducking = 0;
                    m_time_left_rising = 0;
                } else if (m_current_anim_id != anim_id("jump_spin")) {
                    reset_anim("fall");
                    anim::speed(anim_id("fall"), velocity().y / 10.0F);
                }
            }
        }

        if (is_pressed(key_down) && !is_locked(key_down)) {
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
                if (m_time_left_ducking == 0 && m_time_left_rising == 0) {
                    release(key_down);
                    release(key_jump);
                    m_time_left_until_down_thrust = m_time_until_down_thrust;
                    reset_anim("down_thrust");
                    if (sound_is_playing("hover")) {
                        sound_stop("hover");
                    }
                    sound_position("down_thrust", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("down_thrust");
                }
            }
        }

        if (!m_is_ducking) {
            if (is_pressed(key_up) && !is_locked(key_up)) {
                release(key_up);
                particle::spawn( { .parent   = this,
                                   .type     = particle::Type::interact,
                                   .position = position() + Vec2F{ -8.0f, 4.0f },
                                   .velocity = velocity() + moved_velocity(),
                                   .state    = is_pressed(key_sprint) ? State::run : State::none });
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


        if (m_time_left_jumping_wall > 0) {
            //set_anim("jump"));

            velocity({ 0.0F, 0.0F });
            --m_time_left_jumping_wall;
            if (is_pressed(key_left)) {
                m_is_to_wall_jump_left = true;
            }
            if (is_pressed(key_right)) {
                m_is_to_wall_jump_left = false;
            }
            if (m_time_left_jumping_wall == 0) {
                if (m_is_to_wall_jump_left) {
                    //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                    velocity_x(m_is_wall_to_left ? 1.0F : -1.5F);
                } else {
                    //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                    velocity_x(m_is_wall_to_left ? 1.5F : -1.0F);
                }
                //console::log("velocity.y: ", velocity().y, "\n");
                m_is_on_ground = true;

                unlock(key_jump);
            }
        }


        if (is_pressed(key_jump)) {
            if (m_is_on_ground && !is_locked(key_jump) && !m_is_ducking) {
                lock(key_jump);
                m_time_left_jump_again = m_time_to_jump_again;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_is_hovering = false;
                m_is_sliding_ground = false;
                m_time_left_holding_jump = m_time_to_hold_jump;
                m_time_left_rising = 0;
                //velocity() += moved_velocity();
                //moved_velocity({});

                /*if (velocity().x <= -m_ground_velocity_limit.x || velocity().x >= m_ground_velocity_limit.x) {
                    m_velocity_at_jump = velocity().x;
                    console::log("velocity at jump: ", m_velocity_at_jump, "\n");
                }*/

                add_position({ 0.0F,-2.0F });

                ++m_num_jumps;
                if (m_is_carrying) {
                    m_num_jumps = 1;
                }
                if (m_num_jumps == 3) {
                    m_time_left_skidding = 0;                    
                    velocity_y(-3.0F);
                    reset_anim("jump_spin");
                    sound_pitch("jump", 1.122454545454545F);
                } else {
                    if (m_num_jumps == 2) {
                        velocity_y(-2.2F);
                        reset_anim("jump");
                        sound_pitch("jump", 1.059454545454545F);
                    } else {
                        velocity_y(-2.0F);
                        reset_anim("jump");
                        sound_pitch("jump", 1.0F);
                    }
                    if (m_time_left_skidding > 0) {
                        m_time_left_skidding = 0;
                        reset_anim("jump_skid");
                    }
                }
                sound_position("jump", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("jump");
            } else {
                if (m_time_left_holding_jump > 0 && velocity().y < 0.0F) {
                    --m_time_left_holding_jump;
                    add_velocity({ 0.0F,-0.3F });
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

        if (m_current_anim_id == anim_id("run") || m_current_anim_id == anim_id("walk")) {
            if (velocity().x < 0.0F) {
                anim::speed(m_current_anim_id, velocity().x / -5.0F);
            } else {
                anim::speed(m_current_anim_id, velocity().x / 5.0F);
            }
            if (anim::speed(m_current_anim_id) < 0.2F) {
                anim::speed(m_current_anim_id, 0.2F);
            }
        }


        if (m_is_hovering) {
            cF32 limit_y = m_is_carrying ? 1.5F : 1.0F;

            if (velocity().y >= limit_y) {
                velocity_y(limit_y);
                set_anim("hover");
            }
            if (!sound_is_playing("hover")) {
                sound_position("hover", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_is_looped("hover", true);
                sound_play("hover");
            }
        } else {
            sound_stop("hover");
        }


        if (!m_is_sliding_wall && sound_is_playing("slide_wall")) {
            sound_stop("slide_wall");
        }
        if ((!is_pressed(key_left) && !is_pressed(key_right)) ||
            m_time_left_jumping_wall > 0 || m_is_on_ground || m_is_on_slope ||
            velocity().x < -acceleration().x || velocity().x > acceleration().x) {
            m_is_sliding_wall = false;
        } else {
            if (m_is_sliding_wall) {
                set_anim("slide_wall");
                m_time_left_ducking = 0;
                m_time_left_rising = 0;
                if (sound_is_stopped("slide_wall")) {
                    //console::log("sliding wall\n");
                    sound_play("slide_wall");
                }
                m_is_on_ground = false;
            }
        }


        if (anim::source(anim_id("jump_spin")).x >= anim::source(anim_id("jump_spin")).w * 4) {
            anim::first_frame(anim_id("jump_spin"), 4);
        } else {
            anim::first_frame(anim_id("jump_spin"), 0);
        }


        if (m_current_anim_id == anim_id("run") || m_current_anim_id == anim_id("walk")) {
            if (anim::source(m_current_anim_id).x == 128 || anim::source(m_current_anim_id).x == 384) {
                sound_position("step_grass", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("step_grass");
            }
        }


        //console::log(start::to_string(m_next_level), "\n");

        if (is_pressed(input::Key::num_5)) {
            release(input::Key::num_5);
            m_prev_state = m_state;
            m_next_state = State::swim;
            m_time_left_in_state = 20;
        }


        /*if (m_velocity_at_jump < 0.0F && velocity().x < m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }
        if (m_velocity_at_jump > 0.0F && velocity().x > m_velocity_at_jump) {
            velocity_x(m_velocity_at_jump;
        }*/

        //console::log("sling_position: ", (position().x - m_sling_position.x), "\n");

        if (m_time_left_ducking > 0) {
            //console::log("Player::run() time_left_ducking: ", (int)m_time_left_ducking, "\n");
            --m_time_left_ducking;

            if (!m_is_sliding_ground) {
                if (!is_pressed(key_left) && !is_pressed(key_right)) {
                    m_is_ducking ? set_anim("duck") : reset_anim("duck");
                } else {
                    set_anim("crawl");
                }
            }

            m_is_ducking = m_time_left_ducking > 0;

            anim::source_x(anim_id("rise"), 0);
            m_time_left_rising = m_time_to_rise;
        }

        if (!m_is_ducking) {
            anim::source_x(anim_id("duck"), 0);
            if (m_time_left_rising > 0) {
                --m_time_left_rising;
                set_anim("rise");
            }
        }

        if (m_next_state == State::swim) {
            if (m_time_left_in_state > 0) --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_state = State::swim;
                m_is_first_state_update = true;
                sprite::is_upended(m_sprite_id, sprite::is_leftward(m_sprite_id));
                sprite::is_leftward(m_sprite_id) ? sprite::angle(m_sprite_id, 180.0F) : sprite::angle(m_sprite_id, 0.0F);
            }
        }
        if (m_next_state == State::sling && m_time_left_in_state == 0) {
            m_state = State::sling;
            m_is_first_state_update = true;
        }
    }
}