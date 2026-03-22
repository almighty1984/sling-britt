module;
#include <filesystem>
#include <sstream>
#include <string>

module state.edit;

import console;
import input;
import transform;
import sprite;
import types;

namespace state {
    bool Edit::init_brush(U8 size) {
        if (size > 5) size = 1;

        if (m_mouse_sprite_ids.size() == size) return false;
        
        Vec2F start, end;
        switch (size) {
        case 1: start = {  0.0F,  0.0F }, end = { 16.0F, 16.0F }; break;
        case 2: start = {  0.0F,  0.0F }, end = { 32.0F, 32.0F }; break;
        case 3: start = {-16.0F,-16.0F }, end = { 32.0F, 32.0F }; break;
        case 4: start = {-16.0F,-16.0F }, end = { 48.0F, 48.0F }; break;
        case 5: start = {-32.0F,-32.0F }, end = { 48.0F, 48.0F }; break; }

        for (const auto& i : m_mouse_sprite_ids) {
            sprite::erase(i);
        }
        m_mouse_sprite_ids.clear();

        console::log("state::Edit::init_brush sprite_id: ");
        for (F32 x = start.x; x < end.x; x += 16.0F) {
            for (F32 y = start.y; y < end.y; y += 16.0F) {
                cI32 sprite_id = sprite::make(m_mouse_texture_path);
                //sprite::id(sprite_id, sprite_id);
                sprite::transform_id(sprite_id, m_mouse_transform_id);
                sprite::layer(sprite_id, 13);
                sprite::offset(sprite_id, { x,y });
                
                m_mouse_sprite_ids.push_back(sprite_id);
                console::log(sprite_id, " ");
            }
        }
        console::log("\n");
        return true;
    }
    void Edit::handle_menu_down() {
        cVec2F position = m_mouse_tile_position;
        input::Button button = input::Button::none;
        if (is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (is_pressed(input::Button::right)) {
            button = input::Button::right;
        } else if (is_pressed(input::Button::middle)) {
            button = input::Button::middle;
        }

        if (button == input::Button::none) return;        

        if (m_is_showing_tile_set) {
            if (m_tile_set == 255 &&
                position.x == transform::position(m_menu_down_transform_id).x + sprite::offset(m_save_sprite_id).x &&
                position.y == transform::position(m_menu_down_transform_id).y + sprite::offset(m_save_sprite_id).y) {
                if (button == input::Button::left) {
                    m_time_left_saving = m_time_to_save;
                    save_types_to_bin();
                } else if (button == input::Button::right) {
                    release(button);
                    if (!m_is_typing_text_bar) {
                        init_typing_text_bar();
                        m_is_typing_text_bar = true;
                    } else {
                        quit_typing_text_bar();
                        save_typed_text_bar();
                        m_is_typing_text_bar = false;
                    }
                } else if (button == input::Button::middle) {
                    load_types_from_bin();
                }
                return;
            }            
        } else if (!m_is_showing_tile_set) {
            if (position.x == transform::position(m_menu_down_transform_id).x + sprite::offset(m_grid_icon_sprite_id).x &&
                position.y == transform::position(m_menu_down_transform_id).y + sprite::offset(m_grid_icon_sprite_id).y) {

                release(button);                
                console::log("state::Edit::init_brush() position: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "  ", view().w, " ", view().h, "\n");

                if (button == input::Button::right) {
                    m_is_hidden_grid_map = !m_is_hidden_grid_map;
                    for (auto& i : m_grid_map_sprite_ids) {
                        sprite::is_hidden(i, m_is_hidden_grid_map);
                    }
                    sprite::is_hidden(m_position_on_grid_map_sprite_id, m_is_hidden_grid_map);
                } else if (button == input::Button::left) {
                    m_is_hidden_grid = !m_is_hidden_grid;
                    for (auto& i : m_grid_sprite_ids) {
                        sprite::is_hidden(i, m_is_hidden_grid);
                    }                    
                }
                return;
            }

            if (position.x == transform::position(m_menu_down_transform_id).x + sprite::offset(m_save_sprite_id).x &&
                position.y == transform::position(m_menu_down_transform_id).y + sprite::offset(m_save_sprite_id).y) {
                
                release(button);

                if (button == input::Button::left) {
                    console::log("state::Edit::handle_menu_down() save\n");
                    m_time_left_saving = m_time_to_save;
                    save_typed_text_bar();
                } else if (button == input::Button::right) {
                    if (!m_is_typing_text_bar) {
                        init_typing_text_bar();
                        m_is_typing_text_bar = true;
                    } else {
                        quit_typing_text_bar();
                    }
                }
            }
        }
    }    
    bool Edit::remove_level(const std::filesystem::path& path) {
        try {
            if (std::filesystem::remove(path)) {
                console::log("state::Edit::remove_level: ", path, " deleted\n");
            } else {
                console::error("state::Edit::remove_level: ", path, " not found\n");
            }
        } catch (const std::filesystem::filesystem_error& err) {
            console::error("state::Edit::remove_level filesystem error : ", err.what(), "\n");
            return false;
        }
        //load_menu_up_open_list(std::filesystem::current_path() / "res" / "level");
        return true;
    }   
}