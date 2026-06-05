module sheet.game;
import std;
import sheet.game.save;
import camera;
import aabb;
import console;
import input;
import line;
import quad_tree;
import sound;
import sprite;
import transform;
import types;

//import collision_grid;

std::mutex quad_tree_node_mutex;

namespace sheet {
    Game::Game(cU16 window_w, cU16 window_h, std::filesystem::path level_path, start::Info start) {
        //console::log("Game::Game level: ", level_path, " start: ", start_position.x, " ", start_position.y, "\n");
        m_type = m_transition_to = m_transition_from = Type::game;
        m_window_w = window_w, m_window_h = window_h;

        console::warning("sheet::Game::Game() window extent: ", window_w, " ", window_h, "\n");
        console::warning("sheet::Game::Game() view: ", view().w, " ", view().h, "\n");

        start_info(start);

        console::log("sheet::Game::Game() transform::size: ", transform::size(), "\n");
        console::log("sheet::Game::Game() transform::unused_size: ", transform::unused_size(), "\n");

        m_bg_planes.create(3, 1);

        for (auto& i : m_bg_planes.get_sprites()) {
            m_visible_layers.insert(sprite::layer(i));
        }        

        m_level_transform = transform::make();

        transform::level_id(m_level_transform);

        console::log("sheet::Game::Game() m_level_transform: ", m_level_transform, "\n");

        //console::log("level transform: ", m_level_transform, "\n");

        //m_entity_objects.emplace_back(std::make_unique<entity::Player>());
        //m_entity_objects.back()->level_transform = m_level_transform;
        //m_entity_objects.back()->position(m_start_position);


        sheet::game::current_level_path(level_path);
        sheet::game::add_visited_level(level_path);

        load_level(level_path);
        m_level_path = level_path;

        

        for (auto& i : sheet::game::visited_levels()) {
            console::log("sheet::Game::Game() visited level: ", i, "\n");
        }

        for (auto& i : sheet::game::picked_coins_in_current_level()) {
            console::log("sheet::Game::Game() picked coins in current level: ", i, "\n");
        }

        /*for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            add_visible_layer(i);
        }*/

        m_visible_layers.insert(NUM_VISIBLE_LAYERS - 1);  // for particles like blood
                

        //m_player.start_offset(m_start_position);
        //m_player.load_config("res/entity/player/player.cfg");
        
        m_player.position(m_start_position + Vec2F{ 0.0F, 16.0F - aabb::h(m_player.aabb(aabb::Name::body)) });

        console::log("sheet::Game() start position: ", m_start_position.x, " ", m_start_position.y, "\n");

        //m_player.position() = Vec2F{ 320.0F, 0.0F };
        
        //m_player.id = 0;

        //m_player2.position(m_start_position - Vec2F{ 16.0F, 0.0F });


        //m_player2.id = 1;

        /*m_player2.key_up = input::Key::w;
        m_player2.key_down = input::Key::s;
        m_player2.key_left = input::Key::a;
        m_player2.key_right = input::Key::d;
        m_player2.key_jump = input::Key::g;
        m_player2.key_sprint = input::Key::h;
        m_player2.key_melee = input::Key::f;*/

        /*m_player3.set_layer(NUM_VISIBLE_LAYERS - 1);
        m_player3.position(m_start_position + Vec2F{ 16.0F, 0.0F });

        m_player3.key_up = input::Key::i;
        m_player3.key_down = input::Key::k;
        m_player3.key_left = input::Key::j;
        m_player3.key_right = input::Key::l;
        m_player3.key_jump = input::Key::r;
        m_player3.key_sprint = input::Key::e;
        m_player3.key_melee = input::Key::t;*/

        //m_player3.id = 2;

        //transform::get(m_transform)->velocity = { 2.0F, 1.0F };
        //transform::get(m_transform)->deceleration = { 0.001f, 0.001f };
        //console::log("Game transform: ", m_transform, "\n");

        m_input = input::make();

        //console::log("sprite::Objects size: ", sprite::size(), "\n");



       /* for (auto& i : m_bg_planes.get_transforms()) {
            m_camera.add_transform(i);
        }*/

        //camera::clear();
        camera::add_transform(m_level_transform);
        camera::add_transform(m_player.transform());

        camera::focus_transform = m_player.transform();
        //m_camera.add_transform(m_player2.get_transform());
        //m_camera.add_transform(m_player3.get_transform());

        //Vec2F camera_position = m_player.position() - Vec2F{ window_w / 2.0F, window_h / 2.0F };
        cVec2F camera_position = m_start_position - Vec2F{ window_w / 2.0F, window_h / 2.0F + 6.0F };


        //console::log("\nsheet::Game::Game camera position: ", camera_position.x, " ", camera_position.y, "\n\n");

        camera::set_position(camera_position);

        //m_camera.add_transform(m_transform);
        camera::focus_offset = {};

        //Vec2I num_cells = m_num_level_tiles / 16;
        //console::log("num cells: ", num_cells.x, " ", num_cells.y, "\n");
        //collision_grid::init(num_cells);

        //Vec2F extent = m_num_level_tiles.x > m_num_level_tiles.y ? Vec2F{ m_num_level_tiles.x, m_num_level_tiles.x } : Vec2F{ m_num_level_tiles.y, m_num_level_tiles.y };
        Vec2F extent = m_num_level_tiles;
        I32 depth = m_num_level_tiles.x / m_num_level_tiles.y;
        if (depth < 4) depth = 4;
        console::log("depth: ", depth, "\n\n");
        collision_grid::init(extent, depth);

        m_mouse_transform = transform::make();

        test_aabb = aabb::make(m_mouse_transform, { 0, 0, 16, 16 });
    }

