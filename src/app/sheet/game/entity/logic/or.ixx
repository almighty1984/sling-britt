export module entity.logic_or;
import anim;
import console;
import entity;
import sprite;

export namespace entity {
    class LogicOr : public Object {
    public:
        LogicOr() {
            m_input_limit = 2;
        }
        void update() override {
            m_time_left_alive = 0;
            for (auto& i : m_input_objects) {
                if (i->time_left_alive() > 0) {
                    m_time_left_alive = U16_MAX;
                    break;
                }
            }
            if (m_time_left_alive > 0) {
                m_time_left_dead = 0;
                m_current_anim = anim("idle");

                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                m_current_anim = anim("dead");
            }
            if (m_time_left_dead > 0) {
                m_time_left_alive = 0;
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
            }
            sprite::rect(m_sprite, anim::source(m_current_anim));
        }
    };
}