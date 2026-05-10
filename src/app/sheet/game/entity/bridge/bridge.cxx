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

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cF32 overlap_y = other_DR.y - our_UL.y;

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        switch (other_type) {
            case Type::player:
            case Type::brick:
            case Type::bug:
            case Type::frog: {
                if (other_velocity.y > 0.0F) {

                    //console::log("start position: ", start_position_on_level().y, "\n");
                    //console::log("input position: ", inputs().front()->start_position_on_level().x, " ", inputs().back()->start_position_on_level().x, "\n");

                    m_time_in_state = 0;


                    m_start_offset.y = 4.0F;

                    /*if (m_is_near_wall_L or m_is_near_wall_R) {
                        m_start_offset.y *= 0.5F;
                    }
                    else {
                        for (auto& i : m_input_objects) {
                            if (!i or i->is_near_wall_L() or i->is_near_wall_R()) continue;
                            i->start_offset({ 0.0F, m_start_offset.y * 1.0F });
                            i->time_in_state(0);

                            for (auto& ii : i->m_input_objects) {
                                if (!ii or ii->is_near_wall_L() or ii->is_near_wall_R()) continue;
                                ii->start_offset({ 0.0F, m_start_offset.y * 0.9F });
                                ii->time_in_state(0);

                                for (auto& iii : ii->m_input_objects) {
                                    if (!iii or iii->is_near_wall_L() or iii->is_near_wall_R()) continue;
                                    iii->start_offset({ 0.0F, m_start_offset.y * 0.8F });
                                    iii->time_in_state(0);
                                }
                            }

                        }
                    }*/


                    /*if (other_DR.y + 2.0F < our_UL.y + m_start_offset.y) {
                        return;
                    }*/
                    //m_force.y = other_velocity.y * 4.0F;                

                    other.owner->position_add_y(-overlap_y);

                    other.owner->velocity_y(0.0F);
                    //other.owner->velocity_y(0.0F);
                    other.owner->moved_velocity_x(0.0F);
                    other.owner->moved_velocity_y(0.0F);
                    other.owner->is_on_ground(true);


                    //console::log("inputs: ", m_inputs.size(), "\n");

                }
                break;
            }
        }
    }
    void Bridge::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_alive = U16_MAX;

            //console::log("entity::Bridge::state_idle() source rect h: ", sprite::rect(m_sprite).h,  "\n");
        }

        //deceleration_y( 0.2F );
        
        
        //m_force.y = m_force.y * 0.4F + inputs().front()->force().y * 0.2F + inputs().back()->force().y * 0.2F;

        //diff_y = position_on_level().y - start_position_on_level().y;
        
        //m_force.y -= diff_y * 0.05F;

        //velocity_y(m_force.y);

        //velocity_y(velocity().y * 0.93F);


        

        //if (m_is_near_wall_L or m_is_near_wall_R) {
            //m_start_offset.y = 0.0F;
        //    //sprite::is_hidden(m_sprite, true);
        //}


            if (m_is_near_wall_L) {                
                //sprite::is_hidden(m_sprite, true);
                position_y(start_position().y + m_start_offset.y);
            }
            else if (m_is_near_wall_R) {
                //sprite::is_hidden(m_sprite, true);
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