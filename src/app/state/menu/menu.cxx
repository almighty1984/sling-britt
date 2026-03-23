module;
#include <memory>
#include <sstream>
#include <cmath>

module state.menu;
import config;
import console;
import input;
import line;

import transform;

namespace state {
    Menu::Menu(cU16 window_w, cU16 window_h) {
        console::log("state::Menu::Menu() ", window_w, " ", window_h, "\n");
        m_state = m_next_state = Type::menu;
        m_input_id = input::make();

        m_transform_id = transform::make();
        transform::position(m_transform_id, { window_w / 2.0F, window_h / 2.0F });
        transform::deceleration(m_transform_id, { 0.05F, 0.05F });

        m_line = line::make({ 0.0F, 0.0F }, { 32.0F, -8.0F });
        line::size(m_line, 2.0F);
        line::color(m_line, { 52, 206, 206 });
        line::layer(m_line, 0);
        line::transform_id(m_line, m_transform_id);
                
        Vec2F normal = Vec2F{ -line::delta(m_line).y, line::delta(m_line).x };
        
        F32 normal_length = line::length(normal);

        Vec2F normalized_normal = { normal.x / normal_length, normal.y / normal_length };

        m_normal = line::make({0.0F, 0.0F}, normalized_normal * 10.0F);
        line::size(m_normal, 2.0F);
        line::transform_id(m_normal, m_transform_id);
        
        console::log("state::Menu::Menu() normal delta: ", line::delta(m_normal).x, " ", line::delta(m_normal).y, "\n");

        console::log("state::Menu::Menu() normal length: ", normal_length, "\n");
                
        m_circle.radius(4.0F);
        m_circle.position({ window_w / 2.0F - 16.0F, window_h / 2.0F - 64.0F });

        m_circle.max_velocity({ 4.0F, 4.0F });

        m_proj_on_normal = line::make({ 0.0F, 0.0F }, { 0.0F, 0.0F });
        line::size(m_proj_on_normal, 1.0F);
        line::color(m_proj_on_normal, { 255, 0, 0 });
        line::transform_id(m_proj_on_normal, m_transform_id);

        m_enter_text.set_text("F1: Edit\nF2: Game");

        m_circle.update();
    }
    Menu::~Menu() {
        console::log("state::Menu::~Menu()\n");
        input::erase(m_input_id);
        transform::erase(m_transform_id);
        line::erase(m_line);
        line::erase(m_normal);
        line::erase(m_proj_on_normal);
    }

