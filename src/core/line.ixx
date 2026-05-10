module;
#include <SFML/Graphics.hpp>

export module line;
import console;
import transform;
import types;
import window;
import std;

static U8 s_scale = 1;

Color s_screen_color = { };

export namespace line {
    cF32 length(cVec2F v) {
        return std::sqrtf(v.x * v.x + v.y * v.y);
    }
    cF32 squared_length(cVec2F v) {
        return v.x * v.x + v.y * v.y;
    }
}

class Line {
    bool m_is_aabb   = false,
         m_is_hidden = false;

    I32 m_id        = -1,
        m_transform = -1;

    sf::Vertex m_sf_vertices[4] = {};
    U8  m_size  = 1,
        m_layer = 0;
    Color m_color       = { 0 },
          m_start_color = { 0 },
          m_prev_color  = { 0 };
    Vec2F m_offset   = { 0.0F, 0.0F },
          m_start    = { 0.0F, 0.0F },
          m_end      = { 0.0F, 0.0F },
          m_delta    = { 0.0F, 0.0F },
          m_velocity = { 0.0F, 0.0F };

    F32 m_length     = 0.0F,
        m_max_length = 0.0F,
        m_slope      = 0.0F;
    
public:
    bool is_aabb()   const { return m_is_aabb;   }  void is_aabb(bool q)   { m_is_aabb   = q; }
    bool is_hidden() const { return m_is_hidden; }  void is_hidden(bool q) { m_is_hidden = q; }

    I32 id()        const { return m_id;        } void id(cI32 i)        { m_id        = i; }
    I32 transform() const { return m_transform; } void transform(cI32 t) { m_transform = t; }        

    sf::Vertex* sf_vertices() { return m_sf_vertices; }

    U8 size()  const { return m_size;  }  void size(cU8 s)  { m_size = s; }
    U8 layer() const { return m_layer; }  void layer(cU8 l) { m_layer = l; }        

    Color color() const { return m_color; } void color(Color c) { m_color = c; }
    Color start_color() const { return m_start_color; } void start_color(Color c) { m_start_color = c; }
    Color prev_color() const { return m_prev_color; } void prev_color(Color c) { m_prev_color = c; }

    Vec2F offset()    const { return m_offset;   } void offset(cVec2F o)   { m_offset   = o; }

    Vec2F start()     const { return m_start;    } void start(cVec2F s)     { m_start    = s; }
                                                   void start_x(cF32 x)     { m_start.x  = x; }
                                                   void start_y(cF32 y)     { m_start.y  = y; }
                                                   void start_add(cVec2F s) { m_start   += s; }
                                                   void start_add_x(cF32 x) { m_start.x += x; }
                                                   void start_add_y(cF32 y) { m_start.y += y; }
    Vec2F end()       const { return m_end;      } void end(cVec2F e)       { m_end      = e; }
                                                   void end_x(cF32 x)       { m_end.x    = x; }
                                                   void end_y(cF32 y)       { m_end.y    = y; }
                                                   void end_add(cVec2F e)   { m_end     += e; }
                                                   void end_add_x(cF32 x)   { m_end.x   += x; }
                                                   void end_add_y(cF32 y)   { m_end.y   += y; }

    Vec2F delta()     const { return m_delta;    } void delta(cVec2F d)        { m_delta       = d; }
    Vec2F velocity()  const { return m_velocity; } void velocity(cVec2F v)     { m_velocity    = v; }
                                                   void velocity_x(cF32 x)     { m_velocity.x  = x; }
                                                   void velocity_y(cF32 y)     { m_velocity.y  = y; }
                                                   void velocity_add(cVec2F e) { m_velocity   += e; }
                                                   void velocity_add_x(cF32 x) { m_velocity.x += x; }
                                                   void velocity_add_y(cF32 y) { m_velocity.y += y; }

