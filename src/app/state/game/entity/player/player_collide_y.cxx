module entity.player;
import app.config;
import console;
import particle_system;

namespace entity {
    void Player::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };

        cVec2F our_extent = { our_rect.w - our_rect.x, our_rect.h - our_rect.y };

        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cVec2F other_extent = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };
        /*if (velocity().y > 0.0F and our_rect.h - 4.0F - velocity().y > other_rect.y) {
            return;
        }*/
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);
        aabb::cName   our_name = aabb::name(our.id);

        //if (other_rect.y < our_rect.y and velocity().y >= 0.0F or
            //other_rect.y > our_rect.y and velocity().y <= 0.0F) return;

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        //console::log(class_name(), "::collide_y() type: ", to_string(other_type), " bounce time: ", (I32)m_time_left_bouncing, "\n");

        // landing on ground above water line from a dive
        if (m_state == state::Type::dive) {
            if (position_on_level().y <= m_water_line_y and velocity().y >= 0.0F) {
                if (other_type == Type::clip or other_type == Type::clip_U or other_type == Type::clip_ledge or
                    is_slope(other_type)) {
                    console::log("Player::collide_y ground above water line\n");
                    m_next_state = state::Type::run;
                    m_is_on_ground = true;
                    velocity_y(0.0F);
                }
                else if (other_type == Type::bug or other_type == Type::frog) {
                    m_next_state = state::Type::run;
                    other.owner->velocity_y(our_velocity.y);
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                    m_is_on_ground = true;
                }
            }
        }


        if (velocity().y < 0.0F and m_time_left_ducking == 0) {
            if (m_state != state::Type::swim and our_rect.y > other_rect.h - 2.0F) {
                if (is_arch(other_type) or
                    other_type == Type::clip or
                    other_type == Type::clip_D or
                    other_type == Type::clip_LD or
                    other_type == Type::clip_RD or
                    other_type == Type::clip_ledge) {
                    sound_position("bump_head", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("bump_head");
                }
            }
        }

        if (m_state == state::Type::run and our_rect.y < other_rect.y) {
            if (!m_is_on_ground and !m_is_on_slope and velocity().y > 1.0F and
                (is_clip(other_type) or
                    is_slope(other_type))
                ) {
                particle::spawn({ .parent = this,
                                  .type = particle::Type::dust,
                                  .position = Vec2F{ our_rect.x + 8.0F, other_rect.y },
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        }

        if (m_state == state::Type::sling and (is_clip(other_type) or is_arch(other_type))) {
            if (!sound_is_playing("bump_head")) {
                sound_stop("bump_head");
                sound_position("bump_head", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("bump_head");

                m_rotation_speed *= -1.0F;
                //position_add_y( -overlap_y );
                //velocity_y(-velocity().y;
            }
            return;
        }
        if (is_arch(other_type)) {
            //if (velocity().y > 0.0F) return;

            if (m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                return;
            }
            if (m_state == state::Type::swim) {
                moved_velocity_y(0.0F);
                console::log(class_name(), "::collide_y() swim arch\n");
                if (other_type == Type::arch_L_1x1)
                    position_add({ -0.5F, 0.0F });                    
                else if (other_type == Type::arch_R_1x1)
                    position_add({  0.5F, 0.0F });                    
                else if (other_type == Type::arch_L_2x1_0 or other_type == Type::arch_L_2x1_1)
                    position_add({ -0.5F, 0.0F });
                else if (other_type == Type::arch_R_2x1_0 or other_type == Type::arch_R_2x1_1)
                    position_add({  0.5F, 0.0F });

                position_add({ 0.0F, 0.5F });
                return;
            }
            if (m_is_on_ground) {
                m_time_left_ducking = m_time_to_duck;
            }
            if (our_rect.y < other_rect.y or m_time_left_ducking > 0) {
                return;
            }
            position_add_y(-overlap_y);
            velocity_y(std::abs(velocity().y) * 0.75F);

            if (other_type == Type::arch_L_1x1) {
                velocity_add({ -velocity().y, 0.0F });
            }
            else if (other_type == Type::arch_R_1x1) {
                velocity_add({ velocity().y, 0.0F });
            }
            else if (other_type == Type::arch_L_2x1_0 or other_type == Type::arch_L_2x1_1) {
                velocity_add({ velocity().y * -0.5F, 0.0F });
            }
            else if (other_type == Type::arch_R_2x1_0 or other_type == Type::arch_R_2x1_1) {
                velocity_add({ velocity().y * 0.5F, 0.0F });
            }
        }

        if (is_slope(other_type) and other_type != Type::slope_U and m_state == state::Type::swim) {            
            moved_velocity_y(0.0F);

            if (other_type == Type::slope_L_1x1) {
                position_add({ -0.5F, 0.0F });
            }
            else if (other_type == Type::slope_R_1x1) {
                position_add({ 0.5F, 0.0F });
            }
            else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
                position_add({ -0.5F, 0.0F });
            }
            else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
                position_add({ 0.5F, 0.0F });
            }
            position_add({ 0.0F, -0.5F });   
            
            return;
        }
        else if (other_type == Type::brick) {
            /*if (velocity().y < 0.0F and our_rect.y < other_rect.h - 2.0F or velocity().y > 0.0F and our_rect.h > other_rect.y + 2.0F) {
                collide_x(our, other);
                return;
            }*/

            if (our_rect.h < other_rect.y + 4.0F and our_velocity.y > 0.0F) {
                if (our_velocity.y >= 6.0F) {
                    other.owner->hurt(this);
                    return;
                }
                m_is_on_ground = true;
                moved_velocity(other_velocity);
                velocity_y(0.0F);
                position_add_y(-overlap_y);
            }


            /*if (our_rect.h < other_rect.y + 2.0F and velocity().y > 0.0F) {
                velocity_y(0.0F);
                m_is_on_ground = true;
            } else if (our_rect.y > other_rect.y - 2.0F and velocity().y < 0.0F) {
                velocity_y(velocity().y * -0.9F);
            }*/
        }
        else if (other_type == Type::bug) {
            if (other.owner->parent() == this or other.owner->is_dead() or other.owner->is_interacting()) return;
            //if (m_state != state::Type::run) return;

            if (other.owner->state() == state::Type::swim) {
                if ((m_state == state::Type::swim or m_state == state::Type::dive) and velocity().y < 0.0F and our_rect.h > other_rect.h) {
                    velocity_y(our_velocity.y * -1.0F);
                    position_add_y(-overlap_y);

                    position_add_y(8.0F);

                    m_next_state = state::Type::swim;

                    //time_left_colliding_with(Type::water_line, 10);

                    other.owner->max_velocity_y(10.0F);
                    other.owner->position_add_y(-4.0F);
                    other.owner->velocity_y(our_velocity.y * 1.0F);
                    other.owner->velocity_x(our_velocity.x * 0.9F);

                    other.owner->num_jumps(1);
                }
                else if (velocity().y > 0.0F and our_rect.y < other_rect.y) {

                    other.owner->velocity_y(our_velocity.y * 0.4F);

                    if (std::abs(our_velocity.y) > 1.0F) {
                        other.owner->velocity_x(our_velocity.x * 0.5F);
                    } else {
                        other.owner->velocity_x(our_velocity.x * -0.2F);
                    }

                    if (m_is_down_thrusting) {
                        other.owner->hurt(this);

                        console::log(class_name(), "collide_y() vel y: ", our_velocity.y, "\n");
                        
                        velocity_y(-4.0F);
                        moved_velocity_y(0.0F);

                        reset_anim("jump_spin");
                        m_is_down_thrusting = false;
                        m_is_on_ground = false;
                        m_is_on_slope = false;
                        m_num_jumps = 1;
                        m_time_left_bouncing = m_time_to_bounce;

                        //other.owner->next_state(state::Type::bounce);
                        
                        return;
                    }

                    //other.owner->moved_velocity_y(our_velocity.y;
                    velocity_y(0.0F);
                    moved_velocity_x(other_velocity.x);
                    moved_velocity_y(other_velocity.y);
                    position_add_y(-overlap_y);
                    m_is_on_ground = true;
                    console::log(class_name(), "::collide_y() swim bug ", our_velocity.y, "\n");
                    /*if (velocity().y < 0.0F) {
                        velocity_y(velocity().y * -1.0F);
                    }*/
                }
                return;
            }

            /*if (our_rect.y > other_rect.h) {
                return;
            }*/

            if (our_rect.y + 4.0F >= other_rect.y) {   // bug above
                Vec2F other_vel = other.owner->velocity() + other.owner->moved_velocity();
                other.owner->velocity_y(velocity().y / 4.0F - 3.0F);
                other.owner->position_add_y(-8.0F);

                cF32 our_mid_x = our_rect.w - our_rect.x / 2.0F;
                cF32 other_mid_x = other_rect.w - other_rect.x / 2.0F;

                cF32 mid_diff_x = other_mid_x - our_mid_x;
                other.owner->velocity_x((velocity().x + moved_velocity().x) + mid_diff_x / 4.0F);

                //console::log("mid diff: ", , "\n");
                // 
                //velocity() = vel / 2.0F;

                cF32 v = other.owner->velocity().y;

                //hurt();
                console::log(class_name(), "::collide_y() bug above\n");

                return;
            }
            
            if (velocity().y > 2.0F and m_time_left_bouncing == 0 and
                (other.owner->state() == state::Type::upended or other.owner->state() == state::Type::bounce)) {
                position_add_y(-overlap_y);
                console::log("Player::collide_y() bug vel: ", velocity().y, "\n");
                max_velocity_y(10.0F);
                velocity_y(-5.0F - velocity().y / 4.0F);
                moved_velocity_y(0.0F);
                m_is_down_thrusting ? reset_anim("jump_spin") : reset_anim("jump");
                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;
                m_time_left_bouncing = m_time_to_bounce;

                other.owner->hurt(this);
                other.owner->next_state(state::Type::bounce);
                return;
            }
            if (our_rect.h < other_rect.y + 2.0F) {
                position_add_y(-overlap_y + 1.0F);
                moved_velocity(other_velocity);
                //velocity_x(velocity().x * 0.5F);
                velocity_y(0.0F);
                m_is_on_ground = true;
                console::log(class_name(), "::collide_y() bug below\n");
            }            
        }
        else if (other_type == Type::clip) {
            position_add_y(-overlap_y);

            if (m_state == state::Type::swim) {
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
                return;
            }

            if (velocity().y > 0.0F and our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
            } else {
                velocity_y(std::abs(velocity().y) * 0.75F);   
            }
        }
        else if (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            if (m_state == state::Type::swim and our_rect.h > other_rect.h) {
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
                position_add_y(-overlap_y);                
                return;
            }
            if ((other_rect.y < our_rect.y and m_is_on_ground) or m_is_climbing_ledge) {
                m_time_left_rising = m_time_to_rise;
                m_time_left_ducking = m_time_to_duck;
                return;
            }
            if (m_time_left_rising > 0) {
                return;
            }
            if (our_rect.y < other_rect.h - 2.0F) return;

            position_add_y(-overlap_y);

            if (our_rect.y > other_rect.y) {
                m_is_on_ground = false;
                m_is_on_slope = false;
                                
                velocity_y(std::abs(velocity().y) * 0.75F);                
            }
        }
        else if (other_type == Type::clip_L) { }
        else if (other_type == Type::clip_R) { }
        else if (other_type == Type::clip_ledge) {
            if (velocity().y < 0.0F and velocity().y > -10.0F and our_rect.y < other_rect.h - 4.0F or
                velocity().y > 0.0F and velocity().y <  10.0F and our_rect.h > other_rect.y + 4.0F) {
                collide_x(our, other);
                return;
            }
            if ((velocity().y > 0.0F and our_rect.h > other_rect.y + 2.0F) or
                (velocity().y < 0.0F and our_rect.y < other_rect.h - 2.0F)) {                
                return;
            }

            /*console::log(" ", our_rect.y, " ", our_rect.h, "  ", other_rect.y, " ", other_rect.h, "\n");*/
            position_add_y(-overlap_y);
            if (m_state == state::Type::swim) {                                    
                return;                
            }

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                moved_velocity_x(0.0F);
                velocity_y(0.0F);
                moved_velocity_y(0.0F);
            }
            else if (our_rect.h > other_rect.h) {
                velocity_y(std::abs(velocity().y) * 0.75F);
            }
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::coin) {
            collide_x(our, other);
        }
        else if (is_conduit(other_type)) { }
        else if (other_type == Type::frog) {
            if (m_state == state::Type::sling) {
                collide_x(our, other);
                return;
            }
            if (our_rect.y < other_rect.y + 4.0F and our_velocity.y > 0.0F) {
                position_add_y(-overlap_y);

                //console::log("Player::collide_y frog\n");
                if (our_velocity.y >= 6.0F) {
                    other.owner->hurt(this);

                    reset_anim("jump_spin");
                    m_is_down_thrusting = false;
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    m_num_jumps = 1;
                    //velocity_y(velocity().y * -0.9F);
                    velocity_y(velocity().y * -0.7F);
                } else {
                    m_is_on_ground = true;
                    m_is_on_slope = false;
                    velocity_y(0.2F);
                    moved_velocity(other_velocity);
                }
            }
            else {
                //console::log("Player::collide_y frog above\n");
            }

            //console::log("other vx: ", other_velocity.x, " ", our.owner->moved_velocity().x, "\n");        

            //if (our_rect.y < other_rect.y) {
            //    position_add_y( -overlap_y );// / 4.0F;

            //    if (velocity().y >= 9.0F) {
            //        //bounce
            //        velocity_y(velocity().y * -0.9F);
            //        moved_velocity_y(0.0F);
            //        return;
            //    }
            //    velocity_y(0.2F;
            //    

            //    moved_velocity(other_velocity);
            //    m_is_on_ground = true;
            //    m_is_on_slope = false;
            //}
            //else {
            //    //position_add_y( -overlap_y );
            //}
        }
        else if (other_type == Type::level_L_0 or other_type == Type::level_R_0) {
            collide_x(our, other);
        }
        else if (other_type == Type::level_center) {
            collide_x(our, other);
        }
        else if (other_type == Type::mole) {
            if (other.owner->state() == state::Type::exit) {
                max_velocity_y(10.0F);
                velocity_y(-7.0F);

                m_is_carrying ? reset_anim("jump") : reset_anim("jump_spin");

                m_is_down_thrusting = false;
                m_is_on_ground = false;
                m_is_on_slope = false;
                m_num_jumps = 1;
                m_time_left_bouncing = m_time_to_bounce;

                if (!sound_is_playing("jump")) {
                    sound_position("jump", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("jump");
                }
            }
            else if (other.owner->state() == state::Type::swim or other.owner->state() == state::Type::jump) {
                if (velocity().y < 0.0F and our_rect.h > other_rect.h) {
                    if (position_on_level().y < m_water_line_y - 4.0F) {
                        m_next_state = state::Type::dive;
                    } else {
                        m_next_state = state::Type::swim;
                    }

                    velocity_y(std::abs(our_velocity.y) * 0.5F + std::abs(other_velocity.y) * 0.5F);
                    position_add_y(-overlap_y);

                    position_add_y(8.0F);

                    other.owner->position_add_y(-4.0F);
                    other.owner->velocity_y(our_velocity.y * 0.5F);
                    other.owner->velocity_x(our_velocity.x * 0.5F);

                    other.owner->next_state(state::Type::jump);
                    //other.owner->time_left_colliding_with(Type::water_line, 10);
                }
                else if (velocity().y > 0.0F and our_rect.y < other_rect.y) {
                    other.owner->velocity_y(our_velocity.y * 0.4F);

                    if (std::abs(our_velocity.y) > 1.0F) {
                        other.owner->velocity_x(our_velocity.x * 0.5F);
                    }
                    else {
                        other.owner->velocity_x(our_velocity.x * -0.2F);
                    }
                    velocity_y(0.0F);
                    moved_velocity_x(other_velocity.x);
                    moved_velocity_y(other_velocity.y);
                    position_add_y(-overlap_y);
                    m_is_on_ground = true;
                    console::log(class_name(), "::collide_y() swim mole ", our_velocity.y, "\n");                    
                }                
            }
        }
        else if (other_type == Type::particle_brick) {
            if (our_rect.y < other_rect.y) {
                //position_add_y( -overlap_y );
                //velocity_y(0.0F);
                //moved_velocity(other_velocity);
                //m_is_on_ground = true;
            }
        }
        else if (other_type == Type::particle_shot) {
            console::log("\n\nentity::Player::collide_y() ", to_string(other_type), "\n\n");
            hurt(other.owner);
        }
        else if (other_type == Type::player) {
            return;
            position_add_y(-overlap_y);

            //console::log("overlap_y: ", overlap_y, "\n");

            if (our_rect.y < other_rect.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
                //time_left_holding_jump = time_to_hold_jump;

                moved_velocity(other_velocity);
                velocity_y(0.0F);
                //position().y -= 1.0F;

                //console::log("velocity: ", velocity.x, " ", velocity.y, "\n");
                //velocity() = { 0.0F, 0.0F };
                //velocity().x -= (velocity().x - velocity.x);
            } else {
                //object->is_on_ground = true;

                //object->moved_velocity() = velocity();
                //if (!is_on_ground and velocity().y < 0.0F) {
                //    velocity_y(velocity().y * -1.0F);
                //}
                //moved_velocity(other_velocity);
                //velocity_y(0.0F);
            }


            //velocity() = { 0.0F, 0.0F };


            //velocity_y(velocity.y;
            //velocity_y(0.0F);
        }
        else if (other_type == Type::slope_L_1x1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            if (m_state == state::Type::swim) {
                if (velocity().y > 0.0F and std::abs(velocity().x) < velocity().y) {
                    velocity_x(-velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F);
            }
            //if (velocity().x < 0.0F) {
            //velocity_y(-velocity().x);
            //}

            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            if (m_state == state::Type::swim) {
                if (velocity().y > 0.0F and std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F);
            }
            //if (velocity().x > 0.0F) {
            //velocity_y(velocity().x);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            if (m_state == state::Type::swim) {
                if (velocity().y > 0.0F and std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * -0.5F);
                    velocity_y(velocity().y * 0.95f);
                }
                return;
            }
            if (velocity().x > max_velocity().x * 0.9F) {
                velocity_x(max_velocity().x * 0.9F);
            }
            //if (velocity().x < 0.0F) {
            //velocity_y(velocity().x / -2.0F);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            if (m_state == state::Type::swim) {
                if (velocity().y > 0.0F and std::abs(velocity().x) < velocity().y) {
                    velocity_x(velocity().y * 0.5F);
                    velocity_y(velocity().y * 0.95F);
                }
                return;
            }
            if (velocity().x < -max_velocity().x * 0.9F) {
                velocity_x(-max_velocity().x * 0.9F);
            }
            //if (velocity().x > 0.0F) {
            //velocity_y(velocity().x / 2.0F);
            //}
            velocity_y(0.0F);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::spring_U) {
            if (velocity().y < 0.0F) return;
            if (m_state == state::Type::swim) {
                m_next_state = state::Type::run;
            }
            position_add_y(-overlap_y);
            max_velocity_y(10.0F);
            velocity_y(-5.0F - velocity().y / 4.0F);
            m_is_down_thrusting ? reset_anim("jump_spin") : reset_anim("jump");
            m_is_down_thrusting = false;
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_num_jumps = 1;
            m_time_left_bouncing = m_time_to_bounce;
        }
        else if (other_type == Type::train) {            
            //console::log("hello train ", aabb::name_as_string(other_name), "\n");
            if (velocity().y < 0.0F or other_name == aabb::Name::bone or
                //our_rect.h > other_rect.h
                //or
                position().y + 20 > other_rect.y
                ) {
                return;
            }

            moved_velocity(other_velocity);            
            velocity_y(1.0F);
            position_add({ 0.0F, -overlap_y / 6.0F }); // so we don't fly off at turns
            //} else {
                //velocity_y(0.0F);
                //position_add_y( -overlap_y );
            //}
            m_is_on_ground = true;

        }
        else if (is_water_line(other_type)) {            
            //next_state(state::Type::swim);
            //other.owner->time_left_interacting(3);
            
            if (std::abs(velocity().y) > 1.0F) {
                cF32 vel_x = velocity().x + moved_velocity().x;
                if (other_rect.x + other_extent.x / 2.0F + vel_x < our_rect.x + our_extent.x / 2.0F - 2.0F) {
                    //other.owner->force_x(velocity().x / 2.0F - std::abs(velocity().y));
                    other.owner->force_x(-1.0F + velocity().x / max_velocity().x);
                }
                else if (other_rect.x + other_extent.x / 2.0F + vel_x > our_rect.x + our_extent.x / 2.0F + 2.0F) {
                    //other.owner->force_x(velocity().x / 2.0F + std::abs(velocity().y));
                    other.owner->force_x(1.0F + velocity().x / max_velocity().x);
                }
            }
            //console::log(class_name(), "::collide_y() force_x: ", other.owner->force().x, "\n");

            bool is_to_splash = false,
                 is_upwards   = false;

            //other.owner->velocity_x(/*other.owner->moved_velocity().x * 0.5F + */velocity().y);
            
            //m_water_line_y = other.owner->position_on_level().y;
            m_water_line_y = other.owner->start_position_on_level().y;

            if (velocity().y > 0.0F and m_state != state::Type::swim) {
                if (our_rect.h <= other_rect.y + 4.0F) return;
                is_upwards = false;
                if (velocity().y > 1.0F and m_time_in_state != 0) {
                    m_time_in_state = 0;
                    if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                        sound_position("water_enter", { (position().x + 8.0F) / app::config::extent().x / 2.0F, (position().y + 8.0F) / app::config::extent().y / 2.0F });
                        sound_play("water_enter");
                        is_to_splash = true;
                    }
                }
                
                //max_velocity({ 10.0F, 10.0F }); 

                if (velocity().y < 1.0F) {
                    velocity_y(1.0F);
                }

                //velocity_y(velocity().y + std::abs(cos));
                //if (m_next_state != state::Type::swim) {
                    m_next_state = state::Type::swim;
                    
                    if (m_state == state::Type::run) {
                        cVec2F vel_limit = m_is_down_thrusting ? Vec2F{ 7.0F, 7.0F } : Vec2F{ 2.0F, 2.0F };
                        if (velocity().x < -vel_limit.x) {
                            velocity_x(-vel_limit.x);
                        }
                        else if (velocity().x > vel_limit.x) {
                            velocity_x(vel_limit.x);
                        }
                        if (velocity().y > vel_limit.y) {
                            velocity_y(vel_limit.y);
                        }
                        //console::log(class_name(), "::collide_y() rotation: ", sprite::rotation(m_sprite_id), " ", cos, "\n");
                    }
                //}
                
            }
            else if (velocity().y < 0.0F and m_state == state::Type::swim) {
                //if (our_rect.y + 4.0F > other_rect.h) return;

                if (velocity().y >= -1.0F or is_pressed(key_jump)) {
                    velocity_y(std::abs(velocity().y * 0.2F));
                    if (velocity().y < 1.0F) {
                        velocity_y(1.0F);
                    }
                    return;
                }

                //console::log("is_pressed(key_jump): ", is_pressed(key_jump), "\n");
                if (velocity().y <= -1.0F and !is_pressed(key_jump) ) {
                    is_upwards = true;
                    if (!sound_is_playing("water_exit") and !sound_is_playing("water_enter") and m_state != state::Type::dive) {
                        sound_position("water_exit", { (position().x + 8.0F) / app::config::extent().x / 2.0F, (position().y + 8.0F) / app::config::extent().y / 2.0F });
                        sound_play("water_exit");
                        is_to_splash = true;
                    }
                    
                    if (m_next_state != state::Type::dive) {
                        max_velocity({ 10.0F, 10.0F });

                        velocity_y(velocity().y * 1.2F);
                        //velocity_y(velocity().y + other_velocity.y * 1.0F);
                        console::log(class_name(), "::collide_y() vel y : ", velocity().y, "\n\n");

                        if (velocity().y > -5.0F) {
                            velocity_y(-5.0F);
                        }
                        m_next_state = state::Type::dive;
                        m_time_in_state = 0;
                    }
                }
                else {
                    //velocity_y(1.0F);
                    m_time_in_state = 0;
                    is_to_splash = false;
                }
            }

            m_is_on_ground = false;
            m_is_down_thrusting = false;

            if (is_to_splash) {
                console::log(class_name(), "::collide_y() is upwards: ", is_upwards, " velocity y: ", velocity().y, "\n");
                
                cVec2F pos = Vec2F{ our_rect.x + 4.0F, other_rect.y } + Vec2F{ 0.0F, is_upwards ? -10.0F : 0.0F };
                cF32 speed = is_upwards ? 1.2F : 1.0F;

                particle::splash_water(this, pos, velocity(), speed);

            }

        }
    }
}