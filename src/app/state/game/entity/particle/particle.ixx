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
    public:        
        bool is_to_erase() const { return m_is_to_erase; }

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_time_left_alive = m_time_to_be_alive;
                m_time_left_dead = m_time_to_be_dead;
                //m_start_offset = position_on_level();
            }
            /*if (m_time_left_alive == 0 and m_time_left_dead == 0) {
                m_is_to_erase = true;
                return;
            }*/
            if (m_type == entity::Type::particle_sense and m_parent and m_parent->state() == state::Type::melee) {
                m_is_to_erase = true;
            }
            if (m_time_left_hurt > 0) {
                --m_time_left_hurt;
            }
            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            velocity_add_y(acceleration().y);

            switch (m_state) {
                case state::Type::idle: state_idle();  break;                
                case state::Type::dead: state_dead();  break;
                default:                               break;
            }
            if (m_time_left_alive > 0) {
                --m_time_left_alive;
                if (m_time_left_alive == 0) {
                    m_time_left_dead = m_time_to_be_dead;
                    if (m_time_to_be_dead == 0) {
                        m_is_to_erase = true;
                    }
                }
                if (m_current_anim_id != anim_id("idle")) {
                    m_current_anim_id = anim_id("idle");
                    anim::source_x(anim_id("idle"), anim::first_frame(anim_id("idle")));                    
                }
            }
            else if (m_time_left_dead > 0) {
                velocity({});

                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                    if (m_time_left_dead == 0) {
                        m_is_to_erase = true;
                    }
                }
                if (m_current_anim_id != anim_id("dead")) {
                    m_current_anim_id = anim_id("dead");
                    anim::source_x(anim_id("dead"), anim::first_frame(anim_id("dead")));
                }
            }
            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));
        }
    };
}