    F32 length()     const { return m_length;     }
    F32 max_length() const { return m_max_length; } void max_length(cF32 l) { m_max_length = l; }
    F32 slope()      const { return m_slope;      }


    /*Line() : id(-1), transform(-1),
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
        return m_start + transform::position(m_transform);
    }
    Vec2F transformed_end() const {
        return m_end + transform::position(m_transform);
    }
    void set(cVec2F in_start, cVec2F in_end) {
        cVec2F prev_start = m_start;
        cVec2F prev_end = m_end;
        m_start = in_start;
        m_end = in_end;

        //cVec2F size_offset = { (delta.x / length) * size, -(delta.y / length) * size };
        //cVec2F extra_length = { (delta.x / length) * size * 0.25f, (delta.y / length) * size * 0.25f };
        m_delta = m_end - m_start;

        //if (prev_start != start and prev_end != end) {
            m_length = line::length(m_delta);
        //}
        if (m_max_length > 0.0F and m_length > m_max_length) {
            cF32 over_max_length = m_length - m_max_length;
            m_end.x -= (over_max_length * m_delta.x / m_length);
            m_end.y -= (over_max_length * m_delta.y / m_length);
        }
        cVec2F perpendicular = { -m_delta.y / m_length * m_size / 2.0F, m_delta.x / m_length * m_size / 2.0F };

        m_sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + m_offset.y) * s_scale;
    }
    void update() {
        //Vec2F prev_delta = delta;
        m_delta = m_end - m_start;

        //if (delta != prev_delta) {
            m_length = line::length(m_delta);
            
            m_slope = m_delta.y / m_delta.x;
            //set(start, end);
            //console::log("perpendicular: ", perpendicular.x, " ", perpendicular.y, "\n");
        //}
        cVec2F perpendicular = { -m_delta.y / m_length * m_size / 2.0F, m_delta.x / m_length * m_size / 2.0F };

        m_sf_vertices[0].position.x = (transformed_start().x + perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[0].position.y = (transformed_start().y + perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[1].position.x = (transformed_end().x + perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[1].position.y = (transformed_end().y + perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[2].position.x = (transformed_end().x - perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[2].position.y = (transformed_end().y - perpendicular.y + m_offset.y) * s_scale;

        m_sf_vertices[3].position.x = (transformed_start().x - perpendicular.x + m_offset.x) * s_scale;
        m_sf_vertices[3].position.y = (transformed_start().y - perpendicular.y + m_offset.y) * s_scale;


        /*if (color == Color{255, 0, 0}) {
            console::log("hello\n");
        }*/

        if (m_start_color.r == 0 and m_start_color.g == 0 and m_start_color.b == 0 
            and (m_color.r != 0 or m_color.g != 0 or m_color.b != 0)) {
            m_start_color = m_color;
        }

        //if (color != prev_color) {
            //prev_color = color;
        for (U8 i = 0; i < 4; ++i) {

            Vec3F amount = { s_screen_color.r / 255.0F,
                             s_screen_color.g / 255.0F,
                             s_screen_color.b / 255.0F };

            m_sf_vertices[i].color = { (U8)(m_color.r * amount.x),
                                       (U8)(m_color.g * amount.y),
                                       (U8)(m_color.b * amount.z) };

            //sf_vertices[i].color = { color.r, color.g, color.b };
        }
        //}
    }
};

std::vector<Line*> s_lines;
std::vector<I32>   s_unused_ids;


export namespace line {
    constexpr bool is_valid(size_t i) { return (i < s_lines.size() and s_lines.at(i)) ? true : false; }

    Color screen_color() { return s_screen_color; } void screen_color(Color c) { s_screen_color = c; }

    void set(cI32 i, cVec2F start, cVec2F end) { if (is_valid(i)) s_lines.at(i)->set(start, end); }

