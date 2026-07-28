module entity.bridge;
import aabb;
import console;
import random;

namespace entity {    
    void Bridge::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_alive = U16_MAX;

            if (m_roots.size() == 2) {
                m_offset_from_L = position_on_level().x - m_roots.front()->position_on_level().x;
                m_offset_from_R = m_roots.back()->position_on_level().x - position_on_level().x;
            }

            //console::log("entity::Bridge::state_idle() source rect h: ", sprite::rect(m_sprite).h,  "\n");
        }

        //m_time_in_state++;
        //if (m_time_in_state > 10) {
        //    m_time_in_state = 0;
        //    m_inputs.clear();
        //}

        //return;
        //deceleration_y( 0.2F );
        
        
        //m_force.y = m_force.y * 0.4F + inputs().front()->force().y * 0.2F + inputs().back()->force().y * 0.2F;

        //F32 diff_y = position_on_level().y - start_position_on_level().y;
        
        //m_force.y -= diff_y * 0.05F;

        //velocity_y(m_force.y);

        //velocity_y(velocity().y * 0.93F);

        //if (m_is_near_wall_L or m_is_near_wall_R) {
            //m_start_offset.y = 0.0F;
        //    //sprite::is_hidden(m_sprite, true);
        //}

        //console::log("bridge inputs: ", m_inputs.size(), "\n");

        //position_y(start_position().y + m_force.y * 0.1F);
        //m_force.y = 0.0F;

        //console::log("force y: ", m_force.y, "\n");

        
        

        //position_y(start_position().y + m_force.y);
        
        //m_force.y = 0.0F;
        //if (velocity().y > 0.0F) {
            //velocity_add_y(diff_y);
        //}
        if (m_roots.size() < 2) return;

        if (m_children.empty()) return;
        ////m_time_in_state++;
        ////if (m_time_in_state < 2) return;        
        ////m_time_in_state = 0;


                //m_force.y = 0.0F;
                //for (auto& i : m_inputs) {
                //    m_force.y += i->weight();
                //}

        //F32 offset_from_L = position_on_level().x - m_roots.front()->position_on_level().x;
        //F32 offset_from_R = m_roots.back()->position_on_level().x - position_on_level().x;


            if (m_is_near_wall_L) {
                //sprite::is_hidden(m_sprite, true);
                //position_y(start_position().y + m_start_offset.y);
            }
            else if (m_is_near_wall_R) {
                //sprite::is_hidden(m_sprite, true);
                //position_y(start_position().y + m_start_offset.y);
            }
            else {
                cF32 factor = 1.0F / 3.0F;
                //m_force.y = m_inputs.front()->force().y * 0.5F + m_inputs.back()->force().y * 0.5F;


                if (!m_inputs.empty()) {
                    //m_force.y = 0.0F;
                    //for (auto& i : m_inputs) {
                    //    m_force.y += i->weight() * 4.0F;
                    //}
                    m_force.y = m_inputs.size() * 2.0F;
                } else {
                    //m_force.y *= 0.99F;
                }

                m_force.y = m_force.y * factor + m_children.front()->force().y * factor + m_children.back()->force().y * factor
                    ;

                cF32 next_y = start_position().y + m_force.y;

                cF32 diff_y = next_y - position().y;
                
                //cF32 up_force = (start_position_on_level().y - position_on_level().y);
                //m_force.y += up_force;
                //console::log("up_force: ", up_force, "\n");
                
                //if (std::abs(diff_y) > 0.2F) {

                    //console::log(class_name(), " diff y: ", diff_y, "\n");

                    position_y(next_y);
                //}
            }
        //position_y(start_position().y + m_start_offset.y);
        
        //if (m_start_offset.y > 0.0F) {
        //    
        //    if (m_time_in_state > 2) {
        //        m_time_in_state = 0;

        //        //m_start_offset.y *= 0.5F;
        //    }
        //} else {
        //    m_time_in_state = 0;
        //}

        //if (m_force.y != 0.0F) {
        //    console::log(class_name(), "::state_idle() force y: ", m_force.y, "\n");
        //}
        //m_force.y *= 0.9F;



            m_time_in_state++;
            if (m_time_in_state > 10) {
                m_time_in_state = 0;
                m_force.y *= 0.99F;
                

                    //console::log(class_name(), "::state_idle() inputs: ", m_inputs.size(), "\n");                    

                m_inputs.clear();
            }

    }
}