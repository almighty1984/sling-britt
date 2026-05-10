module entity.particle.drop;
import console;
import random;
import camera;
import app.config;

#define CEILING_DROP_RANDOM_RANGE 50, 100

namespace entity {
    void ParticleDrop::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;
            m_is_near_wall_L = m_is_near_wall_R = false;
            m_time_in_state = 0;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            max_velocity_y(start_max_velocity().y);
        }

        //velocity_add_y(acceleration().y);
                
        ++m_time_in_state;
        
        if (m_time_in_state == 2) {
            aabb::is_active(m_aabbs.front(), true);
            aabb::is_active(m_aabbs.back(), false);
            //console::log("angle: ", sprite::angle(m_sprite), "\n");
        }
        F32 radians = std::atan2f(velocity().y, velocity().x);
        if (radians < 0.0F) radians += (2 * PI);
        cF32 degrees = radians * 180 / PI;
        sprite::angle(m_sprite, degrees);
    }
    void ParticleDrop::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_alive = 0;

            if (m_is_on_ground and sprite_angle() == 90.0F) {
                m_time_left_dead = m_time_to_be_dead;
            }
            
            if (!m_is_near_wall_L and !m_is_near_wall_R) {
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }
            }
            //aabb::is_active(m_aabbs.front(), true);
            //aabb::is_active(m_aabbs.back(), false);

            max_velocity_y(start_max_velocity().y);

            F32 abs_vel_y = velocity().y / 1.0F;
            F32 pitch = 1.0F;
            if (abs_vel_y > 1.0F) {
                if (abs_vel_y > 1.5F) {
                    abs_vel_y = 1.5F;
                }
                pitch = abs_vel_y;
            }
            sound_pitch("dead", pitch + random::number(-10, 10) / 100.0F);
            sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });
            sound_play("dead");
        }
        max_velocity_y(0.5F);

        if (m_parent and is_water_line(m_parent->type())) {
            position_y(m_parent->position().y - 0.0F);
        }


        
        if (m_time_left_dead == 1) {
            if (m_is_near_wall_L or m_is_near_wall_R) {
                m_time_left_alive = m_time_to_be_alive;
                //m_time_left_dead = 0;
                m_next_state = state::Type::slide_wall;
            }
            else if (!m_is_on_ground) {
                m_next_state = state::Type::idle;
                m_time_left_alive = m_time_to_be_alive;
                //m_time_left_dead = 0;
            }

            //console::log("sprite_angle(): ", sprite_angle(), "\n");
        }

        if (!m_is_near_wall_L and !m_is_near_wall_R) {
            velocity_y(0.0F);
        }
    }
    void ParticleDrop::state_slide_wall(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;
            m_time_in_state = 0;
            aabb::is_active(m_aabbs.front(), false);
            aabb::is_active(m_aabbs.back(), true);

            if (m_is_near_wall_L) {
                position_x(m_wall_x - 1.0F- camera::position.x);
            }
            else if (m_is_near_wall_R) {
                position_x(m_wall_x - 4.0F - camera::position.x);
            }
        }
        set_anim("idle");
        sprite_angle(90.0F);
        if (m_time_in_state % 10 == 0) {
            max_velocity_y((m_input_objects.size() + 1) * 0.5F * random::real_number(0.2, 0.3));
        }
        if (max_velocity().y > start_max_velocity().y) {
            max_velocity_y(start_max_velocity().y);
        }

        //if (m_parent) {
            //console::log(class_name(), "::state_slide_wall() parent: ", to_string(m_parent->type()), "\n");

            if (velocity().y >= 0.0F and position_on_level().y > m_wall_drop_y and m_time_in_state > 5) {
                //console::log("drop y: ", m_wall_drop_y, "\n");
                //velocity_y(0.0F);
                m_next_state = state::Type::idle;
            }
        //}
        ++m_time_in_state;
    }

    void ParticleDrop::collide_x(aabb::cInfo our, aabb::cInfo other) {
        //if (m_state == state::Type::dead or m_next_state == state::Type::dead) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_velocity = velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        /*if (other_type == Type::clip_ledge
            and
            (our_DR.y < other_UL.y + other_size.y / 8.0F or our_UL.y > other_DR.y - other_size.y / 8.0F)) {

            collide_y(our, other);
            return;
        }*/

        

        switch (other_type) {
            /*case Type::arch_L_1x1:
            case Type::arch_R_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1:*/
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (m_state == state::Type::slide_wall) {
                    return;
                }
                if (m_state == state::Type::dead) {
                    return;
                }
                collide_y(our, other);
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_L:
            case Type::clip_LD:
            case Type::clip_R:
            case Type::clip_RD:
            case Type::clip_LR: {
                if (m_state == state::Type::slide_wall) {
                    //collide_y(our, other);
                    return;
                }
                if (m_state == state::Type::dead) {
                    return;
                }
                /*if ((other_type == Type::clip_L  and velocity().x < 0.0F) or
                    (other_type == Type::clip_LD and velocity().x < 0.0F) or
                    (other_type == Type::clip_R  and velocity().x > 0.0F) or
                    (other_type == Type::clip_RD and velocity().x > 0.0F)) {
                    return;
                }  */              
                /*if ((our_DR.y < other_UL.y + other_extent.y / 8.0F or our_UL.y > other_DR.y - other_extent.y / 8.0F)) {
                    collide_y(our, other);
                    return;
                }*/
                
                if (other_UL.x < our_UL.x) {
                    sprite::angle(m_sprite, 180.0f);
                    m_is_near_wall_L = true;
                    m_wall_x = camera::position.x + other_DR.x;
                } else {
                    sprite::angle(m_sprite, 0.0f);
                    m_is_near_wall_R = true;
                    m_wall_x = camera::position.x + other_UL.x;
                }
                position_add_x(-overlap_x);

                velocity_x(0.0F);
                m_time_left_dead = 10;
                m_time_in_state = 0;
                m_next_state = state::Type::dead;                
                break;
            }
            case Type::player: {
                break;
            }
            case Type::particle_drop: {
                break;
            }            
        }        
    }
    void ParticleDrop::collide_y(aabb::cInfo our, aabb::cInfo other) {
        //if (m_state == state::Type::dead or m_next_state == state::Type::dead) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_velocity = velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        /*if (other_type == Type::clip_L or other_type == Type::clip_R) {
            collide_x(our, other);
            return;
        }*/

        /*if (m_state == state::Type::slide_wall) {
            collide_x(our, other);
            return;
        }*/

        
        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:                    
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {                
                if (m_state == state::Type::slide_wall) {
                    if (camera::position.y + other_DR.y > m_wall_drop_y) {
                        m_wall_drop_y = camera::position.y + other_DR.y;
                    }
                    /*if (our_DR.y >= other_DR.y - 4.0F) {
                        position_add_y(acceleration().y);
                        m_next_state = state::Type::idle;
                    }*/
                    return;
                }
                                
                //console::log("aabb name: ", aabb::to_string(other_name), "\n");
                //if (other_name == aabb::Name::bone or other_DR.y >= our_DR.y) return;
                
                
                if (our_DR.x < other_UL.x + other_extent.x / 8.0F or
                    our_UL.x > other_DR.x - other_extent.x / 8.0F) {
                    return;
                }

                switch (other_type) {
                    case Type::arch_L_1x1: {
                        sprite_angle(45.0F);
                        break;
                    }
                    case Type::arch_L_2x1_0:
                    case Type::arch_L_2x1_1: {
                        sprite_angle(292.5F);
                        break;
                    }
                    case Type::arch_R_1x1: {
                        sprite_angle(225.0F);
                        break;
                    }
                    case Type::arch_R_2x1_0:
                    case Type::arch_R_2x1_1: {
                        sprite_angle(247.5F);
                        break;
                    }
                }

                m_time_left_dead = random::number(CEILING_DROP_RANDOM_RANGE);
                
                //position_add_y(-overlap_y);

                position_y(other_DR.y);
                sprite_offset_y(-8.0F);
                velocity_x(0.0F);
                velocity_y(0.0F);
                m_next_state = state::Type::dead;
                if (camera::position.y + other_DR.y > m_wall_drop_y) {
                    m_wall_drop_y = camera::position.y + other_DR.y;
                }
                break;
            }            
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_UD:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_D:
            case Type::clip_L:
            case Type::clip_R:
            case Type::clip_LR:
            case Type::clip_U:
            case Type::slope_U: {                
                if (other_type == Type::clip_ledge and our_UL.y < other_UL.y) {
                    m_next_state = state::Type::dead;
                    m_is_on_ground = true;
                    m_is_near_wall_L = m_is_near_wall_R = false;
                }
                else if (other_type == Type::slope_U or other_type == Type::clip_U or other_type == Type::clip_UD) {
                    m_next_state = state::Type::dead;
                    m_is_on_ground = true;
                    m_is_near_wall_L = m_is_near_wall_R = false;
                }
                else if (m_state == state::Type::slide_wall) {                    
                    //console::log(class_name(), "::collide_y() slide wall ", m_wall_drop_y, "\n");
                    if (camera::position.y + other_DR.y > m_wall_drop_y) {                        
                        m_wall_drop_y = camera::position.y + other_DR.y;
                    }
                    m_time_in_state = 0;
                    //m_parent = other.owner;
                    /*if ((other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD or other_type == Type::clip_ledge)
                        and our_DR.y >= other_DR.y - 4.0F) {
                        position_add_y(acceleration().y);
                        m_next_state = state::Type::idle;
                    }*/
                    return;
                }
                if (our_DR.x < other_UL.x + other_extent.x / 8.0F or
                    our_UL.x > other_DR.x - other_extent.x / 8.0F) {
                    return;
                }
                if (our_velocity.y < 0.0F and
                    other_DR.y < our_UL.y + 4.0F and
                    other_type != Type::clip_U and                    
                    other_type != Type::clip_L and
                    other_type != Type::clip_R and
                    other_type != Type::slope_U) {

                    position_add_y(-overlap_y);

                    velocity_y(0.0F);
                    velocity_x(0.0F);
                    sprite_angle(270.0f);

                    m_next_state = state::Type::dead;

                    //m_parent = nullptr;
                    m_is_on_ground = false;
                    m_time_left_dead = random::number(CEILING_DROP_RANDOM_RANGE);

                    console::log("ceiling: ", to_string(other_type), "\n");
                    //console::log("time left dead: ", m_time_left_dead, "\n");
                }
                else if (our_velocity.y > 0.0F and
                         other_UL.y > our_DR.y - 4.0F and
                         other_type != Type::clip_LD and
                         other_type != Type::clip_RD and
                         other_type != Type::clip_D) {
                    sprite_angle(90.0F);
                    position_add_y(-overlap_y);
                    //position_add_y(2.0F);

                    if (!m_is_near_wall_L and !m_is_near_wall_R) {
                        velocity_y(0.0F);
                    }

                    velocity_x(0.0F);
                    m_next_state = state::Type::dead;
                    //m_parent = nullptr;

                    m_is_on_ground = true;

                    sprite_offset_y(-6.0F);
                }
                break;
            }
            case Type::slope_L_1x1:                    
            case Type::slope_R_1x1:                    
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:                    
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (other_UL.y <= our_UL.y or our_velocity.y < 0.0F) return;
                switch (other_type) {
                    case Type::slope_L_1x1:   sprite::angle(m_sprite, 45.0F);  break;
                    case Type::slope_R_1x1:   sprite::angle(m_sprite, 135.0F); break;
                    case Type::slope_L_2x1_0:
                    case Type::slope_L_2x1_1: sprite::angle(m_sprite, 67.5F);  break;
                    case Type::slope_R_2x1_0:
                    case Type::slope_R_2x1_1: sprite::angle(m_sprite, 112.5F); break;
                }
                position_add_y(-overlap_y);
                position_add_y(2.0F);

                velocity_y(0.0F);
                acceleration_y(0.0F);

                velocity_x(0.0F);

                m_next_state = state::Type::dead;
                m_time_left_dead = m_time_to_be_dead;

                //m_parent = nullptr;
                
                m_is_near_wall_L = m_is_near_wall_R = false;
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::particle_drop: {
                add_input(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (m_state == state::Type::dead) return;
                position_add_y(-overlap_y);
                velocity({});
                //moved_velocity() = other.owner->velocity();
                m_is_on_ground = true;
                m_is_near_wall_L = m_is_near_wall_R = false;
                m_next_state = state::Type::dead;
                m_time_left_dead = m_time_to_be_dead;

                m_parent = other.owner;

                sprite::angle(m_sprite, 270.0f);
                break;
            }
        }    
    }
}