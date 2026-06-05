module entity.particle.drop;

namespace entity {
    void ParticleDrop::collide_x(aabb::cInfo our, aabb::cInfo other) {
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

        state::cType other_state = other.owner->state();

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
            case Type::brick:
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_L:
            case Type::clip_LD:
            case Type::clip_R:
            case Type::clip_RD:
            case Type::clip_LR: {
                if (m_state == state::Type::dead or m_state == state::Type::ledge) return;
                if (our_velocity.x < 0.0F and other_type == Type::clip_L or
                    our_velocity.x > 0.0F and other_type == Type::clip_R or
                    our_velocity.x < 0.0F and our_velocity.y > 0.0F and other_type == Type::clip_LD or
                    our_velocity.x > 0.0F and our_velocity.y > 0.0F and other_type == Type::clip_RD) {
                    return;
                }
                if (other_type == Type::clip_ledge) {
                    if (our_center.y < other_UL.y) {
                        return;
                    }
                }
                /*if ((our_DR.y < other_UL.y + other_extent.y / 8.0F or our_UL.y > other_DR.y - other_extent.y / 8.0F)) {
                    collide_y(our, other);
                    return;
                }*/

                //if (m_original_parent_position_on_level.x == 0.0F) {
                    //console::log(class_name(), "::collide_x() ", to_string(other_type), "\n");

                    //if (is_level_geometry(other_type)) {
                    //    m_original_parent_position_on_level.x = other.owner->start_offset().x;
                    //} else {
                    //    m_original_parent_position_on_level.x = other_UL.x + camera::position.x;
                    //}

                    //m_original_position_on_level.x = our_UL.x + camera::position.x - overlap_x;
                //}
                /*if (our_DR.x > other_DR.x) {
                    m_is_near_wall_R = true;
                }
                if (our_UL.x < other_UL.x) {
                    m_is_near_wall_R = true;
                }*/

                cVec2F other_pos_on_level = is_level_geometry(other_type) ? other.owner->start_offset() :
                    other_UL + camera::position;
                if (other_pos_on_level.y + other_extent.y > m_wall_drop_on_level_y) {
                    m_wall_drop_on_level_y = other_pos_on_level.y + other_extent.y;
                }
                
                if (m_state == state::Type::slide_wall) {
                    collide_y(our, other);
                    return;
                } else if (m_state == state::Type::idle) {
                    
                } else if (m_state == state::Type::ledge) {
                    return;
                }

                m_parent = other.owner;

                position_add_x(-overlap_x);
                if (other_UL.x < our_UL.x) {
                    sprite_angle(180.0F);
                    m_is_near_wall_L = true;                    

                    //position_add_x(1.0F);
                } else {
                    sprite_angle(0.0F);
                    m_is_near_wall_R = true;

                    //position_add_x(1.0F);
                }
                velocity_x(0.0F);

                //m_time_left_dead = random::number(80, 100);
                //m_time_left_dead = 100;

                m_time_in_state = 0;
                m_next_state = state::Type::ledge;
                //m_time_left_alive = m_config.time_to_be_alive();


                if (m_config.weight() < 1.0F) {
                    m_time_to_be_in_state = 20;
                }
                else {
                    m_time_to_be_in_state = 1;
                }


                //if (is_level_geometry(other_type)) {
                //    //m_original_parent_position_on_level.x = other.owner->start_offset().x;
                //    m_original_parent_position_on_level = other.owner->start_offset();
                //} else {
                //    //m_original_parent_position_on_level.x = other_UL.x + camera::position.x;
                //    m_original_parent_position_on_level = other_UL + camera::position + other_velocity;
                //}

                F32 offset_x = 0.0F;
                /*if (m_is_near_wall_L)
                    offset_x = -4.0F;
                if (m_is_near_wall_R)
                    offset_x = 4.0F;*/

                //m_original_position_on_level.x = our_UL.x + camera::position.x - overlap_x + offset_x;
                //m_original_position_on_level.y = our_UL.y + camera::position.y;

                break;
            }
            case Type::player: {
                break;
            }
            case Type::particle_drop: {
                if (m_state == state::Type::slide_wall and other_state == state::Type::slide_wall) {
                    //collide_y(our, other);
                    
                    //position_x(other_UL.x - camera::position.x);
                    //cF32 diff_x = our_center.x - other_center.x;
                    //position_add_x(diff_x);
                    //console::log("diff_x: ", diff_x, "\n");
                    //velocity_x(diff_x * 0.5F);
                }
                break;
            }
        }
    }
}