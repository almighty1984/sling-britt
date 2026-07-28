module sheet.game;
import std;
import sheet.game.save;
import camera;
import aabb;
import console;
import input;
import line;
//import quad_tree;
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

        //m_entities.emplace_back(std::make_unique<entity::Player>());
        //m_entities.back()->level_transform = m_level_transform;
        //m_entities.back()->position(m_start_position);


        sheet::game::current_level_path(level_path);
        sheet::game::add_visited_level(level_path);

        collision_grid::reset();

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
        m_visible_layers.insert(NUM_VISIBLE_LAYERS - 2);
        m_visible_layers.insert(NUM_VISIBLE_LAYERS - 3);
                

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
        //Vec2F extent = m_num_level_tiles;
        //I32 depth = m_num_level_tiles.x / m_num_level_tiles.y;
        //if (depth < 4) depth = 4;
        //console::log("depth: ", depth, "\n\n");

        console::log("m_num_level_tiles: ", m_num_level_tiles.x, " ", m_num_level_tiles.y, "\n\n");
        //cVec2F extent = { 128, 128 };

        collision_grid::init(m_grid_coords);

        m_mouse_transform = transform::make();

        test_aabb = aabb::make(m_mouse_transform, { 0, 0, 16, 16 });
    }


    //void Game::check_collision() {
    //    std::vector<std::thread> threads;
    //    for (std::pair<Vec2I, QuadTreeNode*>& quad_tree_node : m_level_quad_trees) {
    //        if (!quad_tree_node.second) continue;
    //
    //        auto check_collision_lambda = [&]() {
    //            std::unique_lock<std::mutex> quad_tree_node_lock(quad_tree_node_mutex);
    //
    //            quad_tree_node.second->clear();
    //            quad_tree_node.second->init(
    //                { (quad_tree_node.first.x - 1) * 256.0F + transform::position(m_level_transform).x,
    //                  (quad_tree_node.first.y - 1) * 256.0F + transform::position(m_level_transform).y,
    //                256.0F, 256.0F });
    //
    //            //for (auto& i : aabb::get_aabbs()) {
    //            for (I32 aabb : aabb::aabb_ids()) {
    //            //for (I32 aabb = 0; aabb < aabb::size(); ++aabb) {
    //                if (!aabb::is_active(aabb)) continue;
    //
    //                //aabb->quad_tree_node.clear();
    //                //quad_tree_node.second->insert_aabb(aabb);
    //                quad_tree_node.second->insert_point(aabb, aabb::UL(aabb));
    //                quad_tree_node.second->insert_point(aabb, aabb::UR(aabb));
    //                quad_tree_node.second->insert_point(aabb, aabb::DL(aabb));
    //                quad_tree_node.second->insert_point(aabb, aabb::DR(aabb));
    //            }
    //            quad_tree_node_lock.unlock();
    //
    //            quad_tree_node.second->check_collision();
    //            };
    //        threads.emplace_back(std::thread(check_collision_lambda));
    //    }
    //    //console::log("sheet::Game::quad_trees_check_collision() num threads: ", threads.size(), "\n");
    //    for (auto& thread : threads) {
    //        if (thread.joinable()) {
    //            thread.join();
    //        }
    //    }
    //}
}