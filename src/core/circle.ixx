module;
#include <SFML/Graphics.hpp>

export module circle;
import config;
import console;
import transform;
import transform.trait;
import types;
import window;


export namespace circle {
    class Object : public transform::Trait {
        sf::CircleShape m_circle_shape;
        
    public:
        Object() {
            m_transform_id = transform::make();
            console::log("circle::Object() transform_id: ", m_transform_id, "\n");
            transform::position(m_transform_id, { 32.0F, 32.0F });
            console::log("circle::Object() position: ", transform::position(m_transform_id).x, " ", transform::position(m_transform_id).y, "\n");


            m_circle_shape.setFillColor(sf::Color::Green);
        }        
        ~Object() {
            console::log("~circle::Object() transform_id: ", m_transform_id, "\n");
            transform::erase(m_transform_id);
        }


        F32 radius() const { return m_circle_shape.getRadius(); } void radius(cF32 r) { m_circle_shape.setRadius(r); }


        void update() {
            //console::log("circle::Object::update() transform_id: ", m_transform_id, "\n");

            sf::Vector2f pos = { transform::position(m_transform_id).x * Config::scale(),
                                 transform::position(m_transform_id).y * Config::scale() };

            m_circle_shape.setPosition(pos);

            //m_circle_shape.setPosition(sf::Vector2f{32.0F, 32.0F});
        }
        void draw(std::unique_ptr<Window>& window) {            
            window->draw_circle(m_circle_shape);
        }

    };
    

}