module sheet.game;
import app.config;
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
import sheet.game.save;

namespace sheet {
    void Game::update_unlocked() {
        for (auto& i : m_unlocked_entity_objects) {
            if (i) i->update();
        }
    }
    void Game::update(cF32 ts) {
        if (is_pressed(input::Key::esc)) {
            release(input::Key::esc);
            console::log("state::Game::update() pressed esc\n");
            sheet::game::clear_current_save_data();
            m_next_sheet = sheet::Type::menu_start;
            return;
        }
        if (is_pressed(input::Key::f1) or is_pressed(input::Key::equal)) {
            release(input::Key::f1);
            release(input::Key::equal);
            //console::log("level path: ", m_level_path, "\n");
            //m_next_start = start::Type::none;

            m_start_info = { .type = start::Type::none,
                             .number = 0 };

            m_next_sheet = Type::edit;
            return;
        }

        if (is_pressed(input::Key::t)) {
            release(input::Key::t);
            console::log("sheet::Game::update() pressed t\n");

            m_start_info = { .type = start::Type::center,
                             .number = 0 };

            m_sheet      = sheet::Type::none;
            m_next_sheet = sheet::Type::game;

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
            }
            else {
                camera::focal_point = { view().w / 2.0F, view().h / 2.0F };
            }
            m_is_holding_ctrl = true;
        } else {
            m_is_holding_ctrl = false;
        }

        //console::log("state::Game::update() window size: ", app::config::extent().x, " ", app::config::extent().y, "\n");

        //console::log("level transform: ", m_level_transform_id, " position: ", transform::get(m_level_transform_id)->position.x, "\n");
        //console::log("transforms: ", transform::size(), " unused: ", transform::unused_size(), "\n");

        particle::check_to_erase();
        particle::check_to_spawn();
        particle::update();

        //console::log("state::Game::update() view: ", view().w, " ", view().h, "\n");

        camera::update(view());
        m_bg_planes.set_velocity(-camera::difference / 2.0F);

        m_player.update();

        m_bg_planes.update();
        //console::log("focus transform: ", camera::focus_transform, " ", m_player.get_transform_id(), "\n");

        std::for_each(/*std::execution::par_unseq, */m_entity_objects.begin(), m_entity_objects.end(),
            [](auto& entity) {
                if (entity and !entity::is_water_line(entity->type()) and !entity::is_track(entity->type())) {
                    if (!entity->is_start_in_view() and
                        entity->state() == state::Type::dead and
                        entity->time_left_dead() == 0 and entity->time_to_be_dead() != U16_MAX) {
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


        if (camera::focus_transform == m_player.transform_id() and
            (m_player.position().x < -8.0F or m_player.position().x > m_window_w - 8.0F)) {
            m_start_info = m_player.next_start();
            
            m_sheet         = sheet::Type::none;
            m_next_sheet    = sheet::Type::game;

            m_next_level = m_player.next_level();
            console::log("state::Game next level: ", start::to_string(m_next_level), "\n");
            return;
            //restart();
        }

        //next_start(m_player.next_start().type);

        if (is_pressed(input::Key::g)) {
            release(input::Key::g);
            m_is_drawing_debug = !m_is_drawing_debug;
        }
        if (is_pressed(input::Key::q)) {
            release(input::Key::q);
            m_is_drawing_quad_tree = !m_is_drawing_quad_tree;
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
            if (i and i->time_left_alive() > 0) i->time_left_alive(i->time_left_alive() - 1);
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

        //if (m_player.position().x < -collider::aabb::get(m_player.aabb_ids().front())->get_rect().w or m_player.position().x > m_window_w) {

        
        /*else if (m_camera.focus_transform == m_player2.get_transform_id() and
            (m_player2.position().x < -8.0F or m_player2.position().x > m_window_w)) {
            start_info(m_player2.next_start());
            restart();
        } else if (m_camera.focus_transform == m_player3.get_transform_id() and
            (m_player3.position().x < -8.0F or m_player3.position().x > m_window_w)) {
            start_info(m_player3.next_start());
            restart();
        }*/


        if (is_pressed(input::Key::grave)) {
            release(input::Key::grave);
            m_is_to_change_view = true;
            if (view() == RectU{0, 0, 480, 270 }) {
                view(RectU{ 0, 0, app::config::extent().x, app::config::extent().y });
            } else {
                view(RectU{ 0, 0, 480, 270 });
            }
        }

        //app::config::view(view());


        quad_trees_check_collision();
    }
}