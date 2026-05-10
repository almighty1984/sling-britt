module entity.player;
import app.config;
import console;
import particle_system;

namespace entity {
    void Player::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner->parent() == this) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };
        /*if (velocity().y > 0.0F and our_DR.y - 4.0F - velocity().y > other_UL.y) {
            return;
        }*/
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();
        cVec2F our_velocity = velocity() + moved_velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);
        aabb::cName   our_name = aabb::name(our.id);

        state::cType other_state = other.owner->state();


        if (time_left_colliding_with(other_type) > 0) return;

        //if (other_UL.y < our_UL.y and velocity().y >= 0.0F or
            //other_UL.y > our_UL.y and velocity().y <= 0.0F) return;

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

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


        if (our_velocity.y < 0.0F and m_state != state::Type::duck and m_state != state::Type::swim) {
            if (our_UL.y > other_DR.y - 4.0F) {
                if (is_arch(other_type)         or
                    other_type == Type::clip    or
                    other_type == Type::clip_D  or
                    other_type == Type::clip_LD or
                    other_type == Type::clip_RD) {
                    sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                    sound_pitch("bump_head", 1.0F - our_velocity.y * 0.1F);
                    sound_play("bump_head");
                    m_time_left_rising = 0;
                }
            }
        }

        if ((m_state == state::Type::run or m_state == state::Type::shoot) and our_UL.y < other_UL.y) {
            if (!m_is_on_ground and !m_is_on_slope and velocity().y > 1.0F and
                (is_clip(other_type) or
                    is_slope(other_type))
                ) {
                particle::spawn({ .parent = this,
                                  .type = particle::Type::dust,
                                  .position = Vec2F{ our_UL.x + 8.0F, other_UL.y },
                                  .velocity = Vec2F{ 0.0F, 0.0F } });
            }
        }

        if (m_state == state::Type::sling and (is_clip(other_type) or is_arch(other_type))) {
            if (!sound_is_playing("bump_head")) {
                sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                              position().y - app::config::extent().y / 2.0F });
                sound_pitch("bump_head", 1.0F - velocity().y * 0.1F);
                sound_play("bump_head");

                m_rotation_speed *= -1.0F;
                //position_add_y( -overlap_y );
                //velocity_y(-velocity().y;
            }
            return;
        }

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (m_state == state::Type::climb) {
                    return;
                }
                if (m_state == state::Type::swim) {
                    moved_velocity_y(0.0F);
                    console::log(class_name(), "::collide_y() swim arch\n");

                    switch (other_type) {
                        case Type::arch_L_1x1:   position_add({ -0.5F, 0.0F }); break;
                        case Type::arch_R_1x1:   position_add({  0.5F, 0.0F }); break;
                        case Type::arch_L_2x1_0:
                        case Type::arch_L_2x1_1: position_add({ -0.5F, 0.0F }); break;
                        case Type::arch_R_2x1_0:
                        case Type::arch_R_2x1_1: position_add({  0.5F, 0.0F }); break;
                    }

                    position_add({ 0.0F, 0.5F });
                    return;
                }
                if (m_is_on_ground) {
                    m_time_left_rising = m_time_to_rise;
                    m_next_state = state::Type::duck;
                }
                if (our_UL.y < other_UL.y or m_state == state::Type::duck) {
                    return;
                }
                position_add_y(-overlap_y);
                velocity_y(std::abs(velocity().y) * 0.75F);

                if (other_type == Type::arch_L_1x1) {
                    velocity_add({ -velocity().y, 0.0F });
                } else if (other_type == Type::arch_R_1x1) {
                    velocity_add({ velocity().y, 0.0F });
                } else if (other_type == Type::arch_L_2x1_0 or other_type == Type::arch_L_2x1_1) {
                    velocity_add({ velocity().y * -0.5F, 0.0F });
                } else if (other_type == Type::arch_R_2x1_0 or other_type == Type::arch_R_2x1_1) {
                    velocity_add({ velocity().y * 0.5F, 0.0F });
                }
                break;
            }
            /*case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (m_state == state::Type::swim) {
                    moved_velocity_y(0.0F);

                    if (other_type == Type::slope_L_1x1) {
                        position_add({ -0.5F, 0.0F });
                    } else if (other_type == Type::slope_R_1x1) {
                        position_add({ 0.5F, 0.0F });
                    } else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
                        position_add({ -0.5F, 0.0F });
                    } else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
                        position_add({ 0.5F, 0.0F });
                    }
                    position_add({ 0.0F, -0.5F });
                }
                break;
            }*/
            case Type::brick: {
                /*if (velocity().y < 0.0F and our_UL.y < other_DR.y - 2.0F or velocity().y > 0.0F and our_DR.y > other_UL.y + 2.0F) {
                    collide_x(our, other);
                    return;
                }*/
                if (our_DR.y < other_UL.y + 4.0F and our_velocity.y > 0.0F) {
                    if (our_velocity.y >= 6.0F) {
                        other.owner->hurt(this);
                        return;
                    }
                    m_is_on_ground = true;
                    moved_velocity(other_velocity);
                    velocity_y(0.0F);
                    position_add_y(-overlap_y);
                }
                /*if (our_DR.y < other_UL.y + 2.0F and velocity().y > 0.0F) {
                    velocity_y(0.0F);
                    m_is_on_ground = true;
                } else if (our_UL.y > other_UL.y - 2.0F and velocity().y < 0.0F) {
                    velocity_y(velocity().y * -0.9F);
                }*/
                break;
            }
            case Type::bug: {
                if (other.owner->parent() == this or other.owner->is_dead() or other.owner->is_interacting()) return;
                //if (m_state != state::Type::run) return;

                if (other.owner->state() == state::Type::swim) {
                    if ((m_state == state::Type::swim or m_state == state::Type::dive) and velocity().y < 0.0F and our_DR.y > other_DR.y) {
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
                    } else if (our_velocity.y > 0.0F and our_UL.y < other_UL.y) {
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

                /*if (our_UL.y > other_DR.y) {
                    return;
                }*/

                if (our_UL.y + 4.0F >= other_UL.y) {   // bug above
                    Vec2F other_vel = other.owner->velocity() + other.owner->moved_velocity();
                    other.owner->velocity_y(velocity().y / 4.0F - 3.0F);
                    other.owner->position_add_y(-8.0F);

                    cF32 our_mid_x = our_DR.x - our_UL.x / 2.0F;
                    cF32 other_mid_x = other_DR.x - other_UL.x / 2.0F;

                    cF32 mid_diff_x = other_mid_x - our_mid_x;
                    other.owner->velocity_x((velocity().x + moved_velocity().x) + mid_diff_x / 4.0F);

                    cF32 v = other.owner->velocity().y;

                    //hurt();
                    console::log(class_name(), "::collide_y() bug above\n");
                    return;
                }

                if (our_velocity.y > 2.0F and m_time_left_bouncing == 0 and
                    (other_state == state::Type::upended or other_state == state::Type::bounce)) {
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
                if (our_DR.y < other_UL.y + 4.0F) {
                    position_add_y(-overlap_y + 1.0F);
                    moved_velocity(other_velocity);
                    //velocity_x(velocity().x * 0.5F);
                    velocity_y(0.0F);
                    m_is_on_ground = true;
                    console::log(class_name(), "::collide_y() bug below\n");
                }
                break;
            }
            case Type::clip:
            case Type::clip_UD: {
                if (our_UL.y > other_UL.y and (m_is_on_ground or m_state == state::Type::climb or m_state == state::Type::duck)) {
                    m_next_state = state::Type::duck;
                    m_time_left_rising = m_time_to_rise;
                    return;
                }

                position_add_y(-overlap_y);

                if (m_state == state::Type::swim) {
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                    return;
                }

                if (velocity().y > 0.0F and our_UL.y < other_UL.y) {
                    m_is_on_ground = true;
                    m_is_on_slope = false;
                    moved_velocity_x(0.0F);
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                } else {
                    velocity_y(std::abs(velocity().y) * 0.75F);
                }
                break;
            }
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_D: {
                if (our_velocity.y > 0.0F) return;
                if (m_state == state::Type::swim and our_DR.y > other_DR.y) {
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                    position_add_y(-overlap_y);
                    return;
                }
                if ((our_DR.y > other_DR.y and m_is_on_ground) or m_state == state::Type::climb) {
                    m_next_state = state::Type::duck;
                    m_time_left_rising = m_time_to_rise;
                    return;
                }
                if (m_time_left_rising > 0) {
                    return;
                }
                //if (our_UL.y < other_DR.y - 2.0F) return;

                position_add_y(-overlap_y);

                if (our_UL.y > other_DR.y - 2.0F) {
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    velocity_y(std::abs(velocity().y) * 0.75F);
                }
                break;
            }
            case Type::clip_ledge: {
                if (our_UL.y > other_UL.y and (m_is_on_ground or m_state == state::Type::climb or m_state == state::Type::duck)) {
                    m_next_state = state::Type::duck;
                    m_time_left_rising = m_time_to_rise;
                    return;
                }

                if (our_velocity.y < 0.0F and our_velocity.y > -10.0F and our_UL.y < other_DR.y - 4.0F or
                    our_velocity.y > 0.0F and our_velocity.y <  10.0F and our_DR.y > other_UL.y + 4.0F) {
                    collide_x(our, other);
                    return;
                }

                if ((our_velocity.y > 0.0F and our_DR.y < other_UL.y + 2.0F) or
                    (our_velocity.y < 0.0F and our_UL.y > other_DR.y - 2.0F)) {                    
                    if (our_DR.x < other_UL.x + 2.0F or our_UL.x > other_DR.x - 2.0F) {
                        //console::log(class_name(), "::collide_y() clip_ledge skip\n");
                        collide_x(our, other);
                        return;
                    }
                }
                /*console::log(" ", our_UL.y, " ", our_DR.y, "  ", other_UL.y, " ", other_DR.y, "\n");*/
                position_add_y(-overlap_y);
                if (m_state == state::Type::swim) {
                    return;
                }
                if (our_velocity.y > 0.0F and our_DR.y - 2.0F < other_UL.y) {
                    m_is_on_ground = true;
                    m_is_on_slope = false;
                    moved_velocity_x(0.0F);
                    velocity_y(0.0F);
                    moved_velocity_y(0.0F);
                } else if (our_velocity.y < 0.0F and our_DR.y > other_DR.y) {
                    velocity_y(std::abs(velocity().y) * 0.75F);

                    sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                    sound_pitch("bump_head", 1.0F - our_velocity.y * 0.1F);
                    sound_play("bump_head");
                }
                break;
            }
            case Type::slope_U:
            case Type::clip_U: {
                if (our_velocity.y < 0.0F) return;
                position_add_y(-overlap_y);

                moved_velocity_x(0.0F);
                moved_velocity_y(0.0F);
                velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = other_type == Type::slope_U;
                break;
            }
            case Type::coin: {
                collide_x(our, other);
                break;
            }
            case Type::frog: {
                if (m_state == state::Type::sling) {
                    collide_x(our, other);
                    return;
                }
                if (our_UL.y < other_UL.y + 4.0F and our_velocity.y > 0.0F) {
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
                } else {
                    //console::log("Player::collide_y frog above\n");
                }

                //console::log("other vx: ", other_velocity.x, " ", our.owner->moved_velocity().x, "\n");        

                //if (our_UL.y < other_UL.y) {
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
                break;
            }
            case Type::level_L_0:
            case Type::level_R_0:
            case Type::level_center: {
                collide_x(our, other);
                break;
            }
            case Type::mole: {
                if (other_state == state::Type::exit) {
                    max_velocity_y(10.0F);
                    velocity_y(-7.0F);

                    m_is_carrying or m_state == state::Type::shoot ? reset_anim("jump") : reset_anim("jump_spin");

                    m_is_down_thrusting = false;
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    m_num_jumps = 1;
                    m_time_left_bouncing = m_time_to_bounce;

                    //m_next_state = state::Type::run;

                    if (!sound_is_playing("jump")) {
                        sound_position("jump", { position().x - app::config::extent().x / 2.0F,
                                                 position().y - app::config::extent().y / 2.0F });
                        sound_play("jump");
                    }
                } else if (other_state == state::Type::swim or other_state == state::Type::jump
                    and (m_state == state::Type::swim or m_state == state::Type::dive)) {
                    if (our_DR.y > other_DR.y) {
                        /*if (position_on_level().y < m_water_line_y - 4.0F) {
                            m_next_state = state::Type::dive;
                        } else {
                            m_next_state = state::Type::swim;
                        }*/

                        //if (m_state == state::Type::dive) {
                            //console::log("this\n\n");
                            //if (position_on_level().y > m_water_line_y) {
                                //console::log("and this\n");
                                //m_next_state = state::Type::swim;
                            //}
                        //}

                        velocity_y(std::abs(our_velocity.y) * 0.5F + std::abs(other_velocity.y) * 0.5F);
                        //position_add_y(-overlap_y);

                        //position_add_y(8.0F);

                        other.owner->position_add_y(-8.0F);
                        other.owner->velocity_y(our_velocity.y * 0.5F);
                        other.owner->velocity_x(our_velocity.x * 0.5F);

                        other.owner->next_state(state::Type::jump);
                        //other.owner->time_left_colliding_with(Type::water_line, 10);
                    } else if (our_velocity.y > 0.0F and our_UL.y < other_UL.y) {
                        other.owner->velocity_y(our_velocity.y * 0.4F);

                        if (std::abs(our_velocity.y) > 1.0F) {
                            other.owner->velocity_x(our_velocity.x * 0.5F);
                        } else {
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
                break;
            }
            case Type::player: {
                return;
                position_add_y(-overlap_y);

                //console::log("overlap_y: ", overlap_y, "\n");

                if (our_UL.y < other_UL.y) {
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
                break;
            }
            case Type::slope_L_1x1: {
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
                break;
            }
            case Type::slope_R_1x1: {
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
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
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
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
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
                break;
            }
            case Type::spring_U: {
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
                break;
            }
            case Type::train: {
                //console::log("hello train ", aabb::name_as_string(other_name), "\n");
                if (our_velocity.y < 0.0F or other_name == aabb::Name::bone
                    or our_DR.y > other_DR.y - 4.0F
                    //or our_DR.y - 4.0F > other_UL.y
                    ) {
                    return;
                }

                moved_velocity(other_velocity);
                velocity_y(1.0F);
                position_add_y(-overlap_y / 8.0F); // move in a bit to prevent flying off
                //} else {
                    //velocity_y(0.0F);
                    //position_add_y( -overlap_y );
                //}
                m_is_on_ground = true;

                if (is_pressed(key_down) and is_pressed(key_jump)) {
                    lock(key_down);
                    lock(key_jump);
                    time_left_colliding_with(other_type, 20);
                    m_is_on_ground = false;
                    set_anim("fall");
                    console::log(class_name(), "::collide_y() drop from train\n");
                }
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                //next_state(state::Type::swim);
                //other.owner->time_left_interacting(3);

                if (std::abs(velocity().y) > 1.0F) {
                    cF32 vel_x = velocity().x + moved_velocity().x;
                    if (other_UL.x + other_extent.x / 2.0F + vel_x < our_UL.x + our_extent.x / 2.0F - 2.0F) {
                        //other.owner->force_x(velocity().x / 2.0F - std::abs(velocity().y));
                        other.owner->force_x(-1.0F + velocity().x / max_velocity().x);
                    } else if (other_UL.x + other_extent.x / 2.0F + vel_x > our_UL.x + our_extent.x / 2.0F + 2.0F) {
                        //other.owner->force_x(velocity().x / 2.0F + std::abs(velocity().y));
                        other.owner->force_x(1.0F + velocity().x / max_velocity().x);
                    }
                }
                //console::log(class_name(), "::collide_y() force_x: ", other.owner->force().x, "\n");

                bool is_to_splash = false,
                    is_upwards = false;

                //other.owner->velocity_x(/*other.owner->moved_velocity().x * 0.5F + */velocity().y);

                //m_water_line_y = other.owner->position_on_level().y;
                m_water_line_y = other.owner->start_position_on_level().y;

                if (velocity().y > 0.0F and m_state != state::Type::swim) {
                    if (our_DR.y <= other_UL.y + 4.0F) return;
                    is_upwards = false;
                    if (velocity().y > 1.0F and m_time_in_state != 0) {
                        m_time_in_state = 0;
                        if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                            sound_position("water_enter", { (position().x + 8.0F) / app::config::extent().x / 2.0F,
                                                            (position().y + 8.0F) / app::config::extent().y / 2.0F });
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
                        } else if (velocity().x > vel_limit.x) {
                            velocity_x(vel_limit.x);
                        }
                        if (velocity().y > vel_limit.y) {
                            velocity_y(vel_limit.y);
                        }
                        //console::log(class_name(), "::collide_y() rotation: ", sprite::rotation(m_sprite), " ", cos, "\n");
                    }
                    //}

                } else if (velocity().y < 0.0F and m_state == state::Type::swim) {
                    //if (our_UL.y + 4.0F > other_DR.y) return;

                    if (velocity().y >= -1.0F or is_pressed(key_jump)) {
                        velocity_y(std::abs(velocity().y * 0.2F));
                        if (velocity().y < 1.0F) {
                            velocity_y(1.0F);
                        }
                        return;
                    }

                    //console::log("is_pressed(key_jump): ", is_pressed(key_jump), "\n");
                    if (velocity().y <= -1.0F and !is_pressed(key_jump)) {
                        is_upwards = true;
                        if (!sound_is_playing("water_exit") and !sound_is_playing("water_enter") and m_state != state::Type::dive) {
                            sound_position("water_exit", { (position().x + 8.0F) / app::config::extent().x / 2.0F,
                                                           (position().y + 8.0F) / app::config::extent().y / 2.0F });
                            sound_play("water_exit");
                            is_to_splash = true;
                        }

                        if (m_next_state != state::Type::dive) {
                            max_velocity({ 10.0F, 10.0F });

                            velocity_y(velocity().y * 1.2F);
                            //velocity_y(velocity().y + other_velocity.y * 1.0F);
                            //console::log(class_name(), "::collide_y() vel y : ", velocity().y, "\n\n");

                            if (velocity().y > -5.0F) {
                                velocity_y(-5.0F);
                            }
                            m_next_state = state::Type::dive;
                            m_time_in_state = 0;
                        }
                    } else {
                        //velocity_y(1.0F);
                        m_time_in_state = 0;
                        is_to_splash = false;
                    }
                }

                m_is_on_ground = false;
                m_is_down_thrusting = false;

                if (is_to_splash) {
                    console::log(class_name(), "::collide_y() is upwards: ", is_upwards, " velocity y: ", velocity().y, "\n");

                    cVec2F pos = Vec2F{ our_UL.x + 4.0F, other_UL.y } + Vec2F{ 0.0F, is_upwards ? -10.0F : 0.0F };
                    cF32 speed = is_upwards ? 1.2F : 1.0F;

                    particle::splash_water(this, pos, velocity(), speed);

                }
                break;
            }
        }
    }
}