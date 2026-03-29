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

struct AABB {
    bool is_active = true;

    I32 transform_id = -1;

    I32 up_line_id = -1,
        down_line_id = -1,
        left_line_id = -1,
        right_line_id = -1;

    RectF rect = { 0.0F, 0.0F, 0.0F, 0.0F };

    Color color = { 0, 0, 0 },
          start_color = { 0, 0, 0 };

    I32 id = -1;

    entity::Object* owner = nullptr;
    //U16 time_left_colliding = 0;

    aabb::Name name = aabb::Name::none;

    AABB() = delete;
    AABB(cI32 in_transform_id, cRectF in_rect) {
        transform_id = in_transform_id;
        rect = in_rect;

        up_line_id = line::make({ rect.x, rect.y }, { rect.x + rect.w, rect.y });
        down_line_id = line::make({ rect.x, rect.y + rect.h }, { rect.x + rect.w, rect.y + rect.h });
        left_line_id = line::make({ rect.x, rect.y }, { rect.x,          rect.y + rect.h });
        right_line_id = line::make({ rect.x + rect.w, rect.y }, { rect.x + rect.w, rect.y + rect.h });

        line::layer(up_line_id, NUM_VISIBLE_LAYERS - 1);
        line::layer(down_line_id, NUM_VISIBLE_LAYERS - 1);
        line::layer(left_line_id, NUM_VISIBLE_LAYERS - 1);
        line::layer(right_line_id, NUM_VISIBLE_LAYERS - 1);
        line::transform_id(up_line_id, transform_id);
        line::transform_id(down_line_id, transform_id);
        line::transform_id(left_line_id, transform_id);
        line::transform_id(right_line_id, transform_id);
        line::is_aabb(up_line_id, true);
        line::is_aabb(down_line_id, true);
        line::is_aabb(left_line_id, true);
        line::is_aabb(right_line_id, true);        
    }
    ~AABB() {
        line::erase(up_line_id);
        line::erase(down_line_id);
        line::erase(left_line_id);
        line::erase(right_line_id);
        transform_id = -1;
    }
        
    void set_is_active(bool q) {
        is_active = q;
        line::is_hidden(up_line_id, !q);
        line::is_hidden(down_line_id, !q);
        line::is_hidden(left_line_id, !q);
        line::is_hidden(right_line_id, !q);
    }

    void set_rect(cRectF r) {
        rect = r;
        line::set(up_line_id, { rect.x, rect.y }, { rect.x + rect.w, rect.y });
        line::set(down_line_id, { rect.x, rect.y + rect.h }, { rect.x + rect.w, rect.y + rect.h });
        line::set(left_line_id, { rect.x, rect.y }, { rect.x,          rect.y + rect.h });
        line::set(right_line_id, { rect.x + rect.w, rect.y }, { rect.x + rect.w, rect.y + rect.h });        
    }

    cVec2F point_0() { return transform::position(transform_id) + Vec2F{ rect.x,  rect.y }; }
    cVec2F point_1() { return transform::position(transform_id) + Vec2F{ rect.x + rect.w, rect.y }; }
    cVec2F point_2() { return transform::position(transform_id) + Vec2F{ rect.x,  rect.y + rect.h }; }
    cVec2F point_3() { return transform::position(transform_id) + Vec2F{ rect.x + rect.w, rect.y + rect.h }; }

    const std::vector<Vec2F> points() {
        return { point_0(), point_1(), point_2(), point_3() };
    }
        
