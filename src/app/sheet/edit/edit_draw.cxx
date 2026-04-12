module sheet.edit;

namespace sheet {
    void Edit::draw(std::unique_ptr<Window>& window, cU8 layer) {
        /*for (auto& i : sprite::ids_in_layer(layer)) {
            sprite::draw(window, i);
        }
        return;*/

        if (sprite::layer(m_menu_down_bg_sprite) == layer)             sprite::draw(window, m_menu_down_bg_sprite);
        if (sprite::layer(m_menu_right_bg_sprite) == layer)            sprite::draw(window, m_menu_right_bg_sprite);
        if (sprite::layer(m_position_on_grid_map_sprite) == layer)     sprite::draw(window, m_position_on_grid_map_sprite);
        if (sprite::layer(m_save_sprite) == layer)                     sprite::draw(window, m_save_sprite);
        if (sprite::layer(m_tile_set_bg_sprite) == layer)              sprite::draw(window, m_tile_set_bg_sprite);
        if (sprite::layer(m_tile_set_sprite) == layer)                 sprite::draw(window, m_tile_set_sprite);
        if (sprite::layer(m_current_tile_sprite) == layer)             sprite::draw(window, m_current_tile_sprite);
        if (sprite::layer(m_grid_icon_sprite) == layer)                sprite::draw(window, m_grid_icon_sprite);
        if (sprite::layer(m_active_layer_sprite) == layer)             sprite::draw(window, m_active_layer_sprite);
        if (sprite::layer(m_text_bar_bg_sprite) == layer)              sprite::draw(window, m_text_bar_bg_sprite);
        if (sprite::layer(m_text_current_tile_set_bg_sprite) == layer) sprite::draw(window, m_text_current_tile_set_bg_sprite);

        for (auto& i : m_is_hidden_layer_sprites)       if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_mouse_sprites)                 if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_selection_on_tile_set_sprites) if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_selection_on_level_sprites)    if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_moving_sprites)                if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_grid_sprites)                  if (sprite::layer(i) == layer) sprite::draw(window, i);
        for (auto& i : m_grid_map_sprites)              if (sprite::layer(i) == layer) sprite::draw(window, i);

        if (!m_is_hidden_menu_up) {
            for (auto& i : m_menu_up_bar_bg_sprites) {
                if (sprite::layer(i) == layer) sprite::draw(window, i);
            }
            for (auto& text_label : m_menu_up_bar) {
                if (text_label and text_label->layer() == layer) {
                    text_label->draw(window);
                }
            }
        }
        if (m_text_current_tile_set.layer() == layer) {
            m_text_current_tile_set.draw(window);
        }
        if (m_text_bar.layer() == layer) {
            m_text_bar.draw(window);
        }
        if (m_info_message.layer() == layer) {
            m_info_message.draw(window);
        }

        for (auto& i : m_level_sprites) {
            if (sprite::layer(i) == layer) sprite::draw(window, i);
        }

        for (auto& i : m_menu_up_lists) {
            i.second.draw(window);
        }
    }
}