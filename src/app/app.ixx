export module app;
import anim;
import console;
import bitmap_text;
import health;
import input;
import level_config;
import line;
import music;
import sprite;
import sheet;
import sheet.edit;
import sheet.game;
import sheet.game.save;
import sheet.menu;
import sheet.overlay;
import transform;
import types;
import window;
import std;

cU32 processor_count = std::thread::hardware_concurrency();

static inline std::mutex s_transition_mutex;
static inline bool s_is_running = true;

export namespace app {    
    void shutdown() {
        s_is_running = false;
    }

    export class App {
        std::unique_ptr<Window>   m_window;
        sheet::Type               m_sheet_type;

        std::list<sheet::Object*> m_sheet_objects;

        std::filesystem::path     m_level_path,
            m_level_path_to_save;

        U8 m_time_left_player_save = 0,
            m_time_to_player_save = 50;

    public:
        App() = delete;
        App(cU16 w, cU16 h, cU8 scale, const char* title) {
            m_window = std::make_unique<Window>(w, h, scale, title);

            //console::log("\033[0;31mApp::App() processor count: ", processor_count, "\033[0m\n");
            console::warning("App::App() processor count: ", processor_count, "\n");

        }
        ~App() {
            for (auto& i : m_sheet_objects) {
                if (i) delete i;
            }
            m_sheet_objects.clear();
        }

        void set_level_path_to_save(const std::filesystem::path& p) {
            m_level_path_to_save = p;
        }

        void add_sheet(sheet::cType sheet) {
            console::log("App::add_state() sheet: ", sheet::to_string(sheet), "\n");

            switch (sheet) {
            case sheet::Type::edit:
                m_sheet_objects.emplace_back(new sheet::Edit(m_window->w(), m_window->h(), m_level_path));
                break;
            case sheet::Type::game:
                if (m_level_path.empty()) {
                    m_level_path = sheet::game::read_save(0);
                    console::log("App::add_sheet() read_save(): ", m_level_path, "\n");
                    if (m_level_path.empty()) {
                        console::log("App::add_sheet() level path empty!\n");
                        m_level_path = "res/level/test.bin";
                    }
                    LevelConfig::load(m_level_path);
                    LevelConfig::stop_music();
                }
                m_sheet_objects.emplace_back(new sheet::Game(m_window->w(), m_window->h(), m_level_path, start::Info{}));
                break;
            case sheet::Type::menu_options:
                m_sheet_objects.emplace_back(new sheet::MenuOptions(m_window->w(), m_window->h()));
                break;
            case sheet::Type::menu_start:
                m_sheet_objects.emplace_back(new sheet::MenuStart(m_window->w(), m_window->h()));
                break;
            case sheet::Type::overlay:
                m_sheet_objects.emplace_back(new sheet::Overlay(m_window->w(), m_window->h()));
                break;
            }            
        }

