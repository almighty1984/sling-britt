module entity.bug;
import app.config;
import aabb;
import console;
import sound;
import particle_system;

namespace entity {
    void Bug::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F our_center = aabb::center(our.id);
        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };

        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);
        cVec2F other_center = aabb::center(other.id);        
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };
        state::Type other_state = other.owner->state();

        /*if (velocity().y > 0.0F and our_DR.y - 4.0F - velocity().y > other_UL.y) {
            return;
        }*/

        //if (velocity().y > 0.0F and our_UL.y > other_DR.y - 4.0F) return;


        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        //console::log("type: ", to_string(other_type), "\n");

        if (m_state == state::Type::carried) {
            if (is_clip(other_type) or
                other_type == Type::brick or
                other_type == Type::bug or
                other_type == Type::spring_U
                ) {
                collide_x(our, other);
                return;
            }
        }

        if (m_state == state::Type::tossed) {
            if (our_velocity.y < 0.0F and our_UL.y > other_DR.y - 4.0F) {
                if (is_arch(other_type) or
                    other_type == Type::clip or
                    other_type == Type::clip_D or
                    other_type == Type::clip_D_50 or
                    other_type == Type::clip_LD or
                    other_type == Type::clip_RD or
                    other_type == Type::clip_ledge or
                    other_type == Type::clip_ledge_L_50 or
                    other_type == Type::clip_ledge_R_50
                    ) {
                    position_add_y(-overlap_y);
                    m_parent = nullptr;
                    if (!is_hurting()) {
                        if (other_type == Type::slope_L_1x1 or other_type == Type::slope_R_1x1) {
                            velocity_x(velocity().x * 0.5F);
                        } else if (other_type == Type::slope_L_1x1 or other_type == Type::slope_R_1x1) {
                            velocity_x(velocity().x * 0.75F);
                        }
                        velocity_y(velocity().y * -0.9F);
                        cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                       (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };

                        particle::spawn(this, particle::Type::hit, hit_pos, {});
                        hurt(other.owner);
                        m_next_state = state::Type::hurt;
                        return;
                    }
                }
            }
        }


        if (m_state == state::Type::dive) {
            if (is_clip(other_type) and other_velocity.y >= 0.0F and our_DR.y < m_water_line_y) {
                m_next_state = state::Type::walk;
                sprite_is_leftward(our_velocity.x < 0.0F);
            }
        }

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (is_hurting()) return;
                console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
                position_add_y(-overlap_y);
                if (velocity().y <= -2.0F or velocity().y >= 2.0F) {

                    cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                       (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };

                    velocity_y(velocity().y * -0.9F);
                    particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                    hurt(other.owner);
                    return;
                }
                break;
            }
            case Type::bee:
            case Type::bee_hive: {
                if (m_state == state::Type::tossed) {
                    collide_x(our, other);
                    return;
                }
                break;
            }
            case Type::brick: {                
                return;
                //if (our_UL.y > other_UL.y) return;
                //if (our_UL.y > other_DR.y - 4.0F) return;

            
                if (our_DR.y < other_UL.y + 2.0F) {
                    position_add_y(-overlap_y);
                    console::log(class_name(), "::collide_y brick above\n");
                }
                if (velocity().y <= -2.0F or velocity().y >= 2.0F) {

                    cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                       (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };

                    velocity_y(velocity().y * -0.9F);
                    particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                    hurt(other.owner);
                    //other.owner->hurt(this);
                    return;
                }
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50:
            case Type::clip_ledge_R_50: {
                if (our_UL.y < other_DR.y) {
                    if (velocity().y > 0.0F) {
                        position_add_y(-overlap_y);
                        //console::log(class_name(), "::collide_y hmmmmm\n");                
                    }
                }
                velocity_y(0.0F);
                move_velocity_y(0.0F);
                if (our_UL.y < other_UL.y) {
                    m_is_on_ground = true;
                    m_is_on_slope = false;                    
                }

                //if (!m_is_on_ground) return;
                if (other_type == Type::clip_ledge) {
                    if (our_velocity.y > 0.0F and our_UL.y > other_UL.y) return;

                    /*if (m_state == state::Type::tossed and std::abs(our_velocity.y) >= -2.0F) {
                        velocity_y(our_velocity.y * -0.9F);
                        cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                           (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };

                        particle::spawn(this, particle::Type::hit, hit_pos, {});
                        hurt(other.owner);
                        m_next_state = state::Type::hurt;
                        sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                        sound_play("melee");
                        return;
                    }*/
                    if (m_state == state::Type::walk and m_time_in_state > 30 and m_time_left_turning == 0) {
                        if (our_velocity.x > 0.0F and our_DR.x >= other_DR.x and our_center.x > other_center.x and our_center.x < other_center.x + 2.0F) {
                            m_time_left_turning = m_config.time_to_turn();
                            position_add_x(-1.0F);
                            velocity_x(our_velocity.x * -1.0F);
                            sprite_is_leftward(true);
                        }
                        else if (our_velocity.x < 0.0F and our_UL.x <= other_UL.x and our_center.x < other_center.x and our_center.x > other_center.x - 2.0F) {
                            m_time_left_turning = m_config.time_to_turn();
                            position_add_x(1.0F);
                            velocity_x(our_velocity.x * -1.0F);
                            sprite_is_leftward(false);
                        }
                    }
                }
                else if (other_type == Type::clip_ledge_L_50) {
                    sprite_is_leftward(false);
                }
                else if (other_type == Type::clip_ledge_R_50) {
                    sprite_is_leftward(true);
                }
                break;
            }
            case Type::clip_U:
            case Type::clip_U_50:
            case Type::slope_U: {
                //if (our_velocity.y < 0.0F) return;
                if (our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = other_type == Type::slope_U;
                break;
            }
            case Type::clip_D:
            case Type::clip_D_50:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (velocity().y > 0.0F) return;
                position_add_y(-overlap_y);
                if (m_state == state::Type::tossed/*velocity().y <= -2.0F or velocity().y >= 2.0F*/) {
                    velocity_y(velocity().y * -0.9F);
                    cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                       (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    hurt(other.owner);
                    m_next_state = state::Type::hurt;
                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");
                    return;
                }
                m_is_on_ground = false;
                m_is_on_slope = false;
                velocity_y(std::abs(velocity().y) * 0.75F);

                m_next_state = state::Type::hurt;
                break;
            }
            case Type::frog: {
                console::log("entity::Bug::collide_y frog\n");
                if (m_state == state::Type::tossed or (m_state == state::Type::upended and our_UL.y < other_UL.y)) {
                    cVec2F hit_pos = { (our_UL.x < other_UL.x ? other_UL.x : other_DR.x) - 8.0F,
                                       (our_UL.y < other_UL.y ? other_UL.y : other_DR.y) - 8.0F };
                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    hurt(other.owner);
                    m_next_state = state::Type::hurt;
                    other.owner->hurt(this);
                    //velocity().x *= -1.0f;
                    velocity_y(velocity().y * -1.0f);
                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");
                    return;
                }

                if (our_center.y < other_center.y) {
                    position_add_y(-overlap_y);
                    if (m_state == state::Type::upended) {
                        m_next_state = state::Type::bounce;
                    }
                }
                //if (other_UL.y > our_UL.y) {            
                    //if (!is_hurting()) {
                    //    velocity_y(other.owner->velocity().y + velocity().y - 10.0f;
                    //}
                //} else {
                    //position_add_y( -overlap_y );            
                //}
                break;
            }
            case Type::particle_interact: {
                collide_x(our, other);
                break;
            }
            case Type::player: {
                if (m_state == state::Type::upended or m_state == state::Type::bounce) {                                
                    if (other_DR.y < our_UL.y + 4.0F and other_velocity.y > 2.0F) {
                        console::log(class_name(), "collide_y() player\n");
                        m_state = state::Type::upended;
                        m_next_state = state::Type::bounce;
                    }
                    return;
                }
                //console::log(class_name(), "::collide_y player overlap_y: ", overlap_y, "\n");
                
                if (other.owner->state() == state::Type::sling) return;



                if (our_UL.y > other_DR.y - 2.0F/* - other_velocity.y*/) {
                
                }
                else if (our_DR.y <= other_UL.y + other_extent.y / 2.0F) {
                    position_add_y(-overlap_y);
                    if (other_state == state::Type::swim) {
                        m_next_state = state::Type::dive;
                    }
                    //other.owner->velocity_y(std::abs(our_velocity.y) * 0.5F + std::abs(other_velocity.y) * 0.5F);                        
                    position_add_y(-8.0F);
                    velocity_y(other_velocity.y * 1.2F);
                    velocity_x(other_velocity.x * 0.5F);

                    if (velocity().x > -0.2F and velocity().x < 0.2F) {
                        if (sprite_is_leftward()) {
                            velocity_x(-0.2F);
                        }
                        else {
                            velocity_x(0.2F);
                        }
                    }
                    other.owner->velocity_y(other_velocity.y * -1.0F);

                    cVec2F our_center = Vec2F{ our_UL.x + (our_DR.x - our_UL.x) / 2.0F,     our_UL.y + (our_DR.y - our_UL.y) / 2.0F };
                    cVec2F other_center = Vec2F{ other_UL.x + (other_DR.x - other_UL.x) / 2.0F, other_UL.y + (other_DR.y - other_UL.y) / 2.0F };

                    cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});

                    sound_position("melee", { position().x - app::config::extent().x / 2.0F,
                                              position().y - app::config::extent().y / 2.0F });
                    sound_play("melee");
                }
                break;
            }
            case Type::slope_L_1x1: {
                //if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(-velocity().x);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_1x1: {
                //if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(velocity().x);
                move_velocity_x(0.0F);
                move_velocity_y(0.0F);

                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                position_add_y(-overlap_y);
                move_velocity_y(0.0F);
                velocity_y(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
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
                if (velocity().y < 0.0F) return;
                position_y(other_UL.y - our_extent.y);
                max_velocity_y(10.0F);
                velocity_y(-5.0F - our_velocity.y / 4.0F);
                m_is_on_ground = false;
                m_is_on_slope = false;                                
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (m_state == state::Type::swim and m_num_jumps == 1 and our_velocity.y < 0.0F) return;
                bool is_to_splash = false,
                     is_upwards   = false;

                m_num_jumps = 0;

                m_radians = other.owner->radians();
            
                if (our_velocity.y > 0.0F and m_state != state::Type::swim) {
                    m_next_state = state::Type::swim;
                }

                m_water_line_y = other.owner->start_offset().y;

                //if (position_on_level().y + 0.0F > m_water_line_y) return;

                cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F };

                if (our_velocity.y <= -2.0F) {
                    is_upwards = true;
                    if (!sound_is_playing("water_exit") and !sound_is_playing("water_enter")) {
                        sound_position("water_exit", sound_pos);
                        sound_play("water_exit");
                        is_to_splash = true;
                    }
                    m_time_in_state = 0;
                    m_is_on_ground = false;
                }
                else if (our_velocity.y > 2.0F) {
                    is_upwards = false;
                    if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                        sound_position("water_enter", sound_pos);
                        sound_play("water_enter");
                        is_to_splash = true;
                    }
                }

                if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                    other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                }
                else {
                    velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                }

                //cF32 diff_y = position_on_level().y - m_water_line_y;
                //velocity_add_y(diff_y / 100.0F);
                //console::log(class_name(), "::collide_y() water diff y: ", diff_y, "\n");

                //console::log(class_name(), "::collide_y() water line y: ", m_water_line_y, "\n");

                if (is_to_splash) {
                    console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                    cVec2F pos = Vec2F{ our_UL.x, other_UL.y } + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F };
                    cVec2F vel = Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) };

                    particle::splash_water(this, pos, vel, 2.0F);
                }
                break;
            }
        }
    }
}