module;
#include <SFML/Graphics.hpp>

export module circle;
import console;
import transform;
import transform.trait;
import types;
import window;

static U8 s_scale = 1;

export namespace circle {
    class Object : public transform::Trait {
        sf::CircleShape m_circle_shape;
        
    public:
        Object() {
            m_transform = transform::make();
            console::log("circle::Object() transform: ", m_transform, "\n");
            transform::position(m_transform, { 32.0F, 32.0F });
            console::log("circle::Object() position: ", transform::position(m_transform).x, " ", transform::position(m_transform).y, "\n");


            m_circle_shape.setFillColor(sf::Color::Green);
        }        
        ~Object() {
            console::log("~circle::Object() transform: ", m_transform, "\n");
            transform::erase(m_transform);
        }


        F32 radius() const { return m_circle_shape.getRadius(); } void radius(cF32 r) { m_circle_shape.setRadius(r); }


        void update() {
            //console::log("circle::Object::update() transform: ", m_transform, "\n");

            sf::Vector2f pos = { transform::position(m_transform).x * s_scale,
                                 transform::position(m_transform).y * s_scale };

            m_circle_shape.setPosition(pos);

            //m_circle_shape.setPosition(sf::Vector2f{32.0F, 32.0F});
        }
        void draw(std::unique_ptr<Window>& window) {
            s_scale = window->scale();

            window->draw_circle(m_circle_shape);
        }

    };
    

}