module;
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

export module line;
import console;
import transform;
import types;
import window;

static U8 s_scale = 1;

export namespace line {
    cF32 length(cVec2F v) {
        return std::sqrtf(v.x * v.x + v.y * v.y);
    }
}

struct Line {
    bool is_aabb  = false,
         is_hidden = false;

    I32 id = -1,
        transform_id = -1;

    sf::Vertex sf_vertices[4] = {};
    U8  size = 1,
        layer = 0;
    Color color = { 127 },
        start_color = { 127 },
        prev_color = { 0 };
    Vec2F offset = { 0.0F, 0.0F },
        start = { 0.0F, 0.0F },
        end = { 0.0F, 0.0F },
        delta = { 0.0F, 0.0F },
        velocity = { 0.0F, 0.0F };

    F32 length = 0.0F,
        max_length = 0.0F,
        slope = 0.0F;


    /*Line() : id(-1), transform_id(-1),
        size(1), layer(0),
        color({ 127, 127, 127 }), start_color({ 127, 127, 127 }), prev_color({ 0, 0, 0 }),
        offset({ 0.0F, 0.0F }), start({ 0.0F, 0.0F }), end({ 0.0F, 0.0F }), delta({ 0.0F, 0.0F }), velocity({ 0.0F, 0.0F }),
        length(0.0F), max_length(0.0F), slope(0.0F),
        is_aabb(false), is_hidden(false) {
    }*/
    Line(cVec2F in_start, cVec2F in_end) {
        set(in_start, in_end);
    }
    Vec2F transformed_start() const {
        return start + transform::position(transform_id);
    }
    Vec2F transformed_end() const {
        return end + transform::position(transform_id);
    }
    void set(cVec2F in_start, cVec2F in_end) {
        cVec2F prev_start = start;
        cVec2F prev_end = end;
        start = in_start;
        end = in_end;

        //cVec2F size_offset = { (delta.x / length) * size, -(delta.y / length) * size };
        //cVec2F extra_length = { (delta.x / length) * size * 0.25f, (delta.y / length) * size * 0.25f };
        delta = end - start;

        if (prev_start != start && prev_end != end) {
            length = line::length(delta);
        }
        if (max_length > 0.0F && length > max_length) {
            cF32 over_max_length = length - max_length;
            end.x -= (over_max_length * delta.x / length);
            end.y -= (over_max_length * delta.y / length);
        }
        cVec2F perpendicular = { -delta.y / length * size / 2.0F, delta.x / length * size / 2.0F };

        sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + offset.x) * s_scale;
        sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + offset.y) * s_scale;

        sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + offset.x) * s_scale;
        sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + offset.y) * s_scale;

        sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + offset.x) * s_scale;
        sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + offset.y) * s_scale;

        sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + offset.x) * s_scale;
        sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + offset.y) * s_scale;
    }
    void update() {
        //Vec2F prev_delta = delta;
        delta = end - start;

        //if (delta != prev_delta) {
            length = line::length(delta);
            slope = delta.y / delta.x;
            //set(start, end);
            //console::log("perpendicular: ", perpendicular.x, " ", perpendicular.y, "\n");
        //}
        cVec2F perpendicular = { -delta.y / length * size / 2.0F, delta.x / length * size / 2.0F };

        sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + offset.x) * s_scale;
        sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + offset.y) * s_scale;

        sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + offset.x) * s_scale;
        sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + offset.y) * s_scale;

        sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + offset.x) * s_scale;
        sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + offset.y) * s_scale;

        sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + offset.x) * s_scale;
        sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + offset.y) * s_scale;


        /*if (color == Color{255, 0, 0}) {
            console::log("hello\n");
        }*/

        //if (color != prev_color) {
            //prev_color = color;
        for (U8 i = 0; i < 4; ++i) {
            sf_vertices[i].color = { color.r, color.g, color.b };
        }
        //}
    }
};

std::vector<Line*> s_lines;
std::vector<I32>   s_unused_ids;

export namespace line {
    constexpr bool is_valid(size_t i) { return (i < s_lines.size() && s_lines.at(i)) ? true : false; }

    void set(cI32 i, cVec2F start, cVec2F end) { if (is_valid(i)) s_lines.at(i)->set(start, end); }