    void Menu::update(cF32 ts) {
        
        //console::log("mouse: ", input::mouse.x, " ", input::mouse.y, "\n");

        //console::log("state::Game::update() delta: ", line::delta(m_normal).x, " ", line::delta(m_normal).y, "\n");





        if (is_pressed(input::Key::f1)) {
            release(input::Key::f1);
            m_next_state = Type::edit;
            return;
        }
        if (is_pressed(input::Key::f2) || is_pressed(input::Key::enter)) {
            release(input::Key::f2);
            release(input::Key::enter);
            console::log("state::Menu::update()\n");

            m_next_state = Type::game;
            m_start_info = { .type   = start::Type::center,
                             .number = 0 };
            return;
        }

        //line::set(m_line, { 0.0F, 0.0F }, { 16.0F, 16.0F });

        if (is_pressed(input::Key::w)) {
            transform::add_velocity_y(m_transform_id, -0.1F);
        }
        if (is_pressed(input::Key::s)) {
            transform::add_velocity_y(m_transform_id, 0.1F);
        }
        if (is_pressed(input::Key::a)) {
            transform::add_velocity_x(m_transform_id, -0.1F);
        }
        if (is_pressed(input::Key::d)) {
            transform::add_velocity_x(m_transform_id, 0.1F);
        }


        //m_circle.add_velocity({ 0.0F, 0.05F });

        Vec2F normal = line::delta(m_normal);
        Vec2F tangent = { normal.y, -normal.x };
        
        Vec2F point = m_circle.position() - transform::position(m_transform_id);
        cF32 point_dot_normal = point.x * normal.x + point.y * normal.y;
        cF32 point_dot_tangent = tangent.x * point.x + tangent.y * point.y;
        cF32 normal_dot_normal = normal.x * normal.x + normal.y * normal.y;
        Vec2F proj_onto_normal = normal * (point_dot_normal / normal_dot_normal);

        line::set(m_proj_on_normal, { 0.0F, 0.0F }, proj_onto_normal);


        
        F32 move_speed = 0.1F;
        //F32 move_speed = 0.05F;
        m_circle.deceleration({ 0.05F, 0.05F });
        if (point_dot_normal <= 0.0F) {
            move_speed = 1.0F;


            //m_circle.deceleration({ 0.01F, 0.01F });
            //console::log("state::Menu::update() point dot normal: ", point_dot_normal, "\n");
            m_circle.add_position_x(-proj_onto_normal.x);
            m_circle.add_position_y(-proj_onto_normal.y);

            //m_circle.velocity_x(-proj_onto_normal.x * 0.2F + transform::velocity(m_transform_id).x * 1.0F);
            //m_circle.velocity_y(-proj_onto_normal.y * 0.2F + transform::velocity(m_transform_id).y * 1.0F);

            //m_circle.add_velocity_x(transform::velocity(m_transform_id).x * 0.1F);
            //m_circle.add_velocity_y(transform::velocity(m_transform_id).y * 0.1F);

            F32 speed_along_normal = m_circle.velocity().x * normal.x + m_circle.velocity().y * normal.y;
            F32 speed_along_tangent = m_circle.velocity().x * tangent.x + m_circle.velocity().y * tangent.y;

            if (speed_along_normal <= 0.0F) {
                F32 speed_x = -speed_along_normal * normal.x * 0.1F + speed_along_tangent * tangent.x * 0.1F;
                F32 speed_y = -speed_along_normal * normal.y * 0.1F + speed_along_tangent * tangent.y * 0.1F;

                //    console::log("state::Menu::update() speed_x: ", speed_x, " speed_y: ", speed_y, "\n");

                    m_circle.velocity_x(/*-proj_onto_normal.x * 0.01F +*/ speed_x * 0.01F + transform::velocity(m_transform_id).x);
                    m_circle.velocity_y(/*-proj_onto_normal.y * 0.01F +*/ speed_y * 0.01F + transform::velocity(m_transform_id).y);
                //                            
                    //m_circle.add_velocity_x(speed_x);
                    //m_circle.add_velocity_y(speed_y);

                //    m_circle.add_velocity_x(transform::velocity(m_transform_id).x);
                //    m_circle.add_velocity_y(transform::velocity(m_transform_id).y);
                //}


                /*if (is_pressed(input::Key::up)) {
                    release(input::Key::up);
                    transform::add_velocity(m_circle.transform_id(), normal * 0.2F);
                }*/
            }


        }

        if (is_pressed(input::Key::up)) {            
            transform::add_velocity_y(m_circle.transform_id(), -move_speed);
        }
        if (is_pressed(input::Key::down)) {
            transform::add_velocity_y(m_circle.transform_id(), move_speed);
        }
        if (is_pressed(input::Key::left)) {
            transform::add_velocity_x(m_circle.transform_id(), -move_speed);
        }
        if (is_pressed(input::Key::right)) {
            transform::add_velocity_x(m_circle.transform_id(), move_speed);
        }

        
        //if (point_dot_normal < m_circle.radius()) {
            //console::log("distance from line: ", point_dot_normal, "\n");

            //F32 speed_along_normal = m_circle.velocity().x * normal.x + m_circle.velocity().y * normal.y;
            //F32 speed_along_tangent = m_circle.velocity().x * tangent.x + m_circle.velocity().y * tangent.y;

        //    if (speed_along_normal <= 0.0F) {
        //        F32 speed_x = -speed_along_normal * normal.x * 1.0F + speed_along_tangent * tangent.x * 1.0F;
        //        F32 speed_y = -speed_along_normal * normal.y * 1.0F + speed_along_tangent * tangent.y * 1.0F;

        //        //console::log("state::Menu::update() speed_x: ", speed_x, " speed_y: ", speed_y, "\n");

        //        //m_circle.velocity_x(speed_x * 0.01F + transform::velocity(m_transform_id).x);
        //        //m_circle.velocity_y(speed_y * 0.01F + transform::velocity(m_transform_id).y);

        //        
        //        /*m_circle.add_velocity_x(speed_x * 0.05F);
        //        m_circle.add_velocity_y(speed_y * 0.05F);

        //        m_circle.add_velocity_x(transform::velocity(m_transform_id).x);
        //        m_circle.add_velocity_y(transform::velocity(m_transform_id).y);*/
        //    }   
        //}


        



        m_circle.update();








        //console::log("state::Menu::update() circle position: ", m_circle.position().x, " ", m_circle.position().y, "\n");
        //console::log("line start: ", line::start(m_line).x, " ", line::start(m_line).y, " end: ", line::end(m_line).x, " ", line::end(m_line).y, "\n");

        //console::log("state::Menu::update() transform position: ", transform::position(m_transform).x, " ", transform::position(m_transform).y, "\n");
        //console::log("state::Menu::update()\n");

        //line::update(m_line);
        //line::update(m_normal);

    }
    void Menu::draw(std::unique_ptr<Window>& window, cU8 layer) {        
        line::draw(window, m_line);
        line::draw(window, m_normal);
        line::draw(window, m_proj_on_normal);

        m_circle.draw(window);

        m_enter_text.draw(window);

    }
}