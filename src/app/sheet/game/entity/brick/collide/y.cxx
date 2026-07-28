module entity.brick;
import app.config;
import console;
import aabb;
import particle_system;
import entity;

namespace entity {
    void Brick::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner or is_dead() or m_state == state::Type::carried or other.owner->state() == state::Type::carried) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        //if (velocity().y > 0.0F and our_UL.y > other_DR.y - 4.0F) return;

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F our_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();
        //cVec2F other_move_velocity = other.owner->move_velocity();

        state::cType other_state = other.owner->state();

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (m_parent or other.owner->parent() or is_hurting()) return;
                console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
                position_add_y(-overlap_y);
                velocity_y(our_velocity.y * -0.9F);
                velocity_x(our_velocity.x * 0.5F);
                //if (velocity().y <= -m_break_velocity.y or velocity().y >= m_break_velocity.y) {
                hurt(other.owner);
                //return;
            //}
                break;
            }
            case Type::brick: {
                if (m_state == state::Type::tossed or std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    velocity_y(our_velocity.y * -0.2F);
                    other.owner->velocity_y(our_velocity.y * 0.2F);
                    return;
                }

                if (other_DR.y <= our_UL.y + 1.0F) {
                    add_input(other.owner);
                    add_inputs_from(other.owner);
                    //other.owner->velocity(velocity());
                    //console::log("add_input_from(other.owner)\n");
                    //console::log(class_name(), "::collide_y() num_inputs() ", num_inputs(), "\n");

                    //other.owner->position_add_y(-overlap_y);
                    //other.owner->is_on_ground(true);
                    //other.owner->velocity_y(0.0F);
                    //other.owner->move_velocity({});
                }
                else if (our_DR.y < other_UL.y + 2.0F) {
                    //other.owner->add_input(this);
                    //other.owner->add_inputs_from(this);
                    //console::log(class_name(), "::collide_y() other.owner->num_inputs() ", other.owner->num_inputs(), "\n");
                    //if (our_UL.x > other_DR.x - 1.0F or
                        //our_DR.x < other_UL.x + 1.0F) return;
                    position_add_y(-overlap_y);
                    //position_add_y(-0.1F);
                    //m_is_on_ground = true;
                    //velocity_x(other_velocity.x);
                    velocity_y(0.0F);
                    move_velocity({});

                    //move_velocity(other_velocity);

                    if (!is_interacting()) {
                        m_parent = other.owner;
                        //add_input(other.owner);

                        //other.owner->add_input(this);
                        //other.owner->add_inputs_from(this);
                        //console::log("other.owner->add_inputs_from(this)\n");
                        //console::log("other.owner->num_inputs() ", other.owner->num_inputs(), "\n");

                        //auto v = other.owner->inputs();
                        /*for (auto& input : m_parent->inputs()) {
                            add_input(input);
                        }*/
                    }

                    //move_velocity(other_velocity * 0.8F);
                    

                    /*if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                        velocity_x(0.0F);
                        move_velocity_x(other_velocity.x * 0.9F);
                    }*/


                    //velocity_y(our_velocity.y * 0.0F + other_velocity.y * 0.9F);

                    //velocity_x(0.0F);
                    
                    /*if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                        other.owner->move_velocity_x(our_velocity.x * 0.9F + other_velocity.x * 0.0F);
                    } else {
                        move_velocity_x(our_velocity.x * 0.0F + other_velocity.x * 0.9F);
                    }

                    if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                        other.owner->move_velocity_y(our_velocity.y * 0.9F + other_velocity.y * 0.0F);
                    } else {
                        move_velocity_y(our_velocity.y * 0.0F + other_velocity.y * 0.9F);
                    }*/
                } else {
                    //move_velocity({});
                }
                break;
            }
            case Type::bridge: {
                position_add_y(-overlap_y);
                velocity_y(0.0F);
                m_is_on_ground = true;
                break;
            }
            case Type::bee:
            case Type::bee_hive: {
                if (m_state == state::Type::tossed or std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    collide_x(our, other);
                    return;
                }
                break;
            }
            case Type::bug: {
                if (our_DR.y > other_UL.y + other_extent.y / 2.0F) {
                    //collide_x(our, other);
                    return;
                }
                console::log("Brick::collide_y bug\n");
                //if (m_state == state::Type::tossed
                //    //std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y
                //    ) {
                //    //velocity_y(velocity().y * -0.5F);
                //    //hurt(other.owner);
                //    //other.owner->hurt(this);
                //    collide_x(our, other);
                //    return;
                //}
                /*if (our_UL.y < other_DR.y - 2.0F) {
                    position_add_y(-overlap_y);
                    m_is_on_ground = true;
                    velocity_y(0.0F);
                    move_velocity(other_velocity);
                }*/


                position_add_y(-overlap_y);

                if (our_velocity.y > 2.0F and
                    (other_state == state::Type::upended or other_state == state::Type::bounce)) {
                    
                    console::log(class_name(), "::collide_y() bug vel: ", velocity().y, "\n");
                    max_velocity_y(10.0F);
                    velocity_y(-5.0F - velocity().y / 4.0F);
                    move_velocity_y(0.0F);                    
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    other.owner->hurt(this);
                    other.owner->next_state(state::Type::bounce);
                    return;
                }

                velocity_y(0.0F);
                move_velocity(other_velocity);
                m_is_on_ground = true;


                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_D:
            case Type::clip_D_50:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_UD: {
                /*if (our_UL.y > other_DR.y - 2.0F) {
                    return;
                }*/
                if ((other_type == Type::clip_D or other_type == Type::clip_D_50) and velocity().y + move_velocity().y > 0.0F) {
                    return;
                }
                if (other_type == Type::clip_LD and velocity().x + move_velocity().x < 0.0F
                    and velocity().y + move_velocity().y > 0.0F) {
                    return;
                }
                if (other_type == Type::clip_RD and velocity().x + move_velocity().x > 0.0F
                    and velocity().y + move_velocity().y > 0.0F) {
                    return;
                }

                if (m_state == state::Type::tossed or std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    //console::log("velocity.y: ", velocity().y, "\n");
                    if (
                        ((other_type == Type::clip_D or other_type == Type::clip_D_50 or other_type == Type::clip_LD or other_type == Type::clip_RD) and our_velocity.y < -1.0F)
                        or
                        std::abs(velocity().y) >= m_break_velocity.y) {
                        position_add_y(-overlap_y);
                        velocity_y(velocity().y * -0.9F);
                        hurt(other.owner);
                        return;
                    }
                }

                if (velocity().y < 0.0F and other_DR.y < our_DR.y) {
                    position_add_y(-overlap_y);
                    //velocity_y(velocity().y * -1.0F);
                }
                else if (velocity().y > 0.0F and other_UL.y > our_UL.y) {
                    position_add_y(-overlap_y);
                    move_velocity({});
                    m_is_on_ground = true;
                }
                move_velocity({});
                velocity_y(0.0F);
                break;
            }
            case Type::clip_U:
            case Type::clip_U_50:
            case Type::slope_U: {
                if (our_velocity.y < 0.0F) return;
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    console::log("Brick::collide_y velocity.y: ", velocity().y, "\n");
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                if (other_UL.y > our_UL.y) {
                    position_add_y(-overlap_y);
                    velocity_y(0.0F);
                    move_velocity({});
                    m_is_on_ground = true;
                    m_is_on_slope = other_type == Type::slope_U;
                }                
                break;
            }
            case Type::frog: {
                if (m_state == state::Type::tossed or std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    //if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    position_add_y(-overlap_y);
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    other.owner->hurt(this);
                    return;
                }
                if (our_DR.y >= other_UL.y + 4.0F) return;

                position_add_y(-overlap_y);
                velocity_y(0.0F);
                move_velocity(other_velocity);
                m_is_on_ground = true;
                break;
            }
            case Type::particle_drop: {
                add_child(other.owner);
                break;
            }
            case Type::player: {
                if (other_UL.y < our_UL.y) {
                    //m_weight = m_start_weight + other.owner->weight();
                    //console::log("player on top\n");
                    add_input(other.owner);
                    add_inputs_from(other.owner);
                    return;
                }                
                if (our_DR.y >= other_UL.y + 4.0F) return;

                position_add_y(-overlap_y);
                velocity_y(-std::abs(other_velocity.y));
                if (std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    //console::log(class_name(), "::collide_y() player broke brick ", other_velocity.y, "\n");
                    velocity(other_velocity);
                    hurt(other.owner);
                    //velocity_y(0.0F);

                    other.owner->velocity_x(other_velocity.x * 0.5F);
                    other.owner->velocity_y(other_velocity.y * -0.9F);
                    return;
                }
                
                
                return;

                if (other.owner->state() == state::Type::sling) {
                    velocity_x(-other.owner->rotation_speed());
                    hurt(other.owner);
                    return;
                }
                break;
            }
            case Type::slope_L_1x1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(velocity().y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                position_add_y(-overlap_y);
                //velocity_y(-velocity().x;
                if (velocity().y > 0.0F) {
                    velocity_x(-velocity().y);
                }
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_1x1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(velocity().y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                position_add_y(-overlap_y);
                //velocity_y(velocity().x);
                if (velocity().y > 0.0F) {
                    velocity_x(velocity().y);
                }
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                position_add_y(-overlap_y);
                //velocity_y(0.0F);
                move_velocity_y(0.0F);
                //if (velocity().x < 0.0F) {

                //velocity_y(velocity().x / -2.0F;
                //}
                //if (velocity().y > 0.0F) {
                    //velocity_x(-velocity().y / 2.0F;
                //}
                velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                //if (velocity().x > 0.0F) {
                    //velocity_y(velocity().x / 2.0F;
                //}
                if (velocity().y > 0.0F) {
                    velocity_x(velocity().y / 2.0F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::spring_U: {
                if (m_parent) return;
                position_y(other_UL.y - our_extent.y);
                max_velocity_y(10.0F);
                velocity_y(-5.0F - our_velocity.y / 4.0F);
                m_is_on_ground = false;
                m_is_on_slope = false;
                break;
            }
            case Type::train_platform: {
                //console::log("hello train ", aabb::name_as_string(other_name), "\n");
                if (our_velocity.y < 0.0F or other_name == aabb::Name::bone
                    or our_DR.y > other_DR.y - 4.0F
                    //or our_DR.y - 4.0F > other_UL.y
                    ) {
                    return;
                }

                move_velocity(other_velocity);
                velocity_y(1.0F);
                position_add_y(-overlap_y / 8.0F); // move in a bit to prevent flying off
                break;
            }
            case Type::train_saw: {
                position_add_y(-overlap_y);
                cVec2F aabb_vel = aabb::velocity(other.id);
                velocity(other_velocity + aabb_vel * 0.5F - our_velocity * 0.5F);
                hurt(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (m_state == state::Type::swim) return;

                m_next_state = state::Type::swim;
                cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F };
                bool is_to_splash = false;
                
                if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                    sound_position("water_enter", sound_pos);
                    sound_play("water_enter");
                    is_to_splash = true;
                }
                
                if (is_to_splash) {
                    console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                    cVec2F pos = Vec2F{ our_UL.x, other_UL.y } + Vec2F{ 0.0F, 0.0F };
                    cVec2F vel = Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) };

                    particle::splash_water(this, pos, vel, 2.0F);
                }
                break;
            }
        }
    }
}