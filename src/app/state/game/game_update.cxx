module;
#include <algorithm>
#include <execution>
#include <sstream>

module state.game;
import camera;
import aabb;
import console;
import transform;
import random;
import entity.particle.bubble;
import entity.particle.drop;
import entity.particle.sense;
import entity.water_line;
import sound;
import particle_system;

namespace state {
    void Game::update_unlocked() {
        for (auto& i : m_unlocked_entity_objects) {
            if (i) i->update();
        }
    }
    void Game::update(cF32 ts) {
        if (is_pressed(input::Key::esc)) {
            release(input::Key::esc);
            console::log("state::Game::update() pressed esc\n");
            m_next_state = state::Type::menu;
            return;
        }

        if (is_pressed(input::Key::t)) {
            release(input::Key::t);
            console::log("state::Game::update() pressed t\n");

            m_start_info = { .type = start::Type::center,
                             .number = 0 };

            m_state = state::Type::none;
            m_next_state = state::Type::game;

            return;
        }
        if (is_pressed(input::Key::p)) {
            release(input::Key::p);
            camera::init_shake(320, { 24, 32 }, { 24, 64 }, 0.01F);
        }
        if (is_pressed(input::Key::ctrl)) {
            if (m_is_holding_ctrl) {
                if (is_pressed(input::Key::left)) {
                    camera::focal_point.x += 4.0F;
                }
                if (is_pressed(input::Key::right)) {
                    camera::focal_point.x -= 4.0F;
                }
                if (is_pressed(input::Key::up)) {
                    camera::focal_point.y += 4.0F;
                }
                if (is_pressed(input::Key::down)) {
                    camera::focal_point.y -= 4.0F;
                }
                console::log("state::Game::update camera focal point: ", camera::focal_point.x, " ", camera::focal_point.y, "\n");
            } else {
                camera::focal_point = { WINDOW_W / 2.0F, WINDOW_H / 2.0F };
            }
            m_is_holding_ctrl = true;
        } else {
            m_is_holding_ctrl = false;
        }

        //console::log("level transform: ", m_level_transform_id, " position: ", transform::get(m_level_transform_id)->position.x, "\n");
        //console::log("transforms: ", transform::size(), " unused: ", transform::unused_size(), "\n");

        particle::check_to_erase();
        particle::check_to_spawn();
        particle::update();

        camera::update();
        m_bg_planes.set_velocity(-camera::difference / 2.0F);

        m_player.update();

        m_bg_planes.update();
        //console::log("focus transform: ", camera::focus_transform, " ", m_player.get_transform_id(), "\n");

        std::for_each(/*std::execution::par_unseq, */m_entity_objects.begin(), m_entity_objects.end(),
            [](auto& entity) {
                if (entity && !entity::is_water_line(entity->type()) && !entity::is_track(entity->type())) {
                    if (!entity->is_start_in_view() &&
                        entity->state() == entity::State::dead &&
                        entity->time_left_dead() == 0 && entity->time_to_be_dead() != U16_MAX) {
                        console::log("state::Game::update() ", entity::to_string(entity->type()), " lives again!\n");
                        entity->next_state(entity->start_state());
                        entity->position(entity->start_position());
                    }
                    entity->update();
                }
            }
        );

        std::for_each(m_water_entity_objects.begin(), m_water_entity_objects.end(),
            [](auto& e) {
                if (e) e->update();
            }
        );


        if (camera::focus_transform == m_player.transform_id() &&
            (m_player.position().x < -8.0F || m_player.position().x > m_window_w - 8.0F)) {
            m_start_info = m_player.next_start();
            
            m_state         = state::Type::none;
            m_next_state    = state::Type::game;

            m_next_level = m_player.next_level();
            console::log("state::Game next level: ", start::to_string(m_next_level), "\n");
            return;
            //restart();
        }
        
        
        if (is_pressed(input::Key::f1) || is_pressed(input::Key::equal)) {
            release(input::Key::f1);
            release(input::Key::equal);
            //console::log("level path: ", m_level_path, "\n");
            
            //m_next_start = start::Type::none;            

            m_start_info = { .type = start::Type::none,
                             .number = 0 };            

            m_next_state = Type::edit;

            return;
        }

        //next_start(m_player.next_start().type);


        if (is_pressed(input::Key::g)) {
            release(input::Key::g);
            m_is_drawing_debug = !m_is_drawing_debug;
        }
        /*if (is_pressed(input::Key::enter)) {
            release(input::Key::enter);
            if (m_camera.focus_transform == m_player.get_transform_id()) {
                m_camera.focus_transform = m_player2.get_transform_id();
            } else if (m_camera.focus_transform == m_player2.get_transform_id()) {
                m_camera.focus_transform = m_player3.get_transform_id();
            } else if (m_camera.focus_transform == m_player3.get_transform_id()) {
                m_camera.focus_transform = m_player.get_transform_id();
            }
        }*/

        if (is_pressed(input::Key::space)) {
            release(input::Key::space);
            //m_sound.play();

        }

        //console::log("state::Game::update mouse: ", input::mouse.x, " ", input::mouse.y, "\n");

        /*if (is_pressed(input::Key::up)) {
            release(input::Key::up);
            console::log("Game::update pressed up\n");
        }*/

        if (m_player.is_to_save()) {
            m_is_to_player_save = true;
            m_player.is_to_save(false);
        }

        
        //m_player2.update();
        //m_player3.update();

        
       /* for (auto& i : m_entity_objects) {
            if (i && i->time_left_alive() > 0) i->time_left_alive(i->time_left_alive() - 1);
        }*/



        //console::log("entity objects size: ", m_entity_objects.size(), "\n");

        //m_sound.position({ (m_player.position().x + sprite::get(m_player.aabb_ids().front())->source_rect.w / 2.0F) / m_window_w,
        //                   (m_player.position().y + sprite::get(m_player.aabb_ids().front())->source_rect.h / 2.0F) / m_window_h });

        //console::log("state::Game::update sound position: ", m_sound.position().x, " ", m_sound.position().y, "\n");

        //console::log("quadnode x:", m_level_quad_trees.front().second->x(), "\n");




        //console::log("position: ", m_player.position().x, " ", m_player.position().y, "\n");
        //console::log("position: ", m_player.position().x + m_camera.position.x, " ", m_player.position().y + m_camera.position.y, "\n");

        //console::log("level position: ", transform::get(m_level_transform_id)->position.x, " ", transform::get(m_level_transform_id)->position.y, "\n");


        //console::log("camera position: ", m_camera.position.x, " ", m_camera.position.y, "\n");

        //if (m_player.position().x < -collider::aabb::get(m_player.aabb_ids().front())->get_rect().w || m_player.position().x > m_window_w) {

        
        /*else if (m_camera.focus_transform == m_player2.get_transform_id() &&
            (m_player2.position().x < -8.0F || m_player2.position().x > m_window_w)) {
            start_info(m_player2.next_start());
            restart();
        } else if (m_camera.focus_transform == m_player3.get_transform_id() &&
            (m_player3.position().x < -8.0F || m_player3.position().x > m_window_w)) {
            start_info(m_player3.next_start());
            restart();
        }*/


        if (is_pressed(input::Key::grave)) {
            release(input::Key::grave);
            m_is_to_change_view = true;
            if (view() == RectF{0.0F, 0.0F, 480.0F, 270.0F}) {
                view(RectF{ 0.0F, 0.0F, 320.0F, 180.0F });
            } else {
                view(RectF{ 0.0F, 0.0F, 480.0F, 270.0F });
            }
        }


        quad_trees_check_collision();
    }
}