    bool  is_aabb(cI32 i)      { return is_valid(i) ? s_lines.at(i)->is_aabb()     :  false;  }
    bool  is_hidden(cI32 i)    { return is_valid(i) ? s_lines.at(i)->is_hidden()   :  false;  }
    Color color(cI32 i)        { return is_valid(i) ? s_lines.at(i)->color()       : Color{}; }
    Color start_color(cI32 i)  { return is_valid(i) ? s_lines.at(i)->start_color() : Color{}; }
    Color prev_color(cI32 i)   { return is_valid(i) ? s_lines.at(i)->prev_color()  : Color{}; }
    I32   transform(cI32 i)    { return is_valid(i) ? s_lines.at(i)->transform()   :      -1; }
    U8    size(cI32 i)         { return is_valid(i) ? s_lines.at(i)->size()        :       0; }
    cU8   layer(cI32 i)        { return is_valid(i) ? s_lines.at(i)->layer()       :       0; }
    Vec2F offset(cI32 i)       { return is_valid(i) ? s_lines.at(i)->offset()      : Vec2F{}; }
    Vec2F start(cI32 i)        { return is_valid(i) ? s_lines.at(i)->start()       : Vec2F{}; }
    Vec2F end(cI32 i)          { return is_valid(i) ? s_lines.at(i)->end()         : Vec2F{}; }
    Vec2F delta(cI32 i)        { return is_valid(i) ? s_lines.at(i)->delta()       : Vec2F{}; }

    Vec2F velocity(cI32 i)     { return is_valid(i) ? s_lines.at(i)->velocity()    : Vec2F{}; }
    F32   length(cI32 i)       { return is_valid(i) ? s_lines.at(i)->length()      :       0; }
    F32   max_length(cI32 i)   { return is_valid(i) ? s_lines.at(i)->max_length()  :       0; }

    void is_aabb(cI32 i, bool q)        { if (is_valid(i)) s_lines.at(i)->is_aabb(q);        }
    void is_hidden(cI32 i, bool q)      { if (is_valid(i)) s_lines.at(i)->is_hidden(q);      }
    void transform(cI32 i, cI32 t)      { if (is_valid(i)) s_lines.at(i)->transform(t);      }
    void color(cI32 i, Color c)         { if (is_valid(i)) s_lines.at(i)->color(c);          }
    void start_color(cI32 i, Color c)   { if (is_valid(i)) s_lines.at(i)->start_color(c);    }
    void prev_color(cI32 i, Color c)    { if (is_valid(i)) s_lines.at(i)->prev_color(c);     }
    void size(cI32 i, cU8 s)            { if (is_valid(i)) s_lines.at(i)->size(s);           }
    void layer(cI32 i, cU8 l)           { if (is_valid(i)) s_lines.at(i)->layer(l);          }
    void offset(cI32 i, cVec2F o)       { if (is_valid(i)) s_lines.at(i)->offset(o);         }
    void start(cI32 i, cVec2F s)        { if (is_valid(i)) s_lines.at(i)->start(s);          }
    void start_x(cI32 i, cF32 x)        { if (is_valid(i)) s_lines.at(i)->start_x(x);        }
    void start_y(cI32 i, cF32 y)        { if (is_valid(i)) s_lines.at(i)->start_y(y);        }
    void end(cI32 i, cVec2F e)          { if (is_valid(i)) s_lines.at(i)->end(e);            }
    void end_x(cI32 i, cF32 x)          { if (is_valid(i)) s_lines.at(i)->end_x(x);          }
    void end_y(cI32 i, cF32 y)          { if (is_valid(i)) s_lines.at(i)->end_y(y);          }
    void delta(cI32 i, cVec2F d)        { if (is_valid(i)) s_lines.at(i)->delta(d);          }
    void velocity(cI32 i, cVec2F v)     { if (is_valid(i)) s_lines.at(i)->velocity(v);       }
    void velocity_add_x(cI32 i, cF32 x) { if (is_valid(i)) s_lines.at(i)->velocity_add_x(x); }
    void velocity_add_y(cI32 i, cF32 y) { if (is_valid(i)) s_lines.at(i)->velocity_add_y(y); }    
    void max_length(cI32 i, cF32 m)     { if (is_valid(i)) s_lines.at(i)->max_length(m);     }
    void start_add(cI32 i, cVec2F s)    { if (is_valid(i)) s_lines.at(i)->start_add(s);      }
    void end_add(cI32 i, cVec2F e)      { if (is_valid(i)) s_lines.at(i)->end_add(e);        }
    void start_add_x(cI32 i, cF32 x)    { if (is_valid(i)) s_lines.at(i)->start_add_x(x);    }
    void start_add_y(cI32 i, cF32 y)    { if (is_valid(i)) s_lines.at(i)->start_add_y(y);    }
    void end_add_x(cI32 i, cF32 x)      { if (is_valid(i)) s_lines.at(i)->end_add_x(x);      }
    void end_add_y(cI32 i, cF32 y)      { if (is_valid(i)) s_lines.at(i)->end_add_y(y);      }