    void set_color(Color c) {
        color = c;
        line::color(up_line_id, c);
        line::color(down_line_id, c);
        line::color(left_line_id, c);
        line::color(right_line_id, c);
    }
    void set_start_color(Color c) {
        start_color = c;
        line::start_color(up_line_id, c);
        line::start_color(down_line_id, c);
        line::start_color(left_line_id, c);
        line::start_color(right_line_id, c);
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
        /*line::color(m_up_line_id, color);
        line::color(m_down_line_id, color);
        line::color(m_left_line_id, color);
        line::color(m_right_line_id, color);*/

        //console::log("color: ", (int)color.r, "\n");

        /*for (auto& i : quad_tree_node_ids) {
            console::log("id: ", i, "\n");
        }*/
    //}

    void draw(std::unique_ptr<Window>& window) {
        if (!is_active) return;

        line::draw(window, up_line_id);
        line::draw(window, down_line_id);
        line::draw(window, left_line_id);
        line::draw(window, right_line_id);
    }
};

std::vector<AABB*> s_aabbs;
std::vector<I32>   s_unused_ids;

export namespace aabb {
    constexpr bool is_valid(size_t i) { return (i >= 0 and i < s_aabbs.size() and s_aabbs.at(i)) ? true : false; }

    size_t  size() { return s_aabbs.size(); }

    Color            color(cI32 i)       { return is_valid(i) ? s_aabbs.at(i)->color       :    Color{}; }
    Color            start_color(cI32 i) { return is_valid(i) ? s_aabbs.at(i)->start_color :    Color{}; }
    RectF            rect(cI32 i)        { return is_valid(i) ? s_aabbs.at(i)->rect        :    RectF{}; }
    F32              rect_x(cI32 i)      { return is_valid(i) ? s_aabbs.at(i)->rect.x      :       0.0F; }
    F32              rect_y(cI32 i)      { return is_valid(i) ? s_aabbs.at(i)->rect.y      :       0.0F; }
    F32              w(cI32 i)           { return is_valid(i) ? s_aabbs.at(i)->rect.w      :       0.0F; }
    F32              h(cI32 i)           { return is_valid(i) ? s_aabbs.at(i)->rect.h      :       0.0F; }
    bool             is_active(cI32 i)   { return is_valid(i) ? s_aabbs.at(i)->is_active   :      false; }
    Name             name(cI32 i)        { return is_valid(i) ? s_aabbs.at(i)->name        : Name::none; }
    entity::Object*  owner(cI32 i)       { return is_valid(i) ? s_aabbs.at(i)->owner       :    nullptr; }
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
    I32 transform_id(cI32 i) { return is_valid(i) ? s_aabbs.at(i)->transform_id : -1; }

    void color(cI32 i, Color c)           { if (is_valid(i)) s_aabbs.at(i)->set_color(c);       }
    void start_color(cI32 i, Color c)     { if (is_valid(i)) s_aabbs.at(i)->set_start_color(c); }
    void rect(cI32 i, cRectF r)           { if (is_valid(i)) s_aabbs.at(i)->set_rect(r);        }
    void is_active(cI32 i, bool q)        { if (is_valid(i)) s_aabbs.at(i)->set_is_active(q);   }
    void name(cI32 i, cName n)            { if (is_valid(i)) s_aabbs.at(i)->name = n;           }
    void owner(cI32 i, entity::Object* o) { if (is_valid(i)) s_aabbs.at(i)->owner = o;          }
    //void owner(cI32 i, entity::Object* o);

    //void time_left_colliding(cI32 i, cVec2U tv) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding   = tv; }
    //void time_left_colliding_x(cI32 i, cU16 tx) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding.x = tx; }
    //void time_left_colliding_y(cI32 i, cU16 ty) { if (IS_VALID(i)) s_aabbs.at(i)->time_left_colliding.y = ty; }

    I32 make(cI32 transform_id, cRectF rect) {
        AABB* aabb = new AABB(transform_id, rect);
        if (!s_unused_ids.empty()) {
            aabb->id = s_unused_ids.back();
            s_unused_ids.pop_back();
        } else {
            aabb->id = s_aabbs.size();
            s_aabbs.emplace_back(nullptr);
        }
        if (aabb->id >= 0 and aabb->id < s_aabbs.size() and s_aabbs.at(aabb->id)) {
            delete s_aabbs.at(aabb->id);
        }
        s_aabbs.at(aabb->id) = aabb;
        return aabb->id;
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
