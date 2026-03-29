module;
#include <string>
#include <unordered_map>
export module anim.trait;
import anim;
import sprite;
import types;

export namespace anim {
    class Trait {
    protected:
        I32 m_current_anim_id = -1;
        std::unordered_map<std::string, I32> m_anim_ids;
    public:
        I32 anim_id(const std::string& s) const {
            auto it = m_anim_ids.find(s);
            if (it != m_anim_ids.end()) {
                return it->second;
            }
            return -1;
        }
        void anim_id(const std::string s, cI32 id) {
            m_anim_ids[s] = id;
        }

        I32 current_anim_id() { return m_current_anim_id; } void current_anim_id(cI32 id) { m_current_anim_id = id; }

        void set_anim(const std::string& s) {
            cI32 id = anim_id(s);
            if (id == -1 or m_current_anim_id == id) return;
            m_current_anim_id = id;
        }
        void reset_anim(const std::string& s) {
            m_current_anim_id = anim_id(s);
            anim::loop(m_current_anim_id, 0);
            anim::source_x(m_current_anim_id, anim::first_frame(m_current_anim_id) * anim::source(m_current_anim_id).w);
        }
    };
}