    Vec2F transformed_start(cI32 i) { return is_valid(i) ? s_lines.at(i)->transformed_start() : Vec2F{}; }
    Vec2F transformed_end(cI32 i)   { return is_valid(i) ? s_lines.at(i)->transformed_end()   : Vec2F{}; }

    Vec2F mid_point(cI32 i) { return is_valid(i) ? s_lines.at(i)->delta() / 2.0F : Vec2F{}; }

    std::vector<I32> ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_object_ids;
        std::for_each(s_lines.cbegin(), s_lines.cend(), [&](Line* i) {
            if (i and i->id() != -1 and i->layer() == layer) {
                same_layer_object_ids.emplace_back(i->id());
            }
            }
        );
        return same_layer_object_ids;
    }
    size_t  size() { return s_lines.size(); }

    size_t make(cVec2F start, cVec2F end) {
        Line* object = new Line(start, end);
        if (!s_unused_ids.empty()) {
            object->id(s_unused_ids.back());
            s_unused_ids.pop_back();
            if (!s_lines.empty() and object->id() >= 0 and object->id() < s_lines.size() and s_lines.at(object->id())) {
                delete s_lines.at(object->id());
            }
        } else {
            object->id(s_lines.size());
            s_lines.emplace_back(nullptr);
        }
        s_lines.at(object->id()) = object;
        return object->id();
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
        if (!window or !is_valid(i) or s_lines.at(i)->is_hidden()/* or s_lines.at(i)->length < 1.0F*/) return;

        s_scale = window->scale();

        cVec2F start = s_lines.at(i)->transformed_start() + s_lines.at(i)->offset();
        cVec2F end = s_lines.at(i)->transformed_end() + s_lines.at(i)->offset();

        if (start.x < 0.0F and end.x < 0.0F or
            start.x > window->w() * 1.0F and end.x > window->w() * 1.0F or
            start.y < 0.0F and end.y < 0.0F or
            start.y > window->h() * 1.0F and end.y > window->h() * 1.0F) {
            return;
        }

        //console::log("drawing line ", i, " start: ", s_lines.at(i)->start.x, " ", s_lines.at(i)->start.y, " end: ", s_lines.at(i)->end.x, " ", s_lines.at(i)->end.y, "\n"); 
        //sf::RenderStates states;
        //states.shader = test_shader.get_sfml_shader();
        //m_sf_window.draw(line->sf_vertices, 4, sf::PrimitiveType::TriangleFan, states);

        window->draw_line(s_lines.at(i)->sf_vertices());
    }
    void draw_in_layer(std::unique_ptr<Window>& window, cU8 layer) {
        for (size_t i = 0; i < s_lines.size(); ++i) {
            if (is_valid(i) and s_lines.at(i)->layer() == layer) {
                if (!s_lines.at(i)->is_aabb()) {
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