    void Game::check_to_add_input_from(entity::Object* trigger_entity) {
        //if (trigger_entity->type() != entity::Type::track_trigger_UL) {
        //    //console::log("")
        //    return;
        //}

        entity::Object* current_entity = nullptr;

        entity::Object* prev_entity = trigger_entity;
        entity::Object* start_of_loop_entity = nullptr;

        Vec2F current_pos = {};

        Vec2F offset_to_next  = { 0.0F, -16.0F },
              offset_to_next2 = { 0.0F,   0.0F };
        for (auto& entity : m_unlocked_entity_objects) {
            if (entity->start_offset() == prev_entity->start_offset() + offset_to_next) {
                //console::log("sheet::Game::check_to_add_input_from() entity above trigger: ", entity::to_string(entity->type()), "\n");
                //current_offset = trigger_entity->start_offset() + Vec2F{ 0.0F,-16.0F };
                bool is_track_found = false;
                switch (entity->type()) {
                    case entity::Type::conduit_corner_UR: {
                        offset_to_next = { -16.0F, 0.0F };
                        break;
                    }
                    case entity::Type::track_trigger_UL: {
                        offset_to_next = { -16.0F, 0.0F };
                        is_track_found = true;
                        start_of_loop_entity = entity;
                        break;
                    }
                    case entity::Type::conduit_corner_UL: {
                        offset_to_next = { 16.0F, 0.0F };
                        break;
                    }
                    case entity::Type::track_trigger_UR: {
                        offset_to_next = { 16.0F, 0.0F };
                        is_track_found = true;
                        start_of_loop_entity = entity;
                        break;
                    }
                    case entity::Type::conduit_UD: {
                        offset_to_next = { 0.0F, -16.0F };
                        break;
                    }
                    case entity::Type::track_UD: {
                        is_track_found = false;
                        offset_to_next = { 0.0F, -16.0F };
                        break;
                    }
                }

                entity->add_input(prev_entity);

                console::log(to_string(entity->type()), " add input: ", to_string(prev_entity->type()), "\n");

                entity->direction(offset_to_next);
                prev_entity = entity;

                current_entity = entity;
                current_pos = entity->start_offset();                
                
                if (is_track_found) {
                    console::log("state::Game::check_to_add_input_from() found trigger direction: ", entity->direction().x, " ", entity->direction().y, "\n");                    
                    break;
                }
            }
        }



        while (1) {
            cVec2F pos      = current_entity->start_offset(),
                   prev_pos = prev_entity->start_offset();

            offset_to_next = {};
            offset_to_next2 = {};

            console::log("current entity: ", to_string(current_entity->type()), "\n");
            switch (current_entity->type()) {                
                case entity::Type::track_trigger_UL: {
                    offset_to_next = { -16.0F, 0.0F };
                    start_of_loop_entity = current_entity;
                    break;
                }                
                case entity::Type::track_trigger_UR: {
                    offset_to_next = { 16.0F, 0.0F };
                    start_of_loop_entity = current_entity;
                    break;
                }
                case entity::Type::conduit_UD:
                case entity::Type::logic_not_UD:
                case entity::Type::track_UD: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ 0.0F,  16.0F } : Vec2F{ 0.0F, -16.0F };
                    break;
                }                
                case entity::Type::conduit_LR:
                case entity::Type::track_LR: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_corner_DL:
                case entity::Type::track_corner_DL: {
                    offset_to_next = prev_pos.x > pos.x ? Vec2F{ 0.0F, -16.0F } : Vec2F{ 16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_corner_UL:
                case entity::Type::track_corner_UL: {                    
                    offset_to_next = prev_pos.x > pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ 16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_corner_UR:
                case entity::Type::track_corner_UR: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_corner_DR:
                case entity::Type::track_corner_DR: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 0.0F, -16.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_L_1x1_0:
                case entity::Type::track_L_1x1_0: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_L_1x1_1:
                case entity::Type::track_L_1x1_1: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ 0.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_R_1x1_0:
                case entity::Type::track_R_1x1_0: {
                    offset_to_next = prev_pos.y > pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ 0.0F,  16.0F };
                    break;
                }
                case entity::Type::conduit_R_1x1_1:
                case entity::Type::track_R_1x1_1: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ -16.0F,   0.0F } : Vec2F{ 0.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_L_1x2_0:
                case entity::Type::track_L_1x2_0: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_L_1x2_1:
                case entity::Type::track_L_1x2_1: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ 16.0F,  16.0F } : Vec2F{ 0.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_R_1x2_0:
                case entity::Type::track_R_1x2_0: {
                    offset_to_next = prev_pos.y < pos.y ? Vec2F{ 0.0F,   16.0F } : Vec2F{ 16.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_R_1x2_1:
                case entity::Type::track_R_1x2_1: {
                    if (prev_pos.y < pos.y) {
                        offset_to_next = { -16.0F,  16.0F };
                        offset_to_next2 = { 0.0F,  16.0F };
                    } else {
                        offset_to_next = { 0.0F, -16.0F };
                    }
                    break;
                }
                case entity::Type::conduit_L_2x1_0:
                case entity::Type::track_L_2x1_0: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 16.0F,  16.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
                case entity::Type::conduit_L_2x1_1:
                case entity::Type::track_L_2x1_1: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F, -16.0F };
                    break;
                }
                case entity::Type::conduit_R_2x1_0:
                case entity::Type::track_R_2x1_0: {
                    offset_to_next = prev_pos.y > pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F,  16.0F };
                    break;
                }
                case entity::Type::conduit_R_2x1_1:
                case entity::Type::track_R_2x1_1: {
                    if (prev_pos.x < pos.x) {
                        offset_to_next = { 16.0F, -16.0F };
                        offset_to_next2 = { 16.0F,   0.0F };
                    } else {
                        offset_to_next = { -16.0F,   0.0F };
                    }
                    break;
                }
                case entity::Type::logic_not_LR: {
                    offset_to_next = prev_pos.x < pos.x ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F,   0.0F };
                    break;
                }
            }

