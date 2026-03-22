module;
#include <sstream>
module entity.player;
import console;
import particle_system;

namespace entity {
    void Player::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        cType other_type = other.owner->type();        

        if (time_left_colliding_with(other_type) > 0) return;

        aabb::cName our_name   = aabb::name(our.id);
        aabb::cName other_name = aabb::name(other.id);

        cRectF our_rect   = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                              aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (other_rect.x < our_rect.x && velocity().x > 0.0F ||
            other_rect.x > our_rect.x && velocity().x < 0.0F) return;*/

        cState other_state    = other.owner->state();

        cVec2F our_velocity   = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        if (m_state == State::sling) {
            //add_position_x( -overlap_x );
            //velocity_x(-velocity().x;

            if (is_arch(other_type)) {
                collide_y(our, other);
            } else if (is_clip(other_type)) {
                if (!sound_is_playing("bump_head")) {
                    sound_stop("bump_head");
                    sound_position("bump_head", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("bump_head");

                    m_rotation_speed *= -1.0F;
                }
            }
            return;
        }

        //console::log(sprite::to_string(other_type), " overlap_x: ", overlap_x, "\n");


        if (is_arch(other_type)) {
            if (m_is_climbing_ledge) {
                return;
            }
            if (m_is_on_ground && m_is_on_slope) {
                add_position_x(-overlap_x);
                velocity_x(velocity().x * 0.0F);
                //return;
            }
            collide_y(our, other);
        }
        else if (is_slope(other_type) && other_type != Type::slope_U && m_state == State::swim) {
            collide_y(our, other);
        }
        else if (other_type == Type::brick) {
            if (our_velocity.y >= 6.0F) {
                collide_y(our, other);
                return;
            }
            if (our_rect.h < other_rect.y + 2.0F ||
                our_rect.y > other_rect.h - 2.0F) {
                //collide_y(our, other);
                return;
            }
            add_position_x(-overlap_x);
            velocity_x(velocity().x * 0.95F);
        }
        else if (other_type == Type::bug) {
            return;
            if (our_rect.h < other_rect.y + 2.0F) return;

            if (our_velocity.y > 3.0F) {
                console::log("\nhmm\n");
                collide_y(our, other);
                return;
            }
            add_position_x(-overlap_x);

            //velocity_x(-0.9F;

            /*if (std::abs(our_velocity.x) < std::abs(other_velocity.x)) {
                velocity_x(other_velocity.x * 4.0F;
                velocity().y -= 2.0F;
                add_position_x( -overlap_x );
            }*/
            /*if (our_rect.h < other_rect.y + 2.0F ||
                our_rect.y > other_rect.h - 2.0F) {
                return;
            }
            add_position_x( -overlap_x );*/
        }
        else if (other_type == Type::clip) {
            if (our_rect.h > other_rect.y && our_rect.h - other_rect.y < 2.0F) return;
            m_is_wall_to_left = other_rect.x < our_rect.x;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);

            if (m_state == State::dive) {
                if (our_velocity.y > 0.0F &&
                    (m_is_wall_to_left && is_pressed(key_left)) || is_pressed(key_right)) {
                    next_state(State::run);
                    return;
                }
            }
            if (m_time_left_jumping_wall == 0 && our_velocity.y > 0.0F &&
                (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
                m_is_sliding_wall = true;
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                sprite::is_leftward(m_sprite_id, !m_is_wall_to_left);
            }
        }
        else if (other_type == Type::clip_U || other_type == Type::slope_U) { }
        else if (other_type == Type::clip_D) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }
        }
        else if (other_type == Type::clip_ledge) {
            if (m_state == State::ledge) return;
            if (our_rect.h > other_rect.y && our_rect.h - other_rect.y < 2.0F) return;
            if (our_rect.y >= other_rect.h - 2.0F) {
                //console::log("entity::Player::collide_x clip_ledge skip\n");
                return;
            }
            if (our_rect.h <= other_rect.y + 2.0F) {
                //console::log("entity::Player::collide_x clip_ledge above skip\n");
                return;
            }
            if (our_velocity.x < 0.0F && our_rect.w < other_rect.x + our_velocity.x ||
                our_velocity.x > 0.0F && our_rect.x > other_rect.w + our_velocity.x) {
                return;
            }

            add_position_x(-overlap_x);
            if (m_state == State::swim) {
                return;
            }
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = other_rect.x < our_rect.x;

            if (m_state == State::dive) {
                if (velocity().y > 0.0F &&
                    (m_is_wall_to_left && is_pressed(key_left)) || is_pressed(key_right)) {
                    next_state(State::run);
                    return;
                }
            }
            m_is_on_slope = false;
            if (m_is_hovering || is_locked(key_jump) || m_is_carrying) return;
            //if (m_is_on_ground && !is_pressed(key_up)) return;

            if ((m_is_wall_to_left && our_velocity.x > 0.0F) ||
                (!m_is_wall_to_left && our_velocity.x < 0.0F)) {                
                return;
            }

            if (m_time_left_jumping_wall == 0 && our_velocity.y > 0.0F &&
                (m_is_wall_to_left && is_pressed(key_left) || !m_is_wall_to_left && is_pressed(key_right))) {
                m_is_sliding_wall = true;
                if (our_velocity.y > 1.0F) {
                    velocity_y(1.0F);
                }
                sprite_is_leftward(!m_is_wall_to_left);
            }
            if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
            if (our_velocity.y > 0.0F && !m_is_carrying && !m_is_hovering) {
                if (m_is_wall_to_left && is_pressed(key_left) ||
                    !m_is_wall_to_left && is_pressed(key_right)) {
                    if (m_state != State::ledge && our_rect.y <= other_rect.y && our_rect.y + 16 >= other_rect.y) {
                        
                        m_next_state = State::ledge;
                        m_is_sliding_wall = false;
                        position_y(other_rect.y - 6);
                        velocity_y(0.0F);

                        reset_anim("ledge_grab");

                        sound_position("ledge_grab", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                        sound_play("ledge_grab");
                    }
                }
            }
        }
        else if (other_type == Type::clip_L) {
            if (our_velocity.x < 0.0F || m_state == State::ledge) return;
            add_position_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = false;
            if (m_state == State::dive && velocity().y > 0.0F && is_pressed(key_right)) {
                m_next_state = State::run;
                return;
            }
            m_is_on_slope = false;
            
            if (m_is_on_ground) return;

            if (m_state != State::run) return;

            if (our_velocity.y > 0.0F && is_pressed(key_right) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, true);
                if (velocity().y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_anim_ids["slide_wall"]);
            }
            if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall <= 0.0F) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        }
        else if (other_type == Type::clip_LD) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (our_velocity.x <= 0.0F || m_state == State::ledge) return;

            add_position_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = false;
            if (m_state == State::dive && our_velocity.y > 0.0F) {
                m_next_state = State::run;
                return;
            }
            m_is_on_slope = false;
            
            if (m_is_on_ground) return;

            if (our_velocity.y > 0.0F && is_pressed(key_right) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, true);
                if (our_velocity.y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_anim_ids["slide_wall"]);
            }
            if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall <= 0.0F) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        }
        else if (other_type == Type::clip_R) {
            if (our_velocity.x >= 0.0F || m_state == State::ledge) return;
            add_position_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = true;
            if (m_state == State::dive && our_velocity.y > 0.0F && is_pressed(key_left)) {
                m_next_state = State::run;
                return;
            }
            m_is_on_slope = false;
            if (m_is_on_ground) return;
            if (our_velocity.y > 0.0F && is_pressed(key_left) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, false);
                if (our_velocity.y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_anim_ids["slide_wall"]);
            }
            if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        }
        else if (other_type == Type::clip_RD) {
            if ((/*other_rect.h < our_rect.y &&*/ m_is_on_ground) || m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (velocity().x < 0.0F && velocity().x < -m_slide_ground_velocity_limit.x ||
                        velocity().x > 0.0F && velocity().x > m_slide_ground_velocity_limit.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (our_velocity.x >= 0.0F || m_state == State::ledge) return;

            add_position_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            if (m_state == State::dive && our_velocity.y > 0.0F) {
                m_next_state = State::run;
                return;
            }
            m_is_wall_to_left = true;
            m_is_on_slope = false;
            
            if (m_is_on_ground) return;
            if (our_velocity.y > 0.0F && is_pressed(key_left) && m_time_left_jumping_wall == 0 && !m_is_on_ground && !m_is_carrying && !m_is_hovering) {
                m_is_sliding_wall = true;
                sprite::is_leftward(m_sprite_id, false);
                if (our_velocity.y > 1.0F) {
                    velocity_y(1.0F);
                }
                //set_anim(m_anim_ids["slide_wall"]);
            }
            if (is_pressed(key_jump) && !is_locked(key_jump) && m_time_left_jumping_wall <= 0.0F && !m_is_carrying && !m_is_hovering) {
                m_time_left_jumping_wall = m_time_to_jump_wall;
                m_num_jumps = 0;
            }
        }
        else if (is_conduit(other_type)) {
            return;
            if (other.owner->is_dead()) return;
            add_position_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        }
        else if (is_coin(other_type)) {
            if (!other.owner->is_dead()) {
                sound::position(other.owner->sound_id("dead"), { other_rect.x / (WINDOW_W / 2.0F), other_rect.y / (WINDOW_H / 2.0F) });
                sound::play(other.owner->sound_id("dead"));
            }
            other.owner->time_left_alive(0);
            other.owner->time_left_dead(U16_MAX);
        }
        else if (other_type == Type::frog) {
            if (m_state == State::sling) {
                console::log("Player::collide_x() frog state: sling, nah\n");
                return;
            }

            //if (our_rect.h + 1.0F <= other_rect.y) return;
            //velocity_x(moved_velocity_x(0.0F);

            add_position_x(-overlap_x);

            if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                return;
            }

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_rect.x < other_rect.x) {
                add_velocity({ -1.0F, 0.0F });
                other.owner->add_velocity({ 1.0F, 0.0F });
            }
            else {
                add_velocity({ 1.0F, 0.0F });
                other.owner->add_velocity({ -1.0F, 0.0F });
            }
        }
        else if (other_type == Type::level_L_0) {
            m_next_level = start::Type::L_0;
            m_next_start = { start::Type::R_0, 0 };
        }
        else if (other_type == Type::level_R_0) {
            m_next_level = start::Type::R_0;
            m_next_start = { start::Type::L_0, 0 };
        }
        else if (other_type == Type::level_center) {
            if (velocity().x != 0.0F) {
                m_is_to_save = true;
            }
        }
        else if (other_type == Type::mole) {
            collide_y(our, other);
        }
        else if (other_type == Type::particle_brick) {
            //add_position_x( -overlap_x );
            //velocity().x *= 0.95F;
        }
        else if (other_type == Type::particle_health) { }
        else if (other_type == Type::particle_melee) { }
        else if (other_type == Type::particle_shot) {
            console::log("\n\nentity::Player::collide_x() ", to_string(other_type), "\n\n");
            hurt(other.owner);
        }
        else if (other_type == Type::slope_L_1x1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == State::swim) {
                if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (our_velocity.x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            //velocity_y(-velocity().x;
            //position().y -= 1.0F;
            add_position({ 0.0F, -velocity().x });

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == State::swim) {
                if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95F);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //add_position_x( -overlap_x );
            //velocity_y(0.0F);
            if (our_velocity.x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            //velocity_y(velocity().x);
            //position().y -= 1.0F;
            add_position({ 0.0F, velocity().x });
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == State::swim) {
                if (our_velocity.x > 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.5F);
                    velocity_x(our_velocity.x * 0.95F);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            //velocity_y(-velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            add_position({ 0.0F, -velocity().x / 2.0F });

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
            return;
            if (our_rect.h > other_rect.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == State::swim) {
                if (our_velocity.x < 0.0F && std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.5F);
                    velocity_x(velocity().x * 0.95f);
                    add_position({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //add_position_x( -overlap_x );
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            //velocity_y(velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            add_position({ 0.0F, velocity().x / 2.0F });

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::spring_U) {
            collide_y(our, other);
        }
        else if (other_type == Type::train) {
            
        }
        else if (other_type == Type::player) {
            return;
            add_position_x(-overlap_x);
            //velocity_x(0.0F);

            //velocity_x(velocity().x * 0.5F + velocity.x * 0.5F;

            velocity_x(velocity().x * 0.75F + other_velocity.x * 0.25f);
            //velocity_y(velocity().y * 0.5F + velocity.y * 0.5F;
        }
        else if (is_water_line(other_type)) {
            collide_y(our, other);
        }

        if (m_is_sliding_wall && m_state == State::run) {
            //console::log(class_name(), "::collide_x() sliding wall: ", (int)m_time_sliding_wall, "\n");
            if (m_time_sliding_wall > 5) {
                m_time_sliding_wall = 0;
                cF32 wall_x = our_rect.x < other_rect.x ? other_rect.x : other_rect.w;

                particle::cType type = our_rect.x < other_rect.x ? particle::Type::dust_L : particle::Type::dust_R;

                particle::spawn({ .parent   = this,
                                  .type     = type,
                                  .position = Vec2F{ wall_x, position().y + 8.0F },
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        }
    }
}