    bool  is_aabb(cI32 i)     { return is_valid(i) ? s_lines.at(i)->is_aabb     :   false; }
    bool  is_hidden(cI32 i)    { return is_valid(i) ? s_lines.at(i)->is_hidden    :   false; }
    Color color(cI32 i)        { return is_valid(i) ? s_lines.at(i)->color        : Color{}; }
    Color start_color(cI32 i)  { return is_valid(i) ? s_lines.at(i)->start_color  : Color{}; }
    Color prev_color(cI32 i)   { return is_valid(i) ? s_lines.at(i)->prev_color   : Color{}; }
    I32   transform_id(cI32 i) { return is_valid(i) ? s_lines.at(i)->transform_id :       0; }
    U8    size(cI32 i)         { return is_valid(i) ? s_lines.at(i)->size         :       0; }
    cU8   layer(cI32 i)        { return is_valid(i) ? s_lines.at(i)->layer        :       0; }
    Vec2F offset(cI32 i)       { return is_valid(i) ? s_lines.at(i)->offset       : Vec2F{}; }
    Vec2F start(cI32 i)        { return is_valid(i) ? s_lines.at(i)->start        : Vec2F{}; }
    Vec2F end(cI32 i)          { return is_valid(i) ? s_lines.at(i)->end          : Vec2F{}; }
    Vec2F delta(cI32 i)        { return is_valid(i) ? s_lines.at(i)->delta        : Vec2F{}; }
            
    Vec2F velocity(cI32 i)     { return is_valid(i) ? s_lines.at(i)->velocity     : Vec2F{}; }
    F32   length(cI32 i)       { return is_valid(i) ? s_lines.at(i)->length       :       0; }
    F32   max_length(cI32 i)   { return is_valid(i) ? s_lines.at(i)->max_length   :       0; }

    void is_aabb(cI32 i, bool is)      { if (is_valid(i)) s_lines.at(i)->is_aabb     = is; }
    void is_hidden(cI32 i, bool is)     { if (is_valid(i)) s_lines.at(i)->is_hidden    = is; }
    void transform_id(cI32 i, cI32 t)   { if (is_valid(i)) s_lines.at(i)->transform_id = t;  }
    void color(cI32 i, Color c)         { if (is_valid(i)) s_lines.at(i)->color        = c;  }
    void start_color(cI32 i, Color c)   { if (is_valid(i)) s_lines.at(i)->start_color  = c;  }
    void prev_color(cI32 i, Color c)    { if (is_valid(i)) s_lines.at(i)->prev_color   = c;  }
    void size(cI32 i, cU8 s)            { if (is_valid(i)) s_lines.at(i)->size         = s;  }
    void layer(cI32 i, cU8 l)           { if (is_valid(i)) s_lines.at(i)->layer        = l;  }
    void offset(cI32 i, cVec2F o)       { if (is_valid(i)) s_lines.at(i)->offset       = o;  }
    void start(cI32 i, cVec2F s)        { if (is_valid(i)) s_lines.at(i)->start        = s;  }
    void start_x(cI32 i, cF32 x)        { if (is_valid(i)) s_lines.at(i)->start.x      = x;  }
    void start_y(cI32 i, cF32 y)        { if (is_valid(i)) s_lines.at(i)->start.y      = y;  }
    void end(cI32 i, cVec2F e)          { if (is_valid(i)) s_lines.at(i)->end          = e;  }
    void end_x(cI32 i, cF32 x)          { if (is_valid(i)) s_lines.at(i)->end.x        = x;  }
    void end_y(cI32 i, cF32 y)          { if (is_valid(i)) s_lines.at(i)->end.y        = y;  }
    void delta(cI32 i, cVec2F d)        { if (is_valid(i)) s_lines.at(i)->delta        = d;  }
    void velocity(cI32 i, cVec2F v)     { if (is_valid(i)) s_lines.at(i)->velocity     = v;  }
    void velocity_add_x(cI32 i, cF32 x) { if (is_valid(i)) s_lines.at(i)->velocity.x   += x; }
    void velocity_add_y(cI32 i, cF32 y) { if (is_valid(i)) s_lines.at(i)->velocity.y   += y; }    
    void max_length(cI32 i, cF32 m)     { if (is_valid(i)) s_lines.at(i)->max_length   = m;  }    
    void add_start(cI32 i, cVec2F s)    { if (is_valid(i)) s_lines.at(i)->start        += s; }
    void add_end(cI32 i, cVec2F e)      { if (is_valid(i)) s_lines.at(i)->end          += e; }
    void add_start_x(cI32 i, cF32 x)    { if (is_valid(i)) s_lines.at(i)->start.x      += x; }
    void add_start_y(cI32 i, cF32 y)    { if (is_valid(i)) s_lines.at(i)->start.y      += y; }
    void add_end_x(cI32 i, cF32 x)      { if (is_valid(i)) s_lines.at(i)->end.x        += x; }
    void add_end_y(cI32 i, cF32 y)      { if (is_valid(i)) s_lines.at(i)->end.y        += y; }

