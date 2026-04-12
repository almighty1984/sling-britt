module entity.player;
import app.config;
import console;
import particle_system;
import sheet.game.save;

namespace entity {
    void Player::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        cType other_type = other.owner->type();        

        if (time_left_colliding_with(other_type) > 0) return;

        aabb::cName our_name   = aabb::name(our.id);
        aabb::cName other_name = aabb::name(other.id);

        cRectF our_points   = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                              aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (other_points.x < our_points.x and velocity().x > 0.0F or
            other_points.x > our_points.x and velocity().x < 0.0F) return;*/

        state::cType other_state    = other.owner->state();

        cVec2F our_velocity   = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cU16 other_number = other.owner->number();

        cF32 overlap_x = our_points.x < other_points.x ? our_points.w - other_points.x : -(other_points.w - our_points.x);

        if (m_state == state::Type::sling) {
            //position_add_x( -overlap_x );
            //velocity_x(-velocity().x;

            if (is_arch(other_type)) {
                collide_y(our, other);
            }
            else if (is_clip(other_type)) {
                if (!sound_is_playing("bump_head")) {
                    sound_stop("bump_head");
                    sound_position("bump_head", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("bump_head");

                    m_rotation_speed *= -1.0F;
                }
            }
            return;
        }

        //console::log(sprite::to_string(other_type), " overlap_x: ", overlap_x, "\n");


        if (is_arch(other_type)) {
            //if (m_is_climbing_ledge) {
            //    return;
            //}
            //if (m_is_on_ground and m_is_on_slope) {
            //    position_add_x(-overlap_x);
            //    velocity_x(velocity().x * 0.0F);
            //    //return;
            //}
            //collide_y(our, other);
        }
        else if (is_slope(other_type) and other_type != Type::slope_U and m_state == state::Type::swim) {
            collide_y(our, other);
        }
        else if (other_type == Type::brick) {
            if (our_velocity.y >= 6.0F) {
                collide_y(our, other);
                return;
            }
            if (our_points.h < other_points.y + 2.0F or
                our_points.y > other_points.h - 2.0F) {
                //collide_y(our, other);
                return;
            }
            position_add_x(-overlap_x);
            velocity_x(velocity().x * 0.95F);
        }
        else if (other_type == Type::bug) {
            return;
            if (our_points.h < other_points.y + 2.0F) return;

            if (our_velocity.y > 3.0F) {
                console::log(class_name(), "::collide_x() bug collide_y()\n");
                collide_y(our, other);
                return;
            }
            position_add_x(-overlap_x);

            //velocity_x(-0.9F;

            /*if (std::abs(our_velocity.x) < std::abs(other_velocity.x)) {
                velocity_x(other_velocity.x * 4.0F;
                velocity().y -= 2.0F;
                position_add_x( -overlap_x );
            }*/
            /*if (our_points.h < other_points.y + 2.0F or
                our_points.y > other_points.h - 2.0F) {
                return;
            }
            position_add_x( -overlap_x );*/
        }
        else if (other_type == Type::clip) {
            if (our_points.h > other_points.y and our_points.h - other_points.y < 2.0F) return;
            m_is_wall_to_left = other_points.x < our_points.x;
            position_add_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);

            if (m_state == state::Type::dive) {
                if (our_velocity.y > 0.0F and
                    (m_is_wall_to_left and is_pressed(key_left)) or is_pressed(key_right)) {
                    next_state(state::Type::run);
                    return;
                }
            }
            if (our_velocity.y > 0.0F and
                (m_is_wall_to_left and is_pressed(key_left) or !m_is_wall_to_left and is_pressed(key_right))) {
                if (m_state == state::Type::run or m_state == state::Type::shoot) {
                    m_next_state = state::Type::slide_wall;
                    sprite::is_leftward(m_sprite, !m_is_wall_to_left);
                }
            }
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) { }
        else if (other_type == Type::clip_D) {
            if ((/*other_points.h < our_points.y and*/ m_is_on_ground) or m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (velocity().x < 0.0F and velocity().x < -m_slide_ground_max_velocity.x or
                        velocity().x > 0.0F and velocity().x > m_slide_ground_max_velocity.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }
        }
        else if (other_type == Type::clip_ledge) {
            if (m_state == state::Type::ledge) return;
            if (our_points.h > other_points.y and our_points.h - other_points.y < 3.0F) return;
            if (our_points.y > other_points.h - 2.0F) {
                //console::log(class_name(), "::collide_x clip_ledge skip\n");
                return;
            }
            if (our_points.h < other_points.y + 2.0F) {
                //console::log(class_name(), "::collide_x clip_ledge above skip\n");
                return;
            }
            if (our_velocity.x < 0.0F and our_points.x < other_points.w - 4.0F or
                our_velocity.x > 0.0F and our_points.w > other_points.x + 4.0F) {
                return;
            }

            position_add_x(-overlap_x);
            if (m_state == state::Type::swim) {
                return;
            }

            if (our_velocity.x < 0.0F) m_is_wall_to_left = true;
            if (our_velocity.x > 0.0F) m_is_wall_to_left = false;

            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
                                    
            m_is_on_slope = false;
            if (m_is_hovering or is_locked(key_jump) or m_is_carrying) return;
            //if (m_is_on_ground and !is_pressed(key_up)) return;

            /*if ((m_is_wall_to_left and our_velocity.x > 0.0F) or
                (!m_is_wall_to_left and our_velocity.x < 0.0F)) {                
                return;
            }*/

            if (our_velocity.y > 0.0F) {
                if ((is_pressed(key_left)) or (is_pressed(key_right))) {
                    console::log("hmm\n");
                    if (m_state != state::Type::ledge and our_points.y < other_points.y - 1.0F /*and our_points.y + 16 > other_points.y*/) {
                        m_next_state = state::Type::ledge;
                        position_y(other_points.y - 6);
                        velocity_y(0.0F);
                    }
                    else if (m_state == state::Type::run or m_state == state::Type::shoot) {
                        if (!m_is_on_ground) {
                            m_next_state = state::Type::slide_wall;
                            sprite_is_leftward(!m_is_wall_to_left);
                        }
                    }                    
                }
            }            
        }
        else if (other_type == Type::clip_L) {
            if (our_velocity.x < 0.0F or m_state == state::Type::ledge) return;
            position_add_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = false;
            
            m_is_on_slope = false;
            
            if (m_is_on_ground or m_is_carrying or m_is_hovering) return;

            if (our_velocity.y > 0.0F and is_pressed(key_right)) {
                if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                    m_next_state = state::Type::slide_wall;
                    sprite::is_leftward(m_sprite, true);
                }
            }            
        }
        else if (other_type == Type::clip_LD) {
            if ((/*other_points.h < our_points.y and*/ m_is_on_ground) or m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (our_velocity.x < 0.0F and our_velocity.x < -m_slide_ground_max_velocity.x or
                        our_velocity.x > 0.0F and our_velocity.x > m_slide_ground_max_velocity.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (our_velocity.x <= 0.0F or m_state == state::Type::ledge) return;

            position_add_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = false;
            
            m_is_on_slope = false;
            
            if (m_is_on_ground or m_is_carrying or m_is_hovering) return;

            if (our_velocity.y > 0.0F and is_pressed(key_right)) {
                if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                    m_next_state = state::Type::slide_wall;
                    sprite::is_leftward(m_sprite, true);
                }
            } 
        }
        else if (other_type == Type::clip_R) {
            if (our_velocity.x >= 0.0F or m_state == state::Type::ledge) return;
            position_add_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_wall_to_left = true;
            
            m_is_on_slope = false;
            if (m_is_on_ground or m_is_carrying or m_is_hovering) return;
            if (our_velocity.y > 0.0F and is_pressed(key_left)) {
                if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                    m_next_state = state::Type::slide_wall;
                    sprite::is_leftward(m_sprite, false);
                }
            }            
        }
        else if (other_type == Type::clip_RD) {
            if ((/*other_points.h < our_points.y and*/ m_is_on_ground) or m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                if (!is_pressed(key_down)) {
                    if (velocity().x < 0.0F and velocity().x < -m_slide_ground_max_velocity.x or
                        velocity().x > 0.0F and velocity().x > m_slide_ground_max_velocity.x) {
                        velocity_x(velocity().x * 0.5F);
                    }
                }
                return;
            }

            if (our_velocity.x >= 0.0F or m_state == state::Type::ledge) return;

            position_add_x(-overlap_x);            
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            
            m_is_wall_to_left = true;
            m_is_on_slope = false;
            
            if (m_is_on_ground or m_is_carrying or m_is_hovering) return;

            if (our_velocity.y > 0.0F and is_pressed(key_left)) {
                if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                    m_next_state = state::Type::slide_wall;
                    sprite::is_leftward(m_sprite, false);
                }
            }            
        }
        else if (is_conduit(other_type)) {
            return;
            if (other.owner->is_dead()) return;
            position_add_x(-overlap_x);
            velocity_x(0.0F);
            moved_velocity_x(0.0F);
        }
        else if (other_type == Type::coin) {
            if (other.owner->is_dead()) return;
            
            sound::position(other.owner->sound("dead"), { (other_points.x + 4.0F) / app::config::extent().x / 2.0F,
                                                             (other_points.y + 4.0F) / app::config::extent().y / 2.0F });
            sound::play(other.owner->sound("dead"));
            
            other.owner->time_left_alive(0);
            other.owner->time_left_dead(U16_MAX);

            console::log(class_name(), "::collide_x() coin: ", other_number, "\n");

            sheet::game::add_picked_coin(other_number);
        }
        else if (other_type == Type::frog) {
            if (m_state == state::Type::sling) {
                console::log("Player::collide_x() frog state: sling, nah\n");
                return;
            }

            //if (our_points.h + 1.0F <= other_points.y) return;
            //velocity_x(moved_velocity_x(0.0F);

            position_add_x(-overlap_x);

            if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                return;
            }

            velocity_x(other_velocity.x * 0.5F);
            other.owner->velocity_x(our_velocity.x * 0.25f);
            if (our_points.x < other_points.x) {
                velocity_add({ -1.0F, 0.0F });
                other.owner->velocity_add({ 1.0F, 0.0F });
            }
            else {
                velocity_add({ 1.0F, 0.0F });
                other.owner->velocity_add({ -1.0F, 0.0F });
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
            //position_add_x( -overlap_x );
            //velocity().x *= 0.95F;
        }
        else if (other_type == Type::particle_health) { }
        else if (other_type == Type::particle_melee) { }
        else if (other_type == Type::particle_shot) {
            //console::log("\n\nentity::Player::collide_x() ", to_string(other_type), "\n\n");
            hurt(other.owner);
        }
        else if (other_type == Type::slope_L_1x1) {
            return;
            if (our_points.h > other_points.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == state::Type::swim) {
                if (our_velocity.x > 0.0F and std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95f);
                    position_add({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (our_velocity.x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            //velocity_y(-velocity().x;
            //position().y -= 1.0F;
            position_add({ 0.0F, -velocity().x });

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            return;
            if (our_points.h > other_points.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == state::Type::swim) {
                if (our_velocity.x < 0.0F and std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.9F);
                    velocity_x(velocity().x * 0.95F);
                    position_add({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //position_add_x( -overlap_x );
            //velocity_y(0.0F);
            if (our_velocity.x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            //velocity_y(velocity().x);
            //position().y -= 1.0F;
            position_add({ 0.0F, velocity().x });
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            return;
            if (our_points.h > other_points.h) return;

            //if (velocity().x < 0.0F) return;
            if (m_state == state::Type::swim) {
                if (our_velocity.x > 0.0F and std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(-our_velocity.x * 0.5F);
                    velocity_x(our_velocity.x * 0.95F);
                    position_add({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            //velocity_y(-velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            position_add({ 0.0F, -velocity().x / 2.0F });

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            return;
            if (our_points.h > other_points.h) return;

            //if (velocity().x > 0.0F) return;
            if (m_state == state::Type::swim) {
                if (our_velocity.x < 0.0F and std::abs(our_velocity.y) < our_velocity.x) {
                    velocity_y(our_velocity.x * 0.5F);
                    velocity_x(velocity().x * 0.95f);
                    position_add({ 0.0F, -1.0F });
                }
                return;
            }
            //position().y -= overlap_x;
            //position_add_x( -overlap_x );
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            //velocity_y(velocity().x / 2.0F;
            //velocity().y -= 0.5F;
            position_add({ 0.0F, velocity().x / 2.0F });

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
            position_add_x(-overlap_x);
            //velocity_x(0.0F);

            //velocity_x(velocity().x * 0.5F + velocity.x * 0.5F;

            velocity_x(velocity().x * 0.75F + other_velocity.x * 0.25f);
            //velocity_y(velocity().y * 0.5F + velocity.y * 0.5F;
        }
        else if (is_water_line(other_type)) {
            collide_y(our, other);
        }

    }
}