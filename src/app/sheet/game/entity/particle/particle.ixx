export module entity.particle;
import anim;
import console;
import entity;
import aabb;
import sprite;

export namespace entity {
    class Particle : public Object {
    protected:
        bool m_is_to_erase = false;
        const F32 m_play_bounce_vel = 1.0F;
    public:
        bool is_to_erase() const { return m_is_to_erase; }

        void update(cF32 dt) override {
            if (m_is_first_update) {
                m_is_first_update = false;
                //m_time_left_alive = m_config.time_to_be_alive();
                //m_time_left_dead = m_config.time_to_be_dead();
                //m_start_offset = position_on_level();
            }
            /*if (m_time_left_alive == 0 and m_time_left_dead == 0) {
                m_is_to_erase = true;
                return;
            }*/
            if (m_type == entity::Type::particle_sense and m_parent and m_parent->state() == state::Type::melee) {
                m_is_to_erase = true;
            }
            //if (m_type == Type::particle_melee) {
            //    console::log(class_name(), "::update() ", m_time_left_alive, " ", m_time_left_dead, "\n");
            //}

            reduce_time_left(1);

            velocity_add_y(acceleration().y);

            state_update(dt);

            if (m_time_left_alive > 0) {
                //    if (m_time_left_alive != U16_MAX) {
                //        //--m_time_left_alive;
                //        if (m_time_left_alive == 0) {
                //            m_time_left_dead = m_config.time_to_be_dead();
                //            if (m_config.time_to_be_dead() == 0) {
                //                m_is_to_erase = true;
                //            }
                //        }
                //    }
            } else if (m_time_left_dead > 0) {
                //velocity({});
                m_next_state = state::Type::dead;

                //if (m_time_left_dead != U16_MAX) {
                //    //--m_time_left_dead;
                //    if (m_time_left_dead == 0) {
                //        m_is_to_erase = true;
                //    }
                //}
                if (m_current_anim != anim("dead")) {
                    m_current_anim = anim("dead");
                    anim::source_x(anim("dead"), anim::first_frame(anim("dead")));
                }
            } else if (m_time_left_alive == 0 and m_time_left_dead == 0) {
                m_is_to_erase = true;
            }

            
            
            sprite_rect(anim::source(m_current_anim));
        }
    };
}