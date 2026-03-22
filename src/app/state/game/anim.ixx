module;
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>

export module anim;
import console;
import types;

struct Anim {
private:
    I32        m_id;
    anim::Type m_type;
    F32        m_speed;
    U16        m_loop, m_loops;
    U16        m_first_frame, m_last_frame;
    RectI      m_source;
    Vec2U      m_texture_size;
    bool       m_is_reverse;
    F32        m_timer;
public:
    Anim() : m_id(-1),
             m_type(anim::Type::none),
             m_speed(1.0F),
             m_loop(0), m_loops(0),
             m_first_frame(0), m_last_frame(0),
             m_source(0, 0, 16, 16),
             m_texture_size(16, 16),
             m_is_reverse(false),
             m_timer(0.0F) {
    }

    I32 id()             const { return m_id;           }   void id(cI32 i)            { m_id           = i; }
    anim::Type type()    const { return m_type;         }   void type(anim::cType t)   { m_type         = t; }
    F32 speed()          const { return m_speed;        }   void speed(cF32 s)         { m_speed        = s; }
    U16 loop()           const { return m_loop;         }   void loop(cU16 l)          { m_loop         = l; }
    U16 loops()          const { return m_loops;        }   void loops(cU16 l)         { m_loops        = l; }
    U16 first_frame()    const { return m_first_frame;  }   void first_frame(cU16 f)   { m_first_frame  = f; }
    U16 last_frame()     const { return m_last_frame;   }   void last_frame(cU16 f)    { m_last_frame   = f; }
    RectI source()       const { return m_source;       }   void source(cRectI r)      { m_source       = r; }
    void source_x(cF32 x) { m_source.x = x; }
    void source_y(cF32 y) { m_source.y = y; }
    void source_w(cF32 w) { m_source.w = w; }
    void source_h(cF32 h) { m_source.h = h; }
    Vec2U texture_size() const { return m_texture_size; }   void texture_size(Vec2U s) { m_texture_size = s; }
    void texture_size_x(cF32 x) { m_texture_size.x = x; }
    void texture_size_y(cF32 y) { m_texture_size.y = y; }
    bool is_reverse()    const { return m_is_reverse;   }   void is_reverse(bool q)    { m_is_reverse   = q; }

    cU16 num_frames()    const { return m_texture_size.x / m_source.w; }
    cU16 current_frame() const { return m_source.x / m_source.w; }

    bool is_last_frame() const { return (current_frame() == m_last_frame); }

    void update() {
        m_timer += m_speed;
        if (m_timer < 1.0F) {
            return;
        }
        m_timer = 0.0F;

        if (m_is_reverse) {
            m_source.x -= m_source.w;

            //console::log("Anim::update() source.x: ", source.x, "\n");
            if (m_source.x < 0) {
                if (m_loops == 0) {
                    m_source.x = m_first_frame * m_source.w;
                } else {
                    m_loop += 1;
                    if (m_loop >= m_loops) {
                        m_source.x = 0;
                        m_loop = m_loops;
                    } else {
                        m_source.x = m_first_frame * m_source.w;
                    }
                }
            }
            return;
        }

        m_source.x += m_source.w;

        if (m_source.x >= m_texture_size.x) {
            if (m_loops == 0) {
                m_source.x = m_first_frame * m_source.w;
            } else {
                m_loop += 1;
                if (m_loop >= m_loops) {
                    m_source.x = m_texture_size.x - m_source.w;
                    m_loop = m_loops;
                } else {
                    m_source.x = 0;
                }
            }
        }
        //console::log("source: ", source.x, "\n");
    }
};

std::vector<Anim*>  s_anims;
std::vector<I32>    s_unused_ids;

export namespace anim {
    constexpr bool    is_valid(size_t i) { return (i >= 0 && i < s_anims.size() && s_anims.at(i)) ? true : false; }

    Anim* get(cI32 i)      { return is_valid(i) ? s_anims.at(i) : nullptr; }
    size_t  size()           { return s_anims.size(); }