    Vec2F transformed_start(cI32 i) { return is_valid(i) ? s_lines.at(i)->transformed_start() : Vec2F{}; }
    Vec2F transformed_end(cI32 i)   { return is_valid(i) ? s_lines.at(i)->transformed_end()   : Vec2F{}; }

    Vec2F mid_point(cI32 i) { return is_valid(i) ? s_lines.at(i)->delta / 2.0F : Vec2F{}; }

    std::vector<I32> ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_object_ids;
        std::for_each(s_lines.cbegin(), s_lines.cend(), [&](Line* i) {
            if (i && i->id != -1 && i->layer == layer) {
                same_layer_object_ids.emplace_back(i->id);
            }
            }
        );
        return same_layer_object_ids;
    }
    size_t  size() { return s_lines.size(); }

    size_t make(cVec2F start, cVec2F end) {
        Line* object = new Line(start, end);
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            if (!s_lines.empty() && object->id >= 0 && object->id < s_lines.size() && s_lines.at(object->id)) {
                delete s_lines.at(object->id);
            }
        } else {
            object->id = s_lines.size();
            s_lines.emplace_back(nullptr);
        }
        s_lines.at(object->id) = object;
        return object->id;
    }
    bool erase(cI32 i) {
        if (!is_valid(i)) {
            //console::log("line::erase ", i, " can't do it\n");
            return false;
        }
        //console::log("line::erase ", i, "\n");
        delete s_lines.at(i);
        s_lines.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
        
    }
    void update() {
        for (auto& i : s_lines) {
            if (i) i->update();
        }
    }
    void update(cI32 i) { if (is_valid(i)) s_lines.at(i)->update(); }

    void draw(std::unique_ptr<Window>& window, cI32 i) {        
        if (!window || !is_valid(i) || s_lines.at(i)->is_hidden/* || s_lines.at(i)->length < 1.0F*/) return;

        s_scale = window->scale();

        cVec2F start = s_lines.at(i)->transformed_start() + s_lines.at(i)->offset;
        cVec2F end = s_lines.at(i)->transformed_end() + s_lines.at(i)->offset;

        if (start.x < 0.0F && end.x < 0.0F ||
            start.x > window->w() * 1.0F && end.x > window->w() * 1.0F ||
            start.y < 0.0F && end.y < 0.0F ||
            start.y > window->h() * 1.0F && end.y > window->h() * 1.0F) {
            return;
        }

        //console::log("drawing line ", i, " start: ", s_lines.at(i)->start.x, " ", s_lines.at(i)->start.y, " end: ", s_lines.at(i)->end.x, " ", s_lines.at(i)->end.y, "\n"); 
        //sf::RenderStates states;
        //states.shader = test_shader.get_sfml_shader();
        //m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan, states);

        window->draw_line(s_lines.at(i)->sf_vertices);
    }
    void draw_in_layer(std::unique_ptr<Window>& window, cU8 layer) {
        for (size_t i = 0; i < s_lines.size(); ++i) {
            if (is_valid(i) && s_lines.at(i)->layer == layer) {
                if (!s_lines.at(i)->is_aabb) {
                    draw(window, i);
                }
            }
        }
    }
    void clear() {
        for (size_t i = 0; i < s_lines.size(); ++i) {
            //console::log("line::clear erasing ", i, "\n");
            erase(i);
        }
        s_lines.clear();
        s_unused_ids.clear();
    }
}