module sheet.edit;

namespace sheet {
    bool Edit::init_typing_text_bar() {
        if (!m_selection_on_level_sprites.empty()) {
            console::log("init typing prefab\n");

            //m_level_path = m_text_bar.string();
            //console::log("level path: ", m_level_path, "\n");

            std::string type_str = m_prefab_directory.string();
            type_str.append("_.bin");
            m_is_typing_text_bar = true;
            m_typing_pos = type_str.size() - 5;

            m_text_bar.set_text(type_str);

            m_prefab_path = type_str;

            return true;
        }
        if (m_is_showing_tile_set) {
            if (m_tile_set != 255) return false;
            std::string type_str = m_text_bar.string();
            m_typing_pos = type_str.size();
            type_str.append("_");
            m_text_bar.set_text(type_str);
            return true;
        } else {
            std::string level_path_str;
            if (m_text_bar.string().empty()) {
                std::filesystem::path level_path = std::filesystem::current_path() / "res" / "level" / "";
                level_path_str = level_path.string();

                std::string separator_str;
                separator_str = (char)std::filesystem::path::preferred_separator;

                size_t res_path_pos = level_path_str.find("res" + separator_str + "level", 0);

                if (res_path_pos == std::string::npos) {
                    console::log("sheet::Edit::init_typing_text_bar res" + separator_str + "level" + " not found!\n");
                    return false;
                }
                level_path_str.erase(0, res_path_pos);
                level_path_str.insert(level_path_str.size(), ".bin");
            } else {
                level_path_str = m_text_bar.string();
            }
            m_is_typing_text_bar = true;

            m_typing_pos = level_path_str.size() - 4;
            level_path_str.insert(m_typing_pos, "_");

            m_text_bar.set_text(level_path_str);

            console::log("sheet::Edit::init_typing_text_bar pos: ", m_typing_pos, "\n");
            return true;
        }
    }
    bool Edit::quit_typing_text_bar() {
        if (!m_is_typing_text_bar) return false;
        m_is_typing_text_bar = false;

        if (!m_selection_on_level_sprites.empty()) {
            console::log("sheet::Edit::quit_typing_text_bar() prefab\n");

            m_text_bar.is_hidden(false);
            m_text_bar.erase_char(m_typing_pos);

            m_prefab_path = m_text_bar.string();
            return true;
        }


        if (m_is_showing_tile_set) {
            if (m_text_bar.string().empty()) {
                console::log("sheet::Edit::quit_typing_text_bar_current_type empty!\n");
                return false;
            }
            m_text_bar.is_hidden(false);
            m_text_bar.erase_char(m_typing_pos);            

            if (m_types.find(entity::Info{ 255,  m_tile_number }) != m_types.end()) {
                m_types.at(entity::Info{ 255, m_tile_number }) = m_text_bar.string();
            } else {
                m_types.emplace(entity::Info{ 255, m_tile_number }, m_text_bar.string());
            }
            return true;
        }
        else {
            if (m_text_bar.string().size() < 16) {
                console::log("sheet::Edit::quit_typing_text_bar_current_level file name empty!\n");
                for (auto& i : m_text_bar.get_sprites()) {                    
                    sprite::is_hidden(i, false);                    
                }
                return false;
            }
            std::string level_path_str = m_text_bar.string();
            level_path_str.erase(m_typing_pos, 1);
            m_text_bar.set_text(level_path_str);
            //m_level_path = m_text_bar.string();
            m_level_path = level_path_str;
            return true;
        }
    }
    void Edit::handle_typing_text_bar() {
        if (!m_is_typing_text_bar or is_pressed(input::Key::ctrl)) return;

        ++m_time_typing_text_bar;
        if (m_time_typing_text_bar > 10) {
            m_time_typing_text_bar = 0;
            
            if (m_typing_pos < m_text_bar.get_sprites().size() and m_text_bar.get_sprites().at(m_typing_pos)) {
                sprite::is_hidden(m_text_bar.get_sprites().at(m_typing_pos), !sprite::is_hidden(m_text_bar.get_sprites().at(m_typing_pos)));

                for (auto& i : m_text_bar.get_sprites()) {
                    if (i != m_text_bar.get_sprites().at(m_typing_pos)) {
                        sprite::is_hidden(i, false);
                    }
                }
            }
        }

        //console::log("sheet::Edit::handle_typing_text_bar() ", m_typing_pos, " ", "\n");

        if (is_pressed(input::Key::del)) {
            release(input::Key::del);
            if (is_pressed(input::Key::ctrl)) {
                if (m_is_showing_tile_set) {
                    m_text_bar.clear_text();
                    if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                        m_types.erase(entity::Info{ 255, m_tile_number });
                    }
                } else {

                }
                m_is_typing_text_bar = false;
            } else {
                m_text_bar.erase_char(m_typing_pos + 1);
            }
        }
        else if (is_pressed(input::Key::backspace)) {
            release(input::Key::backspace);
            console::log("sheet::Edit::handle_typing_text_bar() typing pos: ", m_typing_pos, "\n");
            if (m_typing_pos > 10) {
                if (m_text_bar.erase_char(m_typing_pos - 1)) {
                    --m_typing_pos;
                }
            }
        }
        else if (is_pressed(input::Key::enter)) {
            release(input::Key::enter);
            save_typed_text_bar();
        }
        else if (is_pressed(input::Key::left)) {
            release(input::Key::left);
            shift_text_bar_typing_pos_left();
        }
        else if (is_pressed(input::Key::right)) {
            release(input::Key::right);
            shift_text_bar_typing_pos_right();
        }
        else {
            input::Key key = input::Key::none;
            if (is_pressed(input::Key::subtract)) {
                key = input::Key::subtract;
            } else if (is_pressed(input::Key::period)) {
                key = input::Key::period;
            } else {
                for (I32 k = input::alphabet_begin(); k != input::alphabet_end(); ++k) {
                    if (is_pressed((input::Key)k)) {
                        key = (input::Key)k; goto found_a_key;
                    }
                }
                for (I32 k = input::numbers_begin(); k != input::numbers_end(); ++k) {
                    if (is_pressed((input::Key)k)) {
                        key = (input::Key)k; goto found_a_key;
                    }
                }
                found_a_key:;
            }
            if (key != input::Key::none) {
                release(key);
                m_text_bar.insert(m_typing_pos, input::to_string(key, is_pressed(input::Key::shift)));
                ++m_typing_pos;
            }
        }
    }
    bool Edit::shift_text_bar_typing_pos_left() {
        if ((m_is_showing_tile_set and m_typing_pos < 1) or
            (!m_is_showing_tile_set and m_typing_pos < 12)) {
            return false;
        }        
        std::string text = m_text_bar.string();
        std::string c = text.substr(m_typing_pos - 1, 1);
        text.erase(m_typing_pos - 1, 1);
        text.insert(m_typing_pos, c);
        m_text_bar.set_text(text);
        m_typing_pos -= 1;
        return true;
    }
    bool Edit::shift_text_bar_typing_pos_right() {
        if ((m_is_showing_tile_set and m_typing_pos >= m_text_bar.string().size() - 1) or
            (!m_is_showing_tile_set and m_typing_pos > m_text_bar.string().size() - 6)) {
            return false;
        }
        std::string text = m_text_bar.string();
        std::string c = text.substr(m_typing_pos + 1, 1);
        text.erase(m_typing_pos + 1, 1);
        text.insert(m_typing_pos, c);
        m_text_bar.set_text(text);        
        m_typing_pos += 1;
        return true;
    }
    bool Edit::save_typed_text_bar() {
        if (m_text_bar.string().empty()) return false;        
        m_is_typing_text_bar = false;

        if (!m_selection_on_level_sprites.empty()) {
            //clear_selected_on_level();
            //m_selection_on_level_sprites.clear();

            console::log("sheet::Edit::save_typed_text_bar() level path: ", m_level_path, "\n");
            m_text_bar.set_text(m_level_path.string());

            m_time_left_saving = m_time_to_save;
            return true;
        }

        if (m_is_showing_tile_set) {
            std::string text = m_text_bar.string();
            if (text.back() == '_') {
                if (text.size() == 1) {
                    m_text_bar.clear_text();
                }
                else {
                    text.erase(m_typing_pos, 1);
                    m_text_bar.set_text(text);
                    console::log("sheet::Edit::save_typed_text_bar() text: ", text, "\n");
                }
            }
            if (m_types.find(entity::Info{ 255, m_tile_number }) == m_types.end() and !m_text_bar.string().empty()) {
                m_types.emplace(entity::Info{ 255, m_tile_number }, m_text_bar.string());                
            } else {
                if (m_text_bar.string().empty()) {
                    m_types.erase(entity::Info{ 255, m_tile_number });
                } else {
                    m_types.at(entity::Info{ 255, m_tile_number }) = m_text_bar.string();
                }
            }
            return true;
        }
        else if (!m_is_showing_tile_set) {
            if (m_text_bar.string().back() == '_' and m_text_bar.string().size() < 17 or m_text_bar.string().size() < 16) {
                console::log("sheet::Edit::save_typed_text_bar() text size: ", m_text_bar.string().size(), "\n");
                return false;
            }
            console::log("sheet::Edit::save_typed_text_bar() clicked save\n");
            //sprite::get(m_save_sprite)->rect.x = 16;
            //sprite::get(m_save_sprite)->update();

            std::string text = m_text_bar.string();
            if (text.substr(text.size() - 4, 4) == "_.bin") {
                text.erase(m_typing_pos, 1);
                m_text_bar.set_text(text);
            }
            m_level_path = m_text_bar.string();

            m_time_left_saving = m_time_to_save;

            return true;
        }
        return false;
    }
    bool Edit::save_types_to_bin() {
        console::log("sheet::Edit::save_types_to_bin()\n");
        
        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ofstream out_file(path, std::ios::out | std::ios::binary);

        U16 type_count = m_types.size();

        out_file.write((I8*)&type_count, sizeof(U16));

        for (U16 i = 0; i < 32 * 32; ++i) {
            if (m_types.find(entity::Info{ 255, i }) != m_types.end()) {
                //console::log(i, " ", m_types.at({ 255, i }), "\n");
                const char* type_data = m_types.at(entity::Info{ 255, i }).data();

                out_file.write((I8*)&i, sizeof(U16));
                out_file.write((I8*)type_data, sizeof(I8) * 32);
            }
        }
        out_file.close();
        return true;        
    }
    bool Edit::load_types_from_bin() {
        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return false;

        m_types.clear();

        U16 type_count = 0;
        in_file.read((I8*)&type_count, sizeof(U16));

        console::log("sheet::Edit::load_types_from_bin() count: ", type_count, "\n");
                
        for (U16 i = 0; i < type_count; ++i) {
            U16 number = 0;
            char type[32] = {};
            in_file.read((I8*)&number, sizeof(U16));
            in_file.read((I8*)type, sizeof(char) * 32);

            //console::log("sheet::Edit::load_types_from_bin type: ", number, " ", type, "\n");

            //const std::pair<U8, U16> tile_info = { 255, number };

            m_types.emplace(entity::Info{ 255, number }, type);
            
        }

        in_file.close();

        return true;
    }
}