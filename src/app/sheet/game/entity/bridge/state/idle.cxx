module entity.bridge;
import aabb;
import console;
import camera;
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
        }

        if (m_roots.size() < 2) return;
        if (m_children.empty()) return;

        if (m_is_near_wall_L) {
            //sprite::is_hidden(m_sprite, true);
            //position_y(start_position().y + m_start_offset.y);
        } else if (m_is_near_wall_R) {
            //sprite::is_hidden(m_sprite, true);
            //position_y(start_position().y + m_start_offset.y);
        } else {
            if (!m_inputs.empty()) {
                m_weight = 0.0F;
                for (auto& i : m_inputs) {
                    m_weight += i->weight() * 1.0F;
                }
                m_force.y = m_weight;
                //console::log("m_weight: ", m_weight, "\n");
            }
            cF32 factor = 1.0F / 3.0F;
            m_force.y = m_force.y * factor + m_children.front()->force().y * factor + m_children.back()->force().y * factor;

            cF32 diff_y = (position_on_level().y + m_force.y) - start_position_on_level().y;

            if (diff_y != 0.0F) {
                F32 tension = diff_y * diff_y;
                m_up_force = tension / (m_offset_from_L * m_offset_from_R / 200.0F);
                console::log("up force: ", m_up_force, "\n");
                position_y(start_position_on_level().y + diff_y - camera::position.y - m_up_force);
            }
        }
        m_inputs.clear();
    }

}