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
        I32 m_current_anim = -1;
        std::unordered_map<std::string, I32> m_anims;
    public:
        I32 anim(const std::string& s) const {
            auto it = m_anims.find(s);
            if (it != m_anims.end()) {
                return it->second;
            }
            return -1;
        }
        void anim(const std::string s, cI32 id) {
            m_anims[s] = id;
        }

        I32 current_anim() { return m_current_anim; } void current_anim(cI32 id) { m_current_anim = id; }

        void set_anim(const std::string& s) {
            cI32 id = anim(s);
            if (id == -1 or m_current_anim == id) return;
            m_current_anim = id;
        }
        void reset_anim(const std::string& s) {
            m_current_anim = anim(s);
            anim::loop(m_current_anim, 0);
            anim::source_x(m_current_anim, anim::first_frame(m_current_anim) * anim::source(m_current_anim).w);
        }
    };
}