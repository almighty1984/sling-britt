module entity.particle.bubble;

namespace entity {
    void ParticleBubble::state_idle(cF32 dt) {
        //console::log("entity::ParticleBubble::state_idle() direction: ", m_direction.x, "\n");
        //++m_time_in_state;
        //if (m_time_in_state < 10) return;

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            direction_x(velocity().x < 0.0F ? -1.0F : 1.0F);
        }

        if (direction().x == 0.0F) {    // hit ceiling
            /*if (velocity().x < 0.0F) {
                velocity_add_x(-acceleration().x);
            } else if (velocity().x > 0.0F) {
                velocity_add_x( acceleration().x);
            }*/
        }
        else {
            if (direction().x < 0.0F) {
                velocity_add_x(-acceleration().x);
            } else if (direction().x > 0.0F) {
                velocity_add_x( acceleration().x);
            }
            if (velocity().x < -0.4F and direction().x < 0.0F) {                
                direction_x(1.0F);
            } else if (velocity().x > 0.4F and direction().x > 0.0F) {
                direction_x(-1.0F);
            }
            //console::log(class_name(), "::state_idle() direction x: ", direction().x, "\n");
        }
    }
}