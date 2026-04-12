export module entity.logic_not;
import anim;
import console;
import entity;
import sprite;

export namespace entity {
    class LogicNot : public Object {
    public:
        LogicNot() {
            m_input_limit = 1;
        }
        void update() override {
            //console::log("LogicNot is_powered: ", m_is_powered, "\n");
            for (auto& i : m_input_objects) {
                m_time_left_alive = i->time_left_alive() > 0 ? 0 : U16_MAX;
                if (i->is_powered()) {
                    m_is_powered = true;
                }
            }
            /*if (m_time_left_dead > 0) {
                m_time_left_alive = 0;
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
            }*/
            if (m_time_left_alive > 0) {
                m_time_left_dead = 0;
                m_current_anim = anim("idle");
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
                //console::warning("alive\n");
            } else {
                //console::error("dead\n");

                m_current_anim = anim("dead");
            }
            
            sprite::rect(m_sprite, anim::source(m_current_anim));
        }
    };
}