            //console::log("current: ", to_string(current_entity->type()), " offset to next: ", offset_to_next.x, " ", offset_to_next.y, "\n");

            
            auto it = std::find_if(m_unlocked_entity_objects.begin(), m_unlocked_entity_objects.end(),
                [current_pos, offset_to_next](const entity::Object* e) {
                    return (is_conduit(e->type()) or is_logic(e->type()) or is_track(e->type())) and
                        offset_to_next != Vec2F{} and
                        e->start_offset() == current_pos + offset_to_next;
                });

            if (it != m_unlocked_entity_objects.end()) {
                if ((*it) == start_of_loop_entity) {
                    console::log("1: found start of loop: ", to_string((*it)->type()), "\n");
                    return;
                }
                prev_entity = current_entity;
                (*it)->add_input(prev_entity);

                //console::log(to_string((*it)->type()), " add input: ", to_string(prev_entity->type()), "\n");
                (*it)->direction(pos - prev_pos);
                //console::log("1: ", to_string((*it)->type()), " ", (*it)->direction().x, " ", (*it)->direction().y, "\n");
                current_entity = (*it);
                current_pos = (*it)->start_offset();
                continue;
            }
            
            auto it2 = std::find_if(m_unlocked_entity_objects.begin(), m_unlocked_entity_objects.end(),
                [current_pos, offset_to_next2](const entity::Object* e) {
                    return (is_conduit(e->type()) or is_logic(e->type()) or is_track(e->type())) and
                        offset_to_next2 != Vec2F{} and
                        e->start_offset() == current_pos + offset_to_next2;
                });

