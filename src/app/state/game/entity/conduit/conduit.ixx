export module entity.conduit;
import anim;
import console;
import entity;
import aabb;
import sprite;

export namespace entity {
    class Conduit : public Object {
    public:
        void update() override {
            m_input_limit = 1;
            m_time_left_alive = 0;

            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : m_input_objects) {
                if (i->is_powered()) {
                    m_is_powered = true;

                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;

                        if (m_type == Type::conduit_LR) {
                            if (i->start_offset().x > m_start_offset.x) {
                                cI16 num_frames = anim::texture_size(m_current_anim_id).x / sprite::source_rect(m_sprite_id).w;
                                anim::first_frame(anim_id("idle"), num_frames - 1);
                                anim::last_frame(anim_id("idle"), 0);
                                anim::is_reverse(anim_id("idle"), true);
                                //console::log("entity::Conduit::update() is reverse: ", anim::is_reverse(anim_id("idle")), "\n");
                            }
                        }
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }

            sprite::is_hidden(m_sprite_id, !m_is_powered);
            if (m_time_left_alive > 0) {
                m_time_left_dead = U16_MAX;
                m_current_anim_id = anim_id("idle");
                
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                m_current_anim_id = anim_id("dead");
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));
        }
    };
}