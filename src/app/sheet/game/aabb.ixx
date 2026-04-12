export module aabb;
import console;
import line;
import sprite;
import entity;
import types;
import transform;
import window;
import std;

//export namespace entity {
    //class Object;
//}

class AABB {
    I32 m_id = -1;
    bool m_is_active = true;

    I32 m_transform = -1;

    I32 m_up_line    = -1,
        m_down_line  = -1,
        m_left_line  = -1,
        m_right_line = -1;

    RectF m_rect = { 0.0F, 0.0F, 0.0F, 0.0F };

    Color m_color = { 0, 0, 0 },
          m_start_color = { 0, 0, 0 };


    entity::Object* m_owner = nullptr;
    //U16 time_left_colliding = 0;

    aabb::Name m_name = aabb::Name::none;

public:
    I32 id()                const { return m_id;        } void id(cI32 id)              { m_id = id;           }
    I32 transform()      const { return m_transform; } void transform(cI32 id)    { m_transform = id;    }
        
    entity::Object* owner() const { return m_owner;     } void owner(entity::Object* o) { m_owner = o;         }
    aabb::Name name()       const { return m_name;      } void name(aabb::cName n)      { m_name = n;          }

    AABB() = delete;
    AABB(cI32 transform, cRectF rect) {
        m_transform = transform;
        m_rect = rect;

        m_up_line = line::make({ rect.x, rect.y }, { rect.x + rect.w, rect.y });
        m_down_line = line::make({ rect.x, rect.y + rect.h }, { rect.x + rect.w, rect.y + rect.h });
        m_left_line = line::make({ rect.x, rect.y }, { rect.x,          rect.y + rect.h });
        m_right_line = line::make({ rect.x + rect.w, rect.y }, { rect.x + rect.w, rect.y + rect.h });

        line::layer(m_up_line, NUM_VISIBLE_LAYERS - 1);
        line::layer(m_down_line, NUM_VISIBLE_LAYERS - 1);
        line::layer(m_left_line, NUM_VISIBLE_LAYERS - 1);
        line::layer(m_right_line, NUM_VISIBLE_LAYERS - 1);
        line::transform(m_up_line, m_transform);
        line::transform(m_down_line, m_transform);
        line::transform(m_left_line, m_transform);
        line::transform(m_right_line, m_transform);
        line::is_aabb(m_up_line, true);
        line::is_aabb(m_down_line, true);
        line::is_aabb(m_left_line, true);
        line::is_aabb(m_right_line, true);        
    }
    ~AABB() {
        line::erase(m_up_line);
        line::erase(m_down_line);
        line::erase(m_left_line);
        line::erase(m_right_line);
        m_transform = -1;
    }
       
    bool is_active() const { return m_is_active; }    
    void is_active(bool q) {
        m_is_active = q;
        line::is_hidden(m_up_line, !q);
        line::is_hidden(m_down_line, !q);
        line::is_hidden(m_left_line, !q);
        line::is_hidden(m_right_line, !q);
    }
    RectF rect() const { return m_rect; }
    void rect(cRectF r) {
        m_rect = r;
        line::set(m_up_line,    { r.x, r.y }, { r.x + r.w, r.y });
        line::set(m_down_line,  { r.x, r.y + r.h }, { r.x + r.w, r.y + r.h });
        line::set(m_left_line,  { r.x, r.y }, { r.x,          r.y + r.h });
        line::set(m_right_line, { r.x + r.w, r.y }, { r.x + r.w, r.y + r.h });        
    }

    cVec2F point_0() { return transform::position(m_transform) + Vec2F{ m_rect.x,  m_rect.y }; }
    cVec2F point_1() { return transform::position(m_transform) + Vec2F{ m_rect.x + m_rect.w, m_rect.y }; }
    cVec2F point_2() { return transform::position(m_transform) + Vec2F{ m_rect.x,  m_rect.y + m_rect.h }; }
    cVec2F point_3() { return transform::position(m_transform) + Vec2F{ m_rect.x + m_rect.w, m_rect.y + m_rect.h }; }

    const std::vector<Vec2F> points() {
        return { point_0(), point_1(), point_2(), point_3() };
    }    

    Color color() const { return m_color; }    
    void color(Color c) {
        m_color = c;
        line::color(m_up_line, c);
        line::color(m_down_line, c);
        line::color(m_left_line, c);
        line::color(m_right_line, c);
    }
    Color start_color() const { return m_start_color; }
    void start_color(Color c) {
        m_start_color = c;
        line::start_color(m_up_line, c);
        line::start_color(m_down_line, c);
        line::start_color(m_left_line, c);
        line::start_color(m_right_line, c);
    }

    //void update() {
        /*if (time_left_colliding > 0) {
            --time_left_colliding;
            color({ 255,0,0 });
        } else {
            color(start_color());
        }*/

        /*if (overlap != 0.0F) {
            color = { 255,0,0 };
            console::log("overlap: ", overlap.x, " ", overlap.y, "\n");
            overlap = 0.0F;
        }
        else {
            color = start_color;
        }*/
        /*line::color(m_up_line, color);
        line::color(m_down_line, color);
        line::color(m_left_line, color);
        line::color(m_right_line, color);*/