            if (it2 != m_unlocked_entity_objects.end()) {
                if ((*it2) == start_of_loop_entity) {
                    console::log("2: found start of loop: ", to_string((*it2)->type()), "\n");
                    return;
                }
                prev_entity = current_entity;
                (*it2)->add_input(prev_entity);
                (*it2)->direction(pos - prev_pos);
                //console::log("2: ", to_string((*it2)->type()), " ", (*it2)->direction().x, " ", (*it2)->direction().y, "\n");
                current_entity = (*it2);
                current_pos = (*it2)->start_offset();
                continue;
            }

            console::log(class_name(), "::check_to_add_input_from() no path forward found.\n");
            return;        
        }
    }

    void Game::check_collision() {
        std::vector<std::thread> threads;
        for (std::pair<Vec2I, QuadTreeNode*>& quad_tree_node : m_level_quad_trees) {
            if (!quad_tree_node.second) continue;

            auto check_collision_lambda = [&]() {
                std::unique_lock<std::mutex> quad_tree_node_lock(quad_tree_node_mutex);

                quad_tree_node.second->clear();
                quad_tree_node.second->init(
                    { (quad_tree_node.first.x - 1) * 256.0F + transform::position(m_level_transform).x,
                      (quad_tree_node.first.y - 1) * 256.0F + transform::position(m_level_transform).y,
                    256.0F, 256.0F });

                //for (auto& i : aabb::get_aabbs()) {
                for (I32 aabb = 0; aabb < aabb::size(); ++aabb) {
                    if (!aabb::is_active(aabb)) continue;

                    //aabb->quad_tree_node.clear();
                    //quad_tree_node.second->insert_aabb(aabb);
                    quad_tree_node.second->insert_point(aabb, aabb::UL(aabb));
                    quad_tree_node.second->insert_point(aabb, aabb::UR(aabb));
                    quad_tree_node.second->insert_point(aabb, aabb::DL(aabb));
                    quad_tree_node.second->insert_point(aabb, aabb::DR(aabb));
                }
                quad_tree_node_lock.unlock();

                quad_tree_node.second->check_collision();
                };
            threads.emplace_back(std::thread(check_collision_lambda));
        }
        //console::log("sheet::Game::quad_trees_check_collision() num threads: ", threads.size(), "\n");
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}