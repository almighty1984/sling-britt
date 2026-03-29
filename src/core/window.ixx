module;
#include <SFML/Graphics.hpp>

export module window;
import console;
//import line;
//import sprite;
import transform;
import types;
import shader;

export class Window {
    RectU m_view = { 0, 0, 320, 180 };
    U8  m_scale = 1;
    U16 m_w = 0,
        m_h = 0;

    bool m_is_in_focus = true;

    Color m_screen_color = { 255, 255, 255 };

public:
    sf::RenderWindow m_sf_window;
    Shader test_shader;

    Window() = delete;
    Window(cU16 w, cU16 h, cU8 scale, const char* title) {
        console::log("Window::init\n");
        m_w = w, m_h = h;
        m_scale = scale;
        m_view = { 0, 0, w, h };
        U32 video_w = w * scale;
        U32 video_h = h * scale;
        m_sf_window = sf::RenderWindow(sf::VideoMode({ video_w, video_h }), title, sf::Style::Default, sf::State::Windowed);
        sf::View sf_view(sf::FloatRect{ {0.0F, 0.0F}, {(F32)(video_w), (F32)(video_h) } });
        m_sf_window.setView(sf_view);        
        test_shader.load("res/shader/test.vert", "res/shader/test.frag");
    }
    
    bool is_open() const { return m_sf_window.isOpen(); }
    bool is_in_focus() const { return m_is_in_focus; }  void is_in_focus(bool q) { m_is_in_focus = q; }

    Color screen_color() const { return m_screen_color; }   void screen_color(Color c) { m_screen_color = c; }

    RectU view()  const { return m_view;  }
    U8    scale() const { return m_scale; }
    U16   w()     const { return m_w;     }    void w(cU16 w) { m_w = w; }
    U16   h()     const { return m_h;     }    void h(cU16 h) { m_h = h; }

    void view(cRectU view) {
        console::log("Window::view: ", view.x, " ", view.y, " ", view.w, " ", view.h, "\n");
        m_view = view;

        cF32 scaled_view_x = (F32)view.x * m_scale;
        cF32 scaled_view_y = (F32)view.y * m_scale;
        cF32 scaled_view_w = (F32)view.w * m_scale;
        cF32 scaled_view_h = (F32)view.h * m_scale;

        sf::View sf_view(sf::FloatRect{{ scaled_view_x, scaled_view_y },
                                       { scaled_view_w, scaled_view_h }});
        m_sf_window.setView(sf_view);
    }
    void clear()   { m_sf_window.clear();   }
    void close()   { m_sf_window.close();   }
    void display() { 
        test_shader.color(m_screen_color);

        m_sf_window.display();
    }
    
    void draw_circle(sf::CircleShape& circle) {
        m_sf_window.draw(circle);
    }
    void draw_line(sf::Vertex* vertices) {
        m_sf_window.draw(vertices, 4, sf::PrimitiveType::TriangleFan);
    }
    void draw_sf_sprite(sf::Sprite& sf_sprite) {
        sf::RenderStates states;

        test_shader.set_uniform("light_pos", Vec2F{ 150, 80 });
        

        states.shader = test_shader.get_sfml_shader();

        
        m_sf_window.draw(sf_sprite, states);
    }    
    std::optional<sf::Event> poll_event() { return m_sf_window.pollEvent(); }
};