        //console::log("color: ", (int)color.r, "\n");

        /*for (auto& i : quad_tree_node_ids) {
            console::log("id: ", i, "\n");
        }*/
    //}

    void draw(std::unique_ptr<Window>& window) {
        if (!m_is_active) return;

        line::draw(window, m_up_line);
        line::draw(window, m_down_line);
        line::draw(window, m_left_line);
        line::draw(window, m_right_line);
    }
};

std::vector<AABB*> s_aabbs;
std::vector<I32>   s_unused_ids;

export namespace aabb {
    constexpr bool is_valid(size_t i) { return (i >= 0 and i < s_aabbs.size() and s_aabbs.at(i)) ? true : false; }

    size_t  size() { return s_aabbs.size(); }

    Color            color(cI32 i)       { return is_valid(i) ? s_aabbs.at(i)->color()       :    Color{}; }
    Color            start_color(cI32 i) { return is_valid(i) ? s_aabbs.at(i)->start_color() :    Color{}; }
    RectF            rect(cI32 i)        { return is_valid(i) ? s_aabbs.at(i)->rect()        :    RectF{}; }
    F32              rect_x(cI32 i)      { return is_valid(i) ? s_aabbs.at(i)->rect().x      :       0.0F; }
    F32              rect_y(cI32 i)      { return is_valid(i) ? s_aabbs.at(i)->rect().y      :       0.0F; }
    F32              w(cI32 i)           { return is_valid(i) ? s_aabbs.at(i)->rect().w      :       0.0F; }
    F32              h(cI32 i)           { return is_valid(i) ? s_aabbs.at(i)->rect().h      :       0.0F; }
    bool             is_active(cI32 i)   { return is_valid(i) ? s_aabbs.at(i)->is_active()   :      false; }
    Name             name(cI32 i)        { return is_valid(i) ? s_aabbs.at(i)->name()        : Name::none; }
    entity::Object*  owner(cI32 i)       { return is_valid(i) ? s_aabbs.at(i)->owner()       :    nullptr; }
    //entity::Object* owner(cI32 i);

    Vec2F point(cI32 i, cU8 p) {
        if (!is_valid(i) or p > 3) return {};
        switch (p) {
            case 0:  return s_aabbs.at(i)->point_0();
            case 1:  return s_aabbs.at(i)->point_1();
            case 2:  return s_aabbs.at(i)->point_2();
            case 3:  return s_aabbs.at(i)->point_3();
            default: return {};
        }
    }
    //Vec2U time_left_colliding(cI32 i) { return IS_VALID(i) ? s_aabbs.at(i)->time_left_colliding : Vec2U{}; }
    I32 transform(cI32 i) { return is_valid(i) ? s_aabbs.at(i)->transform() : -1; }

    void color(cI32 i, Color c)           { if (is_valid(i)) s_aabbs.at(i)->color(c);       }
    void start_color(cI32 i, Color c)     { if (is_valid(i)) s_aabbs.at(i)->start_color(c); }
    void rect(cI32 i, cRectF r)           { if (is_valid(i)) s_aabbs.at(i)->rect(r);        }
    void is_active(cI32 i, bool q)        { if (is_valid(i)) s_aabbs.at(i)->is_active(q);   }
    void name(cI32 i, cName n)            { if (is_valid(i)) s_aabbs.at(i)->name(n);        }
    void owner(cI32 i, entity::Object* o) { if (is_valid(i)) s_aabbs.at(i)->owner(o);       }
    //void owner(cI32 i, entity::Object* o);

    //void time_left_colliding(cI32 i, cVec2U tv) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding   = tv; }
    //void time_left_colliding_x(cI32 i, cU16 tx) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding.x = tx; }
    //void time_left_colliding_y(cI32 i, cU16 ty) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding.y = ty; }

    I32 make(cI32 transform, cRectF rect) {
        AABB* aabb = new AABB(transform, rect);
        if (!s_unused_ids.empty()) {
            aabb->id(s_unused_ids.back());
            s_unused_ids.pop_back();
        } else {
            aabb->id(s_aabbs.size());
            s_aabbs.emplace_back(nullptr);
        }
        if (aabb->id() >= 0 and aabb->id() < s_aabbs.size() and s_aabbs.at(aabb->id())) {
            delete s_aabbs.at(aabb->id());
        }
        s_aabbs.at(aabb->id()) = aabb;
        return aabb->id();
    }
    bool erase(cI32 i) {
        if (!is_valid(i)) {
            //console::log("aabb::erase ", i, " can't do it\n");
            return false;
        }
        //console::log("aabb::erase ", i, "\n");
        delete s_aabbs.at(i);
        s_aabbs.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
    }
    //void update() {
    //    for (auto& i : s_aabbs) {
    //        if (i) i->update();
    //    }
    //}
    void draw(std::unique_ptr<Window>& window, cI32 i) {
        if (is_valid(i)) s_aabbs.at(i)->draw(window);
    }
    void clear() {
        for (size_t i = 0; i < s_aabbs.size(); ++i) {
            //console::log("aabb::clear erasing ", i, "\n");
            erase(i);            
        }
        s_aabbs.clear();
        s_unused_ids.clear();
    }

}
