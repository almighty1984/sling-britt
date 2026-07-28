export module entity.track;
import anim;
import console;
import entity;
import aabb;
import sprite;

export namespace entity {
    class Track : public Object {
    public:
        const char* class_name() override { return "entity::Track"; }
        Track() {
            m_input_limit = 1;
        }
        void update(cF32 dt) override {
            if (m_is_first_update) {
                m_is_first_update = false;
                //direction(m_inputs.back()->direction());

                //if (m_type == Type::track_LR) {
                    //console::log(class_name(), "::update() direction: ", direction().x, " ", direction().y, "\n");
                //}
            }
            m_time_left_alive = 0;

            //sprite()->is_hidden = false;

            //console::log("num inputs: ", m_inputs.size(), "\n");
            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : m_inputs) {
                /*if (i->type() == entity::Type::trigger) {
                    console::log("hello trigger\n");
                    if (!i->is_powered()) {
                        m_is_powered = false;
                        break;
                    }
                }*/
                if (i->is_powered()) {
                    m_is_powered = true;
                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }
            sprite_is_hidden(!m_is_powered);

            //console::log("is_powered: ", m_is_powered, "\n");

            if (m_time_left_alive > 0) {
                //sprite()->rect = { 8, 8, 16, 16 };
                m_time_left_dead = U16_MAX;

                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                //sprite()->rect = { 4, 4, 16, 16 };
            }
        }
    };
}