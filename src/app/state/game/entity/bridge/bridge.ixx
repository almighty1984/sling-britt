module;
#include <cmath>
#include <sstream>
export module entity.bridge;
import anim;
import console;
import aabb;
import entity;
import sprite;

export namespace entity {
    class Bridge : public Object {
    public:
        Bridge() {
            m_state = m_next_state = m_start_state = State::idle;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_idle()    override;
        
        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
            }

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
                case State::idle: state_idle(); break;
                default:                        break;
            }
        }
    };
}