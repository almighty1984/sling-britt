module sheet.edit;
import console;
import input;
import sprite;
import types;

namespace sheet {
    void Edit::handle_menu_right() {
        //console::log("handle_menu_right\n");
        cVec2F position = m_mouse_tile_position;
        input::Button button = input::Button::none;
        if (is_pressed(input::Button::left)) {
            button = input::Button::left;
        } else if (is_pressed(input::Button::right)) {
            button = input::Button::right;
        }

        if (button == input::Button::none) return;
        if (position.y > 144.0F) return;

        if (!m_is_showing_tile_set) {
            release(button);
            //console::log("sheet::Edit::handle_menu_right ", position.x, " ", position.y, "\n");
            if (position.x == transform::position(m_menu_right_transform).x) {
                sprite::offset_y(m_active_layer_sprite, position.y);
                m_layer = (U8)(position.y / 16.0F);
            } else if (position.x == transform::position(m_menu_right_transform).x + 16.0F) {
                U8 layer = (U8)(position.y / 16.0F);
                if (layer < m_is_hidden_layer_sprites.size() and m_is_hidden_layer_sprites.at(layer)) {
                    sprite::is_hidden(m_is_hidden_layer_sprites.at(layer), !sprite::is_hidden(m_is_hidden_layer_sprites.at(layer)));
                }
                if (sprite::is_hidden(m_is_hidden_layer_sprites.at(layer))) {
                    m_visible_layers.erase(layer);                    
                } else {
                    m_visible_layers.insert(layer);                    
                }
                //console::log("sheet::Edit::handle_menu_right layer: ", (int)layer, "\n");
            }
        } else if (m_is_showing_tile_set) {
            console::log("sheet::Edit::handle_menu_right position: ", position.x, " ", position.y, "\n");
            cVec2F bg_sprite_pos = transform::position(m_menu_right_transform) +
                                   sprite::offset(m_text_current_tile_set_bg_sprite);
            cRectI bg_sprite_rect = sprite::rect(m_text_current_tile_set_bg_sprite);

            if (position.x >= bg_sprite_pos.x and position.x <= bg_sprite_pos.x + bg_sprite_rect.w and
                position.y >= bg_sprite_pos.y and position.y <= bg_sprite_pos.y + bg_sprite_rect.h) {
                //console::log("sheet::Edit::handle_menu_down position: ", position.x, " ", position.y, "\n");
                console::log("sheet::Edit::handle_menu_down sprite pos: ", bg_sprite_pos.x, " ", bg_sprite_pos.y, "\n");

                release(button);
                switch (button) {
                case input::Button::left:  init_tile_set(m_tile_set + 1); break;
                case input::Button::right: init_tile_set(m_tile_set - 1); break; }                
            }

        }

    }
}