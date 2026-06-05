module entity.particle.drop;

#define CEILING_DROP_RANDOM_RANGE 100, 400

namespace entity {
    void ParticleDrop::collide_y(aabb::cInfo our, aabb::cInfo other) {
        //if (m_state == state::Type::dead or m_next_state == state::Type::dead) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_center = aabb::center(our.id);
        cVec2F other_center = aabb::center(other.id);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        state::Type other_state = other.owner->state();

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        /*if (other_type == Type::clip_L or other_type == Type::clip_R) {
            collide_x(our, other);
            return;
        }*/

        /*if (m_state == state::Type::slide_wall) {
            collide_x(our, other);
            return;
        }*/

        const bool is_to_left = other_center.x < our_center.x;

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (m_state == state::Type::slide_wall) {
                    if (camera::position.y + other_DR.y > m_wall_drop_on_level_y) {
                        m_wall_drop_on_level_y = camera::position.y + other_DR.y;
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
                console::log(class_name(), "::collide_y() weight: ", m_config.weight(), "\n");

                if (m_config.weight() < 1.0F) {
                    m_time_left_dead = random::number(CEILING_DROP_RANDOM_RANGE);
                }
                else {
                    m_time_left_dead = m_config.time_to_be_dead();
                }

                //position_add_y(-overlap_y);

                position_y(other_DR.y);
                sprite_offset_y(-8.0F);
                velocity_x(0.0F);
                velocity_y(0.0F);
                m_next_state = state::Type::dead;
                if (camera::position.y + other_DR.y > m_wall_drop_on_level_y) {
                    m_wall_drop_on_level_y = camera::position.y + other_DR.y;
                }
                break;
            }
            case Type::brick:
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
                if (other_type == Type::clip_ledge and our_velocity.y < 0.0F and our_center.y > other_DR.y) {
                    return;
                }
                if (our_velocity.y > 0.0F and (other_type == Type::clip_D or
                                               other_type == Type::clip_LD or
                                               other_type == Type::clip_RD)) {
                    return;
                }
                if (m_state == state::Type::dead or m_state == state::Type::ledge) return;

                /*if (m_state == state::Type::idle) {
                    if (other_type == Type::clip_L or other_type == Type::clip_R) {
                        return;
                    }
                }*/

                /*if ((other_type == Type::clip_ledge or other_type == Type::brick) and
                    (our_center.x < other_UL.x or our_center.x > other_DR.x)) {
                    return;
                }*/
                /*if (m_state == state::Type::idle) {
                    if ((our_center.x < other_UL.x or our_center.x > other_DR.x)) {
                        return;
                    }
                }*/

                if (other_type == Type::clip_ledge) {
                    // FIXME: trying to prevent landing on edge when there's a wall underneath
                    /*if (our_velocity.y < 0.0F and our_UL.y < other_DR.y - 2.0F) {
                        return;
                    }
                    if (our_velocity.y > 0.0F and our_DR.y > other_UL.y - 2.0F) {
                        return;
                    }
                    if (our_UL.x + 0.0F < other_UL.x or our_DR.x - 0.0F > other_DR.x) {
                        return;
                    }*/
                }
                
                /*if (other_type == Type::brick and other_state != state::Type::slide_wall and 
                    !(our_UL.x + 4.0F < other_DR.x
                    and
                    our_DR.x - 4.0F > other_UL.x)) {
                    return;
                }*/
                //m_parent = nullptr;
                //move_velocity({});

            //cF32 edge_diff_x = std::abs(our_center.x - (is_to_left ? other_UL.x : other_DR.x));
            //cF32 center_diff_x = std::abs(our_center.x - other_center.x);
            //console::log(m_on_level_wall_x, " ", other_UL.x + camera::position.x, "\n");

                cVec2F other_pos_on_level = is_level_geometry(other_type) ? other.owner->start_offset() :
                                                                            other_UL + camera::position;

                if (other_pos_on_level.y + other_extent.y > m_wall_drop_on_level_y) {
                    m_wall_drop_on_level_y = other_pos_on_level.y + other_extent.y;
                }

                //m_parent = other.owner;
                

                
                /*if (our_DR.x < other_UL.x + other_extent.x / 8.0F or
                    our_UL.x > other_DR.x - other_extent.x / 8.0F) {
                    return;
                }*/
                if (our_velocity.y < 0.0F and
                    other_DR.y < our_UL.y + our_extent.y / 2.0F and
                    other_type != Type::clip_U and
                    other_type != Type::clip_L and
                    other_type != Type::clip_R and
                    other_type != Type::slope_U
                    ) {

                    position_add_y(-overlap_y);

                    velocity_y(0.0F);
                    velocity_x(0.0F);
                    sprite_angle(270.0f);

                    m_next_state = state::Type::dead;

                    m_parent = nullptr;
                    m_is_on_ground = false;
                    
                    console::log("weight: ", m_config.weight(), "\n");

                    if (m_config.weight() < 1.0F) {
                        m_time_left_dead = random::number(CEILING_DROP_RANDOM_RANGE);
                    }

                    console::log(class_name(), "::collide_y() ceiling: ", to_string(other_type), "\n");
                    //console::log("time left dead: ", m_time_left_dead, "\n");
                } else if (our_velocity.y > 0.0F and
                           other_UL.y > our_UL.y + our_extent.y / 2.0F and
                           other_type != Type::clip_LD and
                           other_type != Type::clip_RD and
                           other_type != Type::clip_D) {

                    m_parent = other.owner;
                    m_original_parent_position_on_level.y = other_pos_on_level.y;
                    m_original_parent_position_on_level = other_pos_on_level;
                    if (is_level_geometry(other_type)) {
                        if (other_type != Type::clip_L and other_type != Type::clip_R and other_type != Type::clip_LR) {
                            m_next_state = state::Type::dead;
                        }
                        
                        //m_original_position_on_level = {};
                        //m_original_parent_position_on_level = {};
                        //m_original_position_on_level.y = our_UL.y + camera::position.y;
                        //m_original_position_on_level.x = our_UL.x + camera::position.x;
                    } else {
                        cF32 diff_x = m_is_near_wall_L ? std::abs(our_center.x - other_DR.x) : std::abs(our_center.x - other_UL.x);
                        if (other_type == Type::brick) {
                            //console::log(class_name(), "::collide_y() diff_x: ", diff_x, "\n");
                            if (diff_x <= 3.0F) {
                                return;
                            }
                        }
                        m_original_parent_position_on_level = other_UL + camera::position;
                        m_next_state = state::Type::dead;
                    }

                    if (m_state == state::Type::slide_wall) {
                        //console::log(class_name(), "::collide_y() ", to_string(other_type), "\n");

                        m_time_left_alive = m_config.time_to_be_alive();
                        m_time_in_state = 0;
                        if (our_velocity.x == 0.0F) {
                            if (other_type == Type::clip_L or other_type == Type::clip_R or other_type == Type::clip_LR) {
                                return;
                            }
                        }
                        
                    }
                    sprite_angle(90.0F);
                    position_add_y(-overlap_y);
                    //position_add_y(2.0F);

                    
                    //if (!m_is_near_wall_L and !m_is_near_wall_R) {
                        velocity_y(0.0F);                    
                    //}

                    velocity_x(0.0F);

                    //m_parent = nullptr;

                    m_is_on_ground = true;

                    //sprite_offset_y(-6.0F);

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
                    case Type::slope_L_1x1:   sprite_angle(45.0F);  break;
                    case Type::slope_R_1x1:   sprite_angle(135.0F); break;
                    case Type::slope_L_2x1_0:
                    case Type::slope_L_2x1_1: sprite_angle(67.5F);  break;
                    case Type::slope_R_2x1_0:
                    case Type::slope_R_2x1_1: sprite_angle(112.5F); break;
                }
                position_add_y(-overlap_y);
                position_add_y(2.0F);

                velocity_y(0.0F);
                acceleration_y(0.0F);

                velocity_x(0.0F);

                m_next_state = state::Type::dead;
                m_time_left_dead = m_config.time_to_be_dead();

                m_parent = nullptr;

                m_is_near_wall_L = m_is_near_wall_R = false;
                m_is_on_ground = true;
                m_is_on_slope = true;
                break;
            }
            case Type::particle_drop: {
                if (m_state == state::Type::slide_wall) {
                    add_input(other.owner);
                }
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (m_state == state::Type::dead) return;
                position_add_y(-overlap_y);
                velocity({});
                //move_velocity() = other.owner->velocity();
                m_is_on_ground = true;
                m_is_near_wall_L = m_is_near_wall_R = false;
                m_next_state = state::Type::dead;
                //m_time_left_dead = m_config.time_to_be_dead();

                m_parent = other.owner;

                sprite::angle(m_sprite, 270.0f);
                break;
            }
        }
    }
}