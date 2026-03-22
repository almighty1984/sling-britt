module;
#include <sstream>

module entity.bridge;
import aabb;
import console;


F32 diff_y = 0.0F;

namespace entity {
    void Bridge::collide_x(aabb::cInfo our, aabb::cInfo other) {
        collide_y(our, other);
    }
    void Bridge::collide_y(aabb::cInfo our, aabb::cInfo other) {
        
        cType other_type = other.owner->type();

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cF32 overlap_y = other_rect.h - our_rect.y;

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();


        if (other_type == Type::player || other_type == Type::brick || other_type == Type::bug || other_type == Type::frog) {
            if (other.owner->velocity().y > 0.0F) {
                                
                //console::log("start position: ", start_position_on_level().y, "\n");
                //console::log("input position: ", inputs().front()->start_position_on_level().x, " ", inputs().back()->start_position_on_level().x, "\n");
                
                m_time_in_state = 0;

                
                m_start_offset.y = 2.0F;
                
                /*if (m_is_near_wall_L || m_is_near_wall_R) {
                    m_start_offset.y *= 0.5F;
                }
                else {
                    for (auto& i : m_input_objects) {
                        if (!i || i->is_near_wall_L() || i->is_near_wall_R()) continue;
                        i->start_offset({ 0.0F, m_start_offset.y * 1.0F });
                        i->time_in_state(0);

                        for (auto& ii : i->m_input_objects) {
                            if (!ii || ii->is_near_wall_L() || ii->is_near_wall_R()) continue;
                            ii->start_offset({ 0.0F, m_start_offset.y * 0.9F });
                            ii->time_in_state(0);

                            for (auto& iii : ii->m_input_objects) {
                                if (!iii || iii->is_near_wall_L() || iii->is_near_wall_R()) continue;
                                iii->start_offset({ 0.0F, m_start_offset.y * 0.8F });
                                iii->time_in_state(0);
                            }
                        }

                    }
                }*/
                
                
                /*if (other_rect.h + 2.0F < our_rect.y + m_start_offset.y) {
                    return;
                }*/
                //m_force.y = other_velocity.y * 4.0F;                

                other.owner->add_position_y(-overlap_y);

                other.owner->velocity_y(0.0F);
                //other.owner->velocity_y(0.0F);
                other.owner->moved_velocity_y(0.0F);
                other.owner->is_on_ground(true);


                //console::log("inputs: ", m_inputs.size(), "\n");

            }
        }
    }
    void Bridge::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_alive = U16_MAX;

            //console::log("entity::Bridge::state_idle() source rect h: ", sprite::source_rect(m_sprite_id).h,  "\n");
        }

        //deceleration_y( 0.2F );
        
        
        //m_force.y = m_force.y * 0.4F + inputs().front()->force().y * 0.2F + inputs().back()->force().y * 0.2F;

        //diff_y = position_on_level().y - start_position_on_level().y;
        
        //m_force.y -= diff_y * 0.05F;

        //velocity_y(m_force.y);

        //velocity_y(velocity().y * 0.93F);


        

        //if (m_is_near_wall_L || m_is_near_wall_R) {
            //m_start_offset.y = 0.0F;
        //    //sprite::is_hidden(m_sprite_id, true);
        //}


            if (m_is_near_wall_L) {                
                //sprite::is_hidden(m_sprite_id, true);
                position_y(start_position().y + m_start_offset.y);
            }
            else if (m_is_near_wall_R) {
                //sprite::is_hidden(m_sprite_id, true);
                position_y(start_position().y + m_start_offset.y);
            }
            else {
                position_y(start_position().y + m_start_offset.y + m_input_objects.front()->start_offset().y * 0.5F + m_input_objects.back()->start_offset().y * 0.5F);
            }
        //position_y(start_position().y + m_start_offset.y);
        
        if (m_start_offset.y > 0.0F) {
            m_time_in_state++;
            if (m_time_in_state > 2) {
                m_time_in_state = 0;

                m_start_offset.y *= 0.5F;
            }
        } else {
            m_time_in_state = 0;
        }
    }
}