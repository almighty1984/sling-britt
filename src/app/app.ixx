module;
#include <chrono>
#include <fstream>
#include <filesystem>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <thread>
#include <mutex>

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
import state;
import state.edit;
import state.game;
import state.menu;
import state.overlay;
import transform;
import types;
import window;

cU32 processor_count = std::thread::hardware_concurrency();

std::mutex transition_mutex;

export class App {
    std::unique_ptr<Window>   m_window;
    state::Type               m_state_type;

    std::list<state::Object*> m_state_objects;

    std::filesystem::path     m_level_path,
                              m_level_path_to_save;

    U8 m_time_left_saving = 0,
       m_time_to_save = 50;

public:
    App() = delete;
    App(cU16 w, cU16 h, cU8 scale, const char* title) {
        m_window = std::make_unique<Window>(w, h, scale, title);

        //console::log("\033[0;31mApp::App() processor count: ", processor_count, "\033[0m\n");
        console::error("App::App() processor count: ", processor_count, "\n");
    }
    ~App() {
        for (auto& i : m_state_objects) {
            if (i) delete i;
        }
        m_state_objects.clear();
    }

    void set_level_path_to_save(const std::filesystem::path& p) {
        m_level_path_to_save = p;
    }

    const std::string read_save(cU8 number) {
        std::ostringstream oss;
        oss << (int)number;
        std::ifstream in_file("res/save/" + oss.str() + ".bin", std::ios::in | std::ios::binary);
        char saved_path[32];
        in_file.read((I8*)&saved_path, sizeof(char) * 32);
        return std::string(saved_path);
    }
    void write_save(const std::filesystem::path& level_path_to_save) {
        if (level_path_to_save.empty()) {
            console::error("App::write_save() level_path_to_save empty\n");
            return;
        }
        std::ofstream out_file("res/save/0.bin", std::ios::out | std::ios::binary);
        //char save_path[32] = m_level_path_to_save;
        out_file.write((I8*)level_path_to_save.string().c_str(), sizeof(char) * 32);
        console::log("App::write_save() ", level_path_to_save.string(), "\n");
    }

    void add_state(state::cType state) {
        console::log("App::add_state() state: ", state::to_string(state), "\n");
        if (state == state::Type::edit) {
            m_state_objects.emplace_back(new state::Edit(m_window->w(), m_window->h(), m_level_path));
        }
        else if (state == state::Type::game) {
            start::Info start_info;

            if (m_level_path.empty()) {
                m_level_path = read_save(0);
                console::log("App::set_state() read_save(): ", m_level_path, "\n");
                if (m_level_path.empty()) {
                    console::log("App::set_state() level path empty!\n");
                    m_level_path = "res/level/test.bin";
                }
                LevelConfig::load(m_level_path);
                LevelConfig::stop_music();
            }
            m_state_objects.emplace_back(new state::Game(m_window->w(), m_window->h(), m_level_path, start_info));
        }
        else if (state == state::Type::menu) {
            m_state_objects.emplace_back(new state::Menu(m_window->w(), m_window->h()));
        }
        else if (state == state::Type::overlay) {
            m_state_objects.emplace_back(new state::Overlay(m_window->w(), m_window->h()));
        }
    }

    bool transition_state(state::Object* state_object, state::cType to) {
        console::log("App::transition_state() num states: ", m_state_objects.size(), "\n");

        if (!state_object || to == state::Type::none) return false;
        for (auto it = m_state_objects.begin(); it != m_state_objects.end(); ++it) {            
            if ((*it) && state_object == (*it)) {
                console::log("App::transition_state from: ", state::to_string(state_object->state()), " to: ", state::to_string(to), "\n");

                state::cType prev_state = state_object->state();
                start::cInfo start_info = state_object->start_info();

                delete (*it);
                if (to == state::Type::game) {
                    m_level_path = LevelConfig::level_path(state_object->start_info().type);

                    if (m_level_path.empty()) {
                        m_level_path = read_save(0);
                        console::log("App::set_state() read_save(): ", m_level_path, "\n");
                        if (m_level_path.empty()) {
                            console::log("App::set_state() level path empty!\n");
                            m_level_path = "res/level/test.bin";
                        }
                    }
                    LevelConfig::load(m_level_path);
                    LevelConfig::stop_music();
                    
                    (*it) = new state::Game(m_window->w(), m_window->h(), m_level_path, start_info);
                }
                else if (to == state::Type::edit) {
                    (*it) = new state::Edit(m_window->w(), m_window->h(), m_level_path);
                }
                else if (to == state::Type::menu) {
                    (*it) = new state::Menu(m_window->w(), m_window->h());
                }
                (*it)->prev_state(prev_state);
                console::log("App::transition_state() num states: ", m_state_objects.size(), "\n");
                return true;
            }
        }

        return false;
    }
    void run() {
        /*if (!m_state) {
            m_window.close();
        }*/

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

            std::set<U8> layers_to_draw;

            for (auto& state : m_state_objects) {
                if (!state) continue;
                state->update_unlocked();

                layers_to_draw.insert(state->get_visible_layers().begin(), state->get_visible_layers().end());
            }

            if (frames >= frames_until_update) {
                frames = 0;
                cF32 time_step = std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count() / 1000000000.0F;

                input::handle_events(m_window);
                
                std::vector<std::thread> threads;

                for (auto& state_object : m_state_objects) {
                    if (!state_object) continue;

                    threads.emplace_back(std::thread([&]() {
                        state_object->update(time_step);
                        
                        if (state_object->is_to_change_view()) {
                            state_object->is_to_change_view(false);
                            m_window->view(state_object->view());
                        }
                        else {
                            state_object->view(m_window->view());
                        }
                        if (state_object->is_to_write_save()) {
                            state_object->is_to_write_save(false);
                            if (m_time_left_saving == 0) {
                                m_time_left_saving = m_time_to_save;
                                write_save(state_object->level_path_to_save());
                            }
                        }

                        if (state_object->is_to_transition()) {
                            std::unique_lock<std::mutex> transition_lock(transition_mutex);
                            transition_state(state_object, state_object->next_state());                            
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

                if (m_time_left_saving > 0) {
                    --m_time_left_saving;
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
                for (auto& state : m_state_objects) {
                    if (state) state->draw(m_window, layer);
                }
                //sprite::draw_in_layer(m_window, layer);
                //line::draw_in_layer(m_window, layer);
            }
            m_window->display();


            auto current_time = std::chrono::steady_clock::now();
            delta_time = current_time - last_time;

            frames += std::chrono::duration_cast<std::chrono::nanoseconds>(delta_time).count();
            last_time = current_time;
        }
    }
};