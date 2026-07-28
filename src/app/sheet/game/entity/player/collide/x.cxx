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

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_center = aabb::center(our.id);
        cVec2F other_center = aabb::center(other.id);

        /*if (other_UL.x < our_UL.x and velocity().x > 0.0F or
            other_UL.x > our_UL.x and velocity().x < 0.0F) return;*/

        state::cType other_state    = other.owner->state();

        cVec2F our_velocity   = velocity();
        cVec2F our_move_velocity = move_velocity();
        cVec2F our_sum_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity();
        cVec2F other_move_velocity = other.owner->move_velocity();
        cVec2F other_sum_velocity = other.owner->velocity() + other.owner->move_velocity();

        cU16 other_number = other.owner->number();

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        bool is_shoot_direction_locked_and_away = m_state == state::Type::shoot and
                                                          (other_UL.x < our_UL.x and !sprite_is_leftward() or
                                                           other_DR.x > our_DR.x and sprite_is_leftward());

        bool is_to_duck = other_DR.y < our_center.y and 
                         !is_shoot_direction_locked_and_away and
                          m_is_on_ground or m_time_left_rising > 0;

        if (m_is_carrying) {
            is_to_duck = false;
            m_time_left_rising = 0;
        }

        if (m_state == state::Type::sling) {
            //position_add_x( -overlap_x );
            //velocity_x(-velocity().x;

            if (is_arch(other_type)) {
                collide_y(our, other);
            }
            else if (is_clip(other_type)) {
                if (!sound_is_playing("bump_head")) {
                    sound_stop("bump_head");
                    sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                    sound_play("bump_head");

                    m_rotation_speed *= -1.0F;
                }
            }
            return;
        }

        //console::log(sprite::to_string(other_type), " overlap_x: ", overlap_x, "\n");

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {                
                break;
            }
            case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (m_state == state::Type::swim) {
                    collide_y(our, other);
                }
                break;
            }
            case Type::brick: {
                if (our_velocity.y >= 6.0F) {
                    collide_y(our, other);
                    return;
                }
                if (our_DR.y < other_UL.y + 4.0F or
                    our_UL.y > other_DR.y - 1.0F) {
                    //collide_y(our, other);
                    return;
                }
                if (other_DR.x < our_center.x and our_velocity.x > 0.0F or
                    other_UL.x > our_center.x and our_velocity.x < 0.0F) {
                    return;
                }

                //console::log(class_name(), "::collide_x() num inputs: ", other.owner->num_inputs(), "\n");

                console::log(class_name(), "::collide_x() force y: ", other.owner->force().y, "\n");
                position_add_x(-overlap_x);
                other.owner->velocity_x(our_velocity.x / other.owner->force().y);
                /*if (std::abs(other_velocity.x) > std::abs(our_velocity.x)) {
                    velocity_x(other_velocity.x);
                }
                else {
                    other.owner->velocity_x(our_velocity.x);
                }*/
                //console::log("num inputs: ", other.owner->num_inputs(), "\n");
                //F32 force_x = 1.0F / (other.owner->num_inputs() + 1.0F);
                //console::log("num inputs: ", other.owner->num_inputs(), " force_x: ", force_x, "\n");
                //other.owner->velocity_x(our_velocity.x * force_x);
                //velocity_x(our_velocity.x * force_x);
                //velocity_x(0.0F);
                break;
            }
            case Type::bug: {
                return;
                if (our_DR.y < other_UL.y + 2.0F) return;

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
                /*if (our_DR.y < other_UL.y + 2.0F or
                    our_UL.y > other_DR.y - 2.0F) {
                    return;
                }
                position_add_x( -overlap_x );*/
                break;
            }
            case Type::clip:
            case Type::clip_LR: {
                if (m_state == state::Type::climb or m_state == state::Type::duck) {
                    m_time_left_rising = m_config.time_to_rise();
                    //return;
                }
                if (other_type == Type::clip and is_to_duck) {
                    m_time_left_rising = m_config.time_to_rise();
                    if (m_state != state::Type::duck) {
                        m_next_state = state::Type::duck;
                        velocity_x(velocity().x * 0.5F);
                    }
                    return;
                }
                if (our_DR.y > other_UL.y and our_DR.y - other_UL.y < 2.0F) return;

                if (our_sum_velocity.x < 0.0F) m_is_near_wall_L = true;
                if (our_sum_velocity.x > 0.0F) m_is_near_wall_R = true;

                //m_is_wall_to_left = other_UL.x < our_UL.x;
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                
                if (m_is_on_ground or m_is_carrying or m_is_hovering) return;

                if (our_velocity.y > 0.0F and 
                    ((is_pressed(key_right) and m_is_near_wall_R) or (is_pressed(key_left) and m_is_near_wall_L))
                    ) {
                    if (m_state == state::Type::shoot and is_pressed(key_sprint) and sprite_is_leftward()) {
                        return;
                    }
                    if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                        m_next_state = state::Type::wall_slide;
                    }
                }
                break;
            }
            case Type::clip_D:
            case Type::clip_D_50: {
                if (m_state == state::Type::climb) {
                    m_time_left_rising = m_config.time_to_rise();
                    return;
                }
                if (is_to_duck) {
                    m_time_left_rising = m_config.time_to_rise();
                    if (m_state != state::Type::duck) {
                        m_next_state = state::Type::duck;
                        velocity_x(velocity().x * 0.5F);
                    }
                    return;
                }
                break;
            }
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50: {
                if (m_state == state::Type::ledge) return;
                
                if (m_state == state::Type::climb) {
                    m_time_left_rising = m_config.time_to_rise();
                    return;
                }
                //if (other_type == Type::clip_ledge_L_50 or other_type == Type::clip_ledge_R_50) {
                //}
                if (is_to_duck/* and other_type == Type::clip_ledge*/) {
                    
                    if (m_state != state::Type::duck
                        //and
                        //m_time_left_rising == 0 and
                        //m_next_state != state::Type::duck and
                        //!(our_sum_velocity.x <= 0.0F and m_is_near_wall_L)
                        //and
                        //!(our_sum_velocity.x >= 0.0F and m_is_near_wall_R)
                        ) {
                        if (is_pressed(key_left) or is_pressed(key_right)) {
                            m_time_left_rising = m_config.time_to_rise();
                            m_next_state = state::Type::duck;
                        }
                        velocity_x(velocity().x * 0.5F);
                    }
                    return;
                }

                if (our_DR.y > other_UL.y and our_DR.y - other_UL.y < 3.0F) return;
                if (our_UL.y > other_DR.y - 2.0F) {
                    //console::log(class_name(), "::collide_x clip_ledge skip\n");
                    return;
                }
                if (our_DR.y < other_UL.y + 2.0F) {
                    //console::log(class_name(), "::collide_x clip_ledge above skip\n");
                    return;
                }
                if (our_velocity.x < 0.0F and our_UL.x < other_DR.x - 4.0F or
                    our_velocity.x > 0.0F and our_DR.x > other_UL.x + 4.0F) {
                    return;
                }

                position_add_x(-overlap_x);
                if (m_state == state::Type::swim) {
                    return;
                }

                //if (our_sum_velocity.x < 0.0F) m_is_wall_to_left = true;
                //if (our_sum_velocity.x > 0.0F) m_is_wall_to_left = false;

                if (our_sum_velocity.x < 0.0F) m_is_near_wall_L = true;
                if (our_sum_velocity.x > 0.0F) m_is_near_wall_R = true;


                velocity_x(0.0F);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                m_is_on_slope = false;
                if (m_is_on_ground or m_is_hovering or is_locked(key_jump) or m_is_carrying) return;
                //if (m_is_on_ground and !is_pressed(key_up)) return;

                /*if ((m_is_wall_to_left and our_velocity.x > 0.0F) or
                    (!m_is_wall_to_left and our_velocity.x < 0.0F)) {
                    return;
                }*/
                
                if (our_velocity.y > acceleration().y) {
                    if ((is_pressed(key_left)) or (is_pressed(key_right))) {
                        if (m_state == state::Type::shoot and is_pressed(key_sprint) and our_velocity.x < 0.0F != sprite_is_leftward()) {
                            return;
                        }
                        console::log(class_name(), "::collide_x() ", to_string(other_type), "hmm\n");
                        if (m_state != state::Type::ledge and our_UL.y < other_UL.y - 1.0F /*and our_UL.y + 16 > other_UL.y*/) {                            
                            m_next_state = state::Type::ledge;
                            position_y(other_UL.y - 6);
                            velocity_y(0.0F);
                        } else if (m_state == state::Type::run or m_state == state::Type::shoot) {
                            if (!m_is_on_ground) {
                                m_next_state = state::Type::wall_slide;
                            }
                        }
                    }
                }
                break;
            }
            case Type::clip_L:
            case Type::clip_L_50: {
                if (our_sum_velocity.x < 0.0F or m_state == state::Type::ledge) return;                
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                m_is_on_slope = false;
                                
                m_is_near_wall_R = true;

                if (m_is_on_ground or m_is_carrying or m_is_hovering) return;

                if (our_velocity.y > 0.0F and is_pressed(key_right)) {
                    if (m_state == state::Type::shoot and is_pressed(key_sprint) and sprite_is_leftward()) {
                        return;
                    }
                    if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                        m_next_state = state::Type::wall_slide;
                    }
                }
                break;
            }
            case Type::clip_R:
            case Type::clip_R_50: {
                if (our_sum_velocity.x > 0.0F or m_state == state::Type::ledge) return;
                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                m_is_on_slope = false;

                m_is_near_wall_L = true;

                if (m_is_on_ground or m_is_carrying or m_is_hovering) return;
                if (our_velocity.y > 0.0F and is_pressed(key_left)) {
                    if (m_state == state::Type::shoot and is_pressed(key_sprint) and !sprite_is_leftward()) {
                        return;
                    }
                    if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                        m_next_state = state::Type::wall_slide;
                    }
                }
                break;
            }
            case Type::clip_LD: {
                if (our_UL.y - 4.0F > other_DR.y) {
                    console::log(class_name(), "::collide_x() our_UL.y - 1.0F > other_DR.y\n");
                    return;
                }
                if (m_state == state::Type::climb) {
                    m_time_left_rising = m_config.time_to_rise();
                    console::log("m_time_left_rising = m_config.time_to_rise\n");
                    return;
                }
                
                if (is_to_duck) {
                    m_time_left_rising = m_config.time_to_rise();
                    if (m_state != state::Type::duck) {
                        m_next_state = state::Type::duck;
                        velocity_x(velocity().x * 0.5F);
                    }
                    return;
                }

                if (our_velocity.x < 0.0F or m_state == state::Type::ledge) return;

                console::log(class_name(), "::collide_x ", to_string(other_type), "\n");

                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                //m_is_wall_to_left = false;
                m_is_near_wall_R = true;
                m_is_on_slope = false;

                if (!m_is_on_ground and !m_is_carrying and !m_is_hovering) {
                    if (our_velocity.y > 0.0F and is_pressed(key_right)) {
                        if (m_state == state::Type::shoot and is_pressed(key_sprint) and sprite_is_leftward()) {
                            return;
                        }
                        if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                            m_next_state = state::Type::wall_slide;
                        }
                    }
                }
                break;
            }
            case Type::clip_RD: {
                if (our_UL.y - 4.0F > other_DR.y) {
                    console::log(class_name(), "::collide_x() our_UL.y - 1.0F > other_DR.y\n");
                    return;
                }
                if (m_state == state::Type::climb) {
                    m_time_left_rising = m_config.time_to_rise();
                    console::log("m_time_left_rising = m_config.time_to_rise\n");
                    return;
                }
                if (is_to_duck) {
                    m_time_left_rising = m_config.time_to_rise();
                    if (m_state != state::Type::duck) {
                        m_next_state = state::Type::duck;
                        velocity_x(velocity().x * 0.5F);
                    }
                    return;
                }

                if (our_velocity.x > 0.0F or m_state == state::Type::ledge) return;

                console::log(class_name(), "::collide_x ", to_string(other_type), "\n");

                position_add_x(-overlap_x);
                velocity_x(0.0F);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                //m_is_wall_to_left = true;
                m_is_near_wall_L = true;
                m_is_on_slope = false;

                if (!m_is_on_ground and !m_is_carrying and !m_is_hovering) {
                    if (our_velocity.y > 0.0F and is_pressed(key_left)) {
                        if (m_state == state::Type::shoot and is_pressed(key_sprint) and !sprite_is_leftward()) {
                            return;
                        }
                        if (m_state == state::Type::run or m_state == state::Type::shoot or m_state == state::Type::dive) {
                            m_next_state = state::Type::wall_slide;
                        }
                    }
                }
                break;
            }
            case Type::coin: {
                if (other.owner->is_dead()) return;

                sound::position(other.owner->sound("dead"), { (other_UL.x + 4.0F) / app::config::extent().x / 2.0F,
                                                              (other_UL.y + 4.0F) / app::config::extent().y / 2.0F });
                sound::play(other.owner->sound("dead"));

                other.owner->time_left_alive(0);
                other.owner->time_left_dead(U16_MAX);

                console::log(class_name(), "::collide_x() coin: ", other_number, "\n");

                sheet::game::add_picked_coin(other_number);
                break;
            }
            case Type::frog: {
                if (m_state == state::Type::sling) {                    
                    /*console::log("Player::collide_x() frog slinged");
                    if (!sound_is_playing("hit")) {
                        sound_position("hit", { position().x - app::config::extent().x / 2.0F,
                                                position().y - app::config::extent().y / 2.0F });
                        sound_pitch("hit", 1.0F - velocity().y * 0.1F);
                        sound_play("hit");
                    }*/
                    return;
                }

                //if (our_DR.y + 1.0F <= other_UL.y) return;
                //velocity_x(move_velocity_x(0.0F);

                position_add_x(-overlap_x);

                if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                    return;
                }

                velocity_x(other_velocity.x * 0.5F);
                other.owner->velocity_x(our_velocity.x * 0.25f);
                if (our_UL.x < other_UL.x) {
                    velocity_add({ -1.0F, 0.0F });
                    other.owner->velocity_add({ 1.0F, 0.0F });
                } else {
                    velocity_add({ 1.0F, 0.0F });
                    other.owner->velocity_add({ -1.0F, 0.0F });
                }
                break;
            }
            case Type::level_L_0: {
                m_next_level = start::Type::L_0;
                m_next_start = { start::Type::R_0, 0 };
                break;
            }
            case Type::level_R_0: {
                m_next_level = start::Type::R_0;
                m_next_start = { start::Type::L_0, 0 };
                break;
            }
            case Type::level_center: {
                if (velocity().x != 0.0F) {
                    m_is_to_save = true;
                }
                break;
            }
            case Type::mole: {
                collide_y(our, other);
                break;
            }            
            case Type::spring_U: {
                collide_y(our, other);
                break;
            }
            case Type::train_saw: {
                position_add_x(-overlap_x);
                cVec2F aabb_vel = aabb::velocity(other.id);
                velocity(other_velocity + aabb_vel - our_velocity);
                if (other.owner->time_left_alive() > 0) {
                    hurt(other.owner);
                } else {
                    health_amount_add(-4.0F);
                }
                //console::log("velocity after : ", velocity().x, " ", velocity().y, "\n");
                return;                
                //console::log("rotation speed: ", other.owner->rotation_speed(), "\n");
                break;
            }
            case Type::player: {
                return;
                position_add_x(-overlap_x);
                //velocity_x(0.0F);

                //velocity_x(velocity().x * 0.5F + velocity.x * 0.5F;

                velocity_x(velocity().x * 0.75F + other_velocity.x * 0.25f);
                //velocity_y(velocity().y * 0.5F + velocity.y * 0.5F;
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
    }
}