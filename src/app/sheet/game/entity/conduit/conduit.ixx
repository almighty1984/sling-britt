export module entity.conduit;
import anim;
import console;
import entity;
import aabb;
import sprite;

export namespace entity {
    class Conduit : public Object {
    public:
        void update(cF32 dt) override {
            m_input_limit = 1;
            m_time_left_alive = 0;

            m_is_powered = false;
            m_time_left_alive = 0;
            for (auto& i : m_input_objects) {
                if (i->is_powered()) {
                    m_is_powered = true;

                    if (i->time_left_alive() > 0) {
                        m_time_left_alive = U16_MAX;

                        // Make animations correct direction
                        if (m_is_first_update) {
                            m_is_first_update = false;
                            switch (m_type) {
                                case Type::conduit_corner_DL:
                                case Type::conduit_LR: {
                                    if (i->start_offset().x > m_start_offset.x) {
                                        cI16 num_frames = anim::texture_extent(m_current_anim).x / sprite::rect(m_sprite).w;
                                        anim::first_frame(anim("idle"), num_frames - 1);
                                        anim::last_frame(anim("idle"), 0);
                                        anim::is_reverse(anim("idle"), true);
                                        //console::log("entity::Conduit::update() is reverse: ", anim::is_reverse(anim("idle")), "\n");
                                    }
                                    break;
                                }
                                case Type::conduit_corner_DR: {
                                    console::log(to_string(m_type), " ", start_offset().x, " ", start_offset().y, " input type: ", to_string(i->type()), "\n");
                                    if (i->start_offset().y == m_start_offset.y and i->start_offset().x < m_start_offset.x) {
                                        cI16 num_frames = anim::texture_extent(m_current_anim).x / sprite::rect(m_sprite).w;
                                        anim::first_frame(anim("idle"), num_frames - 1);
                                        anim::last_frame(anim("idle"), 0);
                                        anim::is_reverse(anim("idle"), true);
                                        console::log("\n", class_name(), " is reverse: ", anim::is_reverse(anim("idle")), "\n\n");
                                    }
                                    break;
                                }
                                case Type::conduit_UD: {
                                    if (i->start_offset().y < m_start_offset.y) {
                                        cI16 num_frames = anim::texture_extent(m_current_anim).x / sprite::rect(m_sprite).w;
                                        anim::first_frame(anim("idle"), num_frames - 1);
                                        anim::last_frame(anim("idle"), 0);
                                        anim::is_reverse(anim("idle"), true);
                                        //console::log("entity::Conduit::update() is reverse: ", anim::is_reverse(anim("idle")), "\n");
                                    }
                                    break;
                                }
                            }                            
                        }
                        break;
                    }
                }
            }
            if (!m_is_powered) {
                m_time_left_alive = 0;
            }


            sprite_is_hidden(!m_is_powered);
            if (m_time_left_alive > 0) {
                m_time_left_dead = U16_MAX;
                m_current_anim = anim("idle");
                
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
            } else {
                m_current_anim = anim("dead");
            }

            sprite_rect(anim::source(m_current_anim));
        }
    };
}