module sheet.edit;
import console;
import transform;
import sprite;
import types;

namespace sheet {
    bool Edit::load_types_from_text_file(const std::filesystem::path& path) {
        std::ifstream in_file(path);

        std::ostringstream oss;
        oss << in_file.rdbuf();
        in_file.close();

        std::string file_text = oss.str();

        const size_t start = file_text.find("set_255", 0);
        if (start == std::string::npos) {
            console::error("sheet::Edit::load_types_from_text_file() set_255 not found\n");
            return false;
        }

        const size_t open_bracket = file_text.find("{", start);
        if (open_bracket == std::string::npos) {
            console::error("sheet::Edit::load_types_from_text_file() set_255 open bracket not found\n");
            return false;
        }
        const size_t close_bracket = file_text.find("}", open_bracket);
        if (close_bracket == std::string::npos) {
            console::error("sheet::Edit::load_types_from_text_file() set_255 close bracket not found\n");
            return false;
        }

        size_t label_start = file_text.find("\n", open_bracket);
        while (label_start < close_bracket) {
            while (file_text.at(label_start) == '	' or file_text.at(label_start) == ' ' or file_text.at(label_start) == '\n') {
                ++label_start;
            }
            const size_t label_end = file_text.find(" ",  label_start);
            const size_t equals    = file_text.find("=",  label_start);
            const size_t end_line  = file_text.find("\n", label_start);                        

            const std::string number_label = file_text.substr(label_start, label_end - label_start);

            cU16 number = std::stoi(number_label);

            size_t type_start = equals + 1;
            while (file_text.at(type_start) == '	' or file_text.at(type_start) == ' ' or file_text.at(type_start) == '\n') {
                ++type_start;
            }
            const std::string type = file_text.substr(type_start, end_line - type_start);

            m_types.emplace(entity::Info{ 255, number }, type);
            label_start = end_line + 1;
        }
        return true;
    }
    void Edit::init_tile_set(cU8 tile_set) {
        m_prev_tile_set = m_tile_set;
        m_tile_set = tile_set;
        const std::string tile_set_str = std::to_string((int)m_tile_set);
        m_text_current_tile_set.set_text(tile_set_str);
        sprite::texture(m_tile_set_sprite, "res/texture/set_" + tile_set_str + ".png");
        sprite::texture(m_current_tile_sprite, "res/texture/set_" + tile_set_str + ".png");

        if (!m_is_showing_tile_set) return;
        
        m_text_bar.clear_text();

        if (tile_set == 255) {
            if (!m_selection_on_tile_set_sprites.empty() and 
                m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::is_hidden(m_save_sprite, false);
        } else {
            //m_text_bar.clear_text();
            sprite::is_hidden(m_save_sprite, true);
        }        
                
        console::log("sheet::Edit::init_tile_set tile_number: ", m_tile_number, "\n");
        
        //deselect_all_on_tile_set();
    }
    void Edit::enter_tile_set() {
        console::log("sheet::Edit::enter_tile_set\n");
        //transform::get(m_tile_set_bg_transform)->position.y = 0.0F;
        sprite::is_hidden(m_tile_set_bg_sprite, false);
        sprite::is_hidden(m_tile_set_sprite, false);

        sprite::is_hidden(m_text_current_tile_set_bg_sprite, true);
        
        for (auto& i : m_is_hidden_layer_sprites) {
            sprite::layer(i, 0);
        }
        sprite::is_hidden(m_active_layer_sprite, true);
        sprite::is_hidden(m_position_on_grid_map_sprite, true);

        for (auto& i : m_grid_sprites) {
            sprite::is_hidden(i, true);
        }
        for (auto& i : m_grid_map_sprites) {
            sprite::is_hidden(i, true);
        }
        sprite::is_hidden(m_position_on_grid_map_sprite, true);
        sprite::is_hidden(m_grid_icon_sprite, true);

        sprite::is_hidden(m_text_current_tile_set_bg_sprite, false);
        m_text_current_tile_set.is_hidden(false);
        if (m_tile_set == 255) {
            if (m_types.find(entity::Info{ 255, m_tile_number }) == m_types.end()) {
                m_text_bar.clear_text();
            } else {
                m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
            }
            sprite::is_hidden(m_save_sprite, false);
        } else {
            m_text_bar.clear_text();
            sprite::is_hidden(m_save_sprite, true);
        }
        //sprite::get(m_tile_set_sprite)->is_hidden = false;

        for (auto& i : m_selection_on_tile_set_sprites) {
            sprite::is_hidden(i, false);
        }
    }
    void Edit::exit_tile_set() {
        console::log("sheet::Edit::exit_tile_set\n");
        sprite::is_hidden(m_tile_set_bg_sprite, true);
        sprite::is_hidden(m_tile_set_sprite, true);
        //transform::get(m_tile_set_bg_transform)->position.y = 512.0F;

        //transform::get(m_tile_set_transform)->position.x = view().w - 32.0F;
        //transform::get(m_tile_set_transform)->position.y = view().w == 480.0F ? 240.0F : 160.0F;

        /*if (!m_selection_on_tile_set_sprites.empty() and sprite::get(m_selection_on_tile_set_sprites.back())) {
            cVec2F tile_offset = sprite::get(m_selection_on_tile_set_sprites.back())->offset;
            sprite::get(m_tile_set_sprite)->rect = { (I32)tile_offset.x, (I32)tile_offset.y, 16, 16 };
            sprite::get(m_tile_set_sprite)->layer = MENU_LAYER + 1;
        }*/

        sprite::is_hidden(m_text_current_tile_set_bg_sprite, true);

        m_text_current_tile_set.is_hidden(true);
           
        for (auto& i : m_is_hidden_layer_sprites) {
            sprite::layer(i, MENU_TEXT_LAYER);
        }
        sprite::is_hidden(m_active_layer_sprite, false);
        sprite::is_hidden(m_save_sprite, false);
        sprite::is_hidden(m_grid_icon_sprite, false);
        
        for (auto& i : m_grid_sprites) {
            sprite::is_hidden(i, m_is_hidden_grid);
        }
        for (auto& i : m_grid_map_sprites) {
            sprite::is_hidden(i, m_is_hidden_grid_map);
        }
        sprite::is_hidden(m_position_on_grid_map_sprite, m_is_hidden_grid_map);
        

        if (m_level_path.empty()) {
            m_text_bar.clear_text();
        } else {
            m_text_bar.set_text(m_level_path.string());
        }

        for (auto& i : m_selection_on_tile_set_sprites) {
            sprite::is_hidden(i, true);
        }
    }
    void Edit::move_tile_set(cVec2F amount) {
        //console::log("sheet::Edit::move_tile_set ", amount.x, " ", amount.y, " ", " position: ", transform::get(m_tile_set_transform)->position.x, " ", transform::get(m_tile_set_transform)->position.y, "\n");        

        //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
        cF32 edge_y = 0.0F;
        cVec2F prev_position = transform::position(m_tile_set_transform);
        
        cVec2F limit = { view().w - 512.0F - 32.0F,
                         view().h == 270.0F ? -272.0F : -352.0F };

        transform::position_add(m_tile_set_transform, amount);
        if (transform::position(m_tile_set_transform).y > edge_y) {            
            transform::position_y(m_tile_set_transform, edge_y);
        } else if (transform::position(m_tile_set_transform).y < edge_y + limit.y) {
            transform::position_y(m_tile_set_transform, edge_y + limit.y);
        }
        if (transform::position(m_tile_set_transform).x > 0.0F) {
            transform::position_x(m_tile_set_transform, 0.0F);
        } else if (transform::position(m_tile_set_transform).x < limit.x) {
            transform::position_x(m_tile_set_transform, limit.x);
        }        
    }
    bool Edit::select_on_tile_set(cVec2F offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprites) {
            if (sprite::offset(sel_id) == offset) {
                console::log("sheet::Edit::select_on_tile_set", offset.x, " ", offset.y, " already selected\n");
                return false;
            }
        }
        //console::log("sheet::Edit::select_on_tile_set\n");
        cI32 sprite = sprite::make("res/texture/tile_selection.png");
        sprite::layer(sprite, SELECTION_ON_TILE_SET_LAYER);
        sprite::transform(sprite, m_tile_set_transform);
        sprite::offset(sprite, offset);
        m_selection_on_tile_set_sprites.push_back(sprite);

        sprite::rect(m_current_tile_sprite, { (I32)offset.x, (I32)offset.y, 16,16 });

        if (m_tile_set != 255) {
            m_text_bar.clear_text();
        }
        else if (m_tile_set == 255 and m_selection_on_tile_set_sprites.size() == 1) {
            cVec2F tile_offset = offset / 16.0F;
            m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

            //if (m_types.at(entity::Info{ 255, m_tile_number }) == m_text_bar.string()) {
                //return false;
            //}

            if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                if (m_text_bar.string() != m_types.at(entity::Info{ 255, m_tile_number })) {
                    m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                }
                //console::log("sheet::Edit::select_on_tile_set() text bar: ", m_text_bar.string(), "\n");
                //int num = 0;
                //console::log("sheet::Edit::select_on_tile_set text_bar size: ", m_text_bar.get_sprites().size(), "\n");
                
                //for (auto& i : m_text_bar.get_sprites()) {
                //    if (!sprite::get(i)) {
                //        console::log("text_bar sprite not valid\n");
                //    }
                    //console::log(num, " ", sprite::get(i)->rect.x, " ", sprite::get(i)->rect.y, " ", sprite::get(i)->rect.w, " ", sprite::get(i)->rect.h, "\n");
                    //console::log(num, " ", (int)sprite::get(i)->layer, "\n");
                //}
                //++num;
                //}
            }
            else {
                m_text_bar.clear_text();
            }
        }
        return true;
    }
    bool Edit::deselect_on_tile_set(cVec2F offset) {
        for (auto& sel_id : m_selection_on_tile_set_sprites) {
            if (sprite::transform(sel_id) != m_tile_set_transform) continue;
            if (sprite::offset(sel_id) == offset) {
                console::log("sheet::Edit::deselect_on_tile_set aready selected, deselecting\n");
                sprite::erase(sel_id);
                std::vector<I32> resized_selection;
                for (auto& i : m_selection_on_tile_set_sprites) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_tile_set_sprites.clear();
                m_selection_on_tile_set_sprites = resized_selection;
                return true;
            }
        }
        return false;
    }
    bool Edit::move_selection_on_tile_set(cVec2F amount) {
        console::log("sheet::Edit::move_selection_on_tile_set ", amount.x, " ", amount.y, "\n");
        if (m_selection_on_tile_set_sprites.empty()) return false;
        for (auto& i : m_selection_on_tile_set_sprites) {
            if (sprite::offset(i).x + amount.x < 0 or sprite::offset(i).y + amount.y < 0 or
                sprite::offset(i).x + amount.x > 496.0F or sprite::offset(i).y + amount.y > 496.0F) {
                return false;
            }

            //cF32 edge_y = m_is_showing_tile_set ? 0.0F : 512.0F;
            if (m_is_showing_tile_set) {
                cF32 edge_y = 0.0F;
                if (amount.x < 0.0F and transform::position(m_tile_set_transform).x + sprite::offset(i).x + amount.x < 0.0F or
                    amount.x > 0.0F and transform::position(m_tile_set_transform).x + sprite::offset(i).x + amount.x > view().w - 48.0F or
                    amount.y < 0.0F and transform::position(m_tile_set_transform).y + sprite::offset(i).y + amount.y < edge_y or
                    amount.y > 0.0F and transform::position(m_tile_set_transform).y + sprite::offset(i).y + amount.y > edge_y + view().h - 32.0F) {
                    move_tile_set(-amount);
                }
            }
        }
        //transform::get(m_tile_set_transform)->position += amount;

        for (auto& i : m_selection_on_tile_set_sprites) {
            sprite::offset_add(i, amount);
        }

        cVec2F offset = sprite::offset(m_selection_on_tile_set_sprites.front());

        sprite::rect_x(m_current_tile_sprite, sprite::rect(m_current_tile_sprite).x + amount.x);
        sprite::rect_y(m_current_tile_sprite, sprite::rect(m_current_tile_sprite).y + amount.y);

        cVec2F tile_offset = offset / 16.0F;
        m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 and m_selection_on_tile_set_sprites.size() == 1) {
                if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                    m_text_bar.set_text(m_types.at(entity::Info{ 255, m_tile_number }));
                }
                else {
                    m_text_bar.clear_text();
                }
            }
        }
        return true;
    }
    bool Edit::deselect_all_on_tile_set() {        
        if (m_selection_on_tile_set_sprites.empty()) return false;
        console::log("sheet::Edit::deselect_all_on_tile_set\n");
        for (auto& i : m_selection_on_tile_set_sprites) {
            sprite::erase(i);
        }
        m_selection_on_tile_set_sprites.clear();
        return true;
    }
}