        bool transition_sheet(sheet::Object* sheet_object, sheet::cType to) {
            console::log("App::transition_sheet() num sheets: ", m_sheet_objects.size(), "\n");

            if (!sheet_object or to == sheet::Type::none) return false;
            for (auto it = m_sheet_objects.begin(); it != m_sheet_objects.end(); ++it) {
                if ((*it) and sheet_object == (*it)) {
                    console::log("App::transition_sheet() from: ", sheet::to_string(sheet_object->sheet()), " to: ", sheet::to_string(to), "\n");

                    sheet::cType prev_sheet = sheet_object->sheet();
                    start::cInfo start_info = sheet_object->start_info();

                    delete (*it);
                    switch (to) {
                    case sheet::Type::game:
                        m_level_path = LevelConfig::level_path(sheet_object->start_info().type);

                        if (m_level_path.empty()) {
                            m_level_path = sheet::game::read_save(0);
                            console::log("App::transistion_sheet() read_player_save(): ", m_level_path, "\n");
                            if (m_level_path.empty()) {
                                console::log("App::transition_sheet() level path empty!\n");
                                m_level_path = "res/level/test.bin";
                            }
                        }
                        LevelConfig::load(m_level_path);
                        LevelConfig::stop_music();

                        (*it) = new sheet::Game(m_window->w(), m_window->h(), m_level_path, start_info);
                        break;
                    case sheet::Type::edit:
                        (*it) = new sheet::Edit(m_window->w(), m_window->h(), m_level_path);
                        break;
                    case sheet::Type::menu_options:
                        (*it) = new sheet::MenuOptions(m_window->w(), m_window->h());
                        break;
                    case sheet::Type::menu_start:
                        (*it) = new sheet::MenuStart(m_window->w(), m_window->h());
                        break;
                    }

                    (*it)->prev_sheet(prev_sheet);
                    console::log("App::transition_sheet() num sheets: ", m_sheet_objects.size(), "\n");
                    return true;
                }
            }

            return false;
        }
        void run() {
            //U32 highest_fps = 0;

            //U64 seconds_timer = 0;
            cI64 frames_until_update = 1000000000 / 60;
            //cF32 frames_until_update = 1.0F / 60.0F;
            I64  frames = frames_until_update;

            auto last_time = std::chrono::steady_clock::now();
            std::chrono::steady_clock::duration   delta_time;


            while (m_window->is_open()) {
                //seconds_timer += std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count();
                //if (seconds_timer >= 1000000) {
                    //console::log("hello ", std::chrono::duration_cast<std::chrono::microseconds>(delta_time).count(), "\n");
                    //seconds_timer = 0;
                //}

                input::handle_events(m_window);

                if (!m_window->is_in_focus()) {
                    continue;
                }

                std::set<U8> layers_to_draw;

                for (auto& sheet_object : m_sheet_objects) {
                    if (!sheet_object) continue;
                    sheet_object->update_unlocked();
                    layers_to_draw.insert(sheet_object->get_visible_layers().begin(), sheet_object->get_visible_layers().end());
                }

                /*F32 r = std::clamp(input::mouse.x, 0.0F, 255.0F);
                F32 g = std::clamp(input::mouse.x, 0.0F, 255.0F);
                F32 b = std::clamp(input::mouse.x, 0.0F, 255.0F);
                Color c{ (U8)r, (U8)g, (U8)b };
                m_window->screen_color(c);
                line::screen_color(m_window->screen_color());*/

                if (frames >= frames_until_update) {
                    frames = 0;
                    cF32 time_step = std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count() / 1000000000.0F;

                    std::vector<std::thread> threads;

                    for (auto& sheet_object : m_sheet_objects) {
                        if (!sheet_object) continue;

                        threads.emplace_back(std::thread([&]() {
                            sheet_object->update(time_step);

                            if (sheet_object->is_to_change_view()) {
                                sheet_object->is_to_change_view(false);
                                m_window->view(sheet_object->view());
                            }
                            else {
                                sheet_object->view(m_window->view());
                            }
                            if (sheet_object->is_to_player_save()) {
                                sheet_object->is_to_player_save(false);

                                if (m_time_left_player_save == 0) {
                                    m_time_left_player_save = m_time_to_player_save;

                                    sheet::game::write_save(0);
                                }
                            }
                            if (sheet_object->is_to_transition()) {
                                std::unique_lock<std::mutex> transition_lock(s_transition_mutex);
                                transition_sheet(sheet_object, sheet_object->next_sheet());
                            }
                            })
                        );
                    }
                    for (auto& i : threads) {
                        i.join();
                    }
                    threads.clear();
                    threads.emplace_back([] { transform::update(); });
                    threads.emplace_back([] {    health::update(); });
                    threads.emplace_back([] {      anim::update(); });
                    threads.emplace_back([] {    sprite::update(); });
                    threads.emplace_back([] {      line::update(); });

                    for (auto& i : threads) {
                        i.join();
                    }

                    if (m_time_left_player_save > 0) {
                        --m_time_left_player_save;
                    }
                }

                m_window->clear();
                for (U8 layer = 0; layer < 3; ++layer) {
                    /*const auto bg_sprite_ids_in_layer = sprite::bg_ids_in_layer(layer);
                    for (const auto& i : bg_sprite_ids_in_layer) {
                        sprite::draw(m_window, i);
                    }*/
                    sprite::draw_bg_in_layer(m_window, layer);
                }
                for (auto& layer : layers_to_draw) {
                    for (auto& sheet_object : m_sheet_objects) {
                        if (sheet_object) {
                            sheet_object->draw(m_window, layer);
                        }
                    }
                    //sprite::draw_in_layer(m_window, layer);
                    //line::draw_in_layer(m_window, layer);
                }
                m_window->display();

                auto current_time = std::chrono::steady_clock::now();
                delta_time = current_time - last_time;

                frames += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count();
                last_time = current_time;

                if (!s_is_running) {
                    m_window->close();
                }
            }
        }
    };


}