    Type  type(cI32 i)          { return is_valid(i) ? s_anims.at(i)->type()          :       Type::none; }
    F32   speed(cI32 i)         { return is_valid(i) ? s_anims.at(i)->speed()         :             0.0F; }
    U16   loop(cI32 i)          { return is_valid(i) ? s_anims.at(i)->loop()          :                0; }
    U16   loops(cI32 i)         { return is_valid(i) ? s_anims.at(i)->loops()         :                0; }
    U16   first_frame(cI32 i)   { return is_valid(i) ? s_anims.at(i)->first_frame()   :                0; }
    U16   last_frame(cI32 i)    { return is_valid(i) ? s_anims.at(i)->last_frame()    :                0; }
    RectI source(cI32 i)        { return is_valid(i) ? s_anims.at(i)->source()        : RectI{0,0,16,16}; }
    Vec2U texture_size(cI32 i)  { return is_valid(i) ? s_anims.at(i)->texture_size()  :          Vec2U{}; }    
    cU16  num_frames(cI32 i)    { return is_valid(i) ? s_anims.at(i)->num_frames()    :                0; }
    cU16  current_frame(cI32 i) { return is_valid(i) ? s_anims.at(i)->current_frame() :                0; }
    bool  is_last_frame(cI32 i) { return (current_frame(i) == last_frame(i));                             }
    bool  is_reverse(cI32 i)    { return is_valid(i) ? s_anims.at(i)->is_reverse()    :            false; }

    void type(cI32 i, cType t)          { if (is_valid(i)) s_anims.at(i)->type(t);           }
    void speed(cI32 i, cF32 s)          { if (is_valid(i)) s_anims.at(i)->speed(s);          }
    void loop(cI32 i, cU16 l)           { if (is_valid(i)) s_anims.at(i)->loop(l);           }
    void loops(cI32 i, cU16 l)          { if (is_valid(i)) s_anims.at(i)->loops(l);          }
    void first_frame(cI32 i, cU16 f)    { if (is_valid(i)) s_anims.at(i)->first_frame(f);    }
    void last_frame(cI32 i, cU16 f)     { if (is_valid(i)) s_anims.at(i)->last_frame(f);     }
    void source(cI32 i, cRectI s)       { if (is_valid(i)) s_anims.at(i)->source(s);         }
    void source_x(cI32 i, cI32 x)       { if (is_valid(i)) s_anims.at(i)->source_x(x);       }
    void source_y(cI32 i, cI32 y)       { if (is_valid(i)) s_anims.at(i)->source_y(y);       }
    void source_w(cI32 i, cI32 w)       { if (is_valid(i)) s_anims.at(i)->source_w(w);       }
    void source_h(cI32 i, cI32 h)       { if (is_valid(i)) s_anims.at(i)->source_h(h);       }
    void texture_size(cI32 i, cVec2U s) { if (is_valid(i)) s_anims.at(i)->texture_size(s);   }
    void texture_size_x(cI32 i, cU32 x) { if (is_valid(i)) s_anims.at(i)->texture_size_x(x); }
    void texture_size_y(cI32 i, cU32 y) { if (is_valid(i)) s_anims.at(i)->texture_size_y(y); }
    void is_reverse(cI32 i, bool q)     { if (is_valid(i)) s_anims.at(i)->is_reverse(q);     }

    I32 make() {
        Anim* object = new Anim;
        if (!s_unused_ids.empty()) {
            object->id(s_unused_ids.back());
            s_unused_ids.pop_back();                
            if (!s_anims.empty() && object->id() >= 0 && object->id() < s_anims.size() && s_anims.at(object->id())) {
                delete s_anims.at(object->id());
                //s_anims.at(object->id) = nullptr;
            }
            s_anims.at(object->id()) = object;
        } else {
            object->id(s_anims.size());
            s_anims.emplace_back(object);
        }
        return object->id();
    }
    bool erase(cI32 i) {
        if (i < 0 || !is_valid(i)) {
            console::log("anim::erase ", i, " can't do it!\n");
            return false;
        }
        delete s_anims.at(i);
        s_anims.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
    }
    void update() {
        for (const auto& i : s_anims) {
            if (i) i->update();
        }
    }
    void clear() {
        for (size_t i = 0; i < s_anims.size(); ++i) {
            erase(i);
        }
        s_anims.clear();
        s_unused_ids.clear();
    }
}