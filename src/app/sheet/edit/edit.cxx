module sheet.edit;
import console;
import transform;
import sprite;
import types;

namespace sheet {
    Edit::Edit(cU16 window_w, cU16 window_h, const std::filesystem::path& level_path) {
        console::log("sheet::Edit()\n");
        for (U8 i = 0; i < NUM_VISIBLE_LAYERS; ++i) {
            m_visible_layers.insert(i);
        }
        m_type = m_transition_to = m_transition_from = Type::edit;

        //load_types_from_text_file("edit.cfg");

        load_types_from_bin();

        m_input = input::make();
        m_level_transform = transform::make();

        console::log("sheet::Edit level transform id: ", m_level_transform, "\n");

        //m_tile_set_scroll_transform = transform::make();
        //transform::position(m_tile_set_scroll_transform, { 0.0F, 0.0F });
        //m_selection_on_tile_set_sprites.emplace_back(sprite::make("res/texture/tile_selection.png"));
        //sprite::get(m_selection_on_tile_set_sprites.back())->layer = SELECTION_ON_TILE_SET_LAYER;
        //sprite::get(m_selection_on_tile_set_sprites.back())->transform = m_tile_set_scroll_transform;

        m_tile_set_bg_transform = transform::make();
        transform::position(m_tile_set_bg_transform, { 0.0F, 0.0F });
        m_tile_set_bg_sprite = sprite::make("res/texture/editor/tile_set_bg.png");
        sprite::rect(m_tile_set_bg_sprite, { 0, 0, 512, 240 });
        sprite::layer(m_tile_set_bg_sprite, TILE_SET_BG_LAYER);
        sprite::is_hidden(m_tile_set_bg_sprite, true);
        sprite::transform(m_tile_set_bg_sprite, m_tile_set_bg_transform);

        m_tile_set_transform = transform::make();
        //transform::position(m_tile_set_transform, { 304.0F, 240.0F });
        m_tile_set_sprite = sprite::make(tile_set_texture_path(0));
        sprite::rect(m_tile_set_sprite, { 0, 0, 512, 512 });
        sprite::layer(m_tile_set_sprite, TILE_SET_LAYER);
        sprite::is_hidden(m_tile_set_sprite, true);
        sprite::transform(m_tile_set_sprite, m_tile_set_transform);

        m_menu_up_transform = transform::make();
        transform::position(m_menu_up_transform, { 0.0F, -8.0F });

        add_to_menu_up_bar(m_menu_up_labels[0]);
        add_to_menu_up_bar(m_menu_up_labels[1]);

        m_menu_up_lists[m_menu_up_labels[0]].transform = transform::make();        
        m_menu_up_lists[m_menu_up_labels[0]].bg_sprite = sprite::make("res/texture/editor/menu_bg.png");        

        console::log("sheet::Edit::Edit bg_h: ", m_menu_up_lists[m_menu_up_labels[0]].bg_h, "\n");

        sprite::layer(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite, MENU_BG_LAYER);
        sprite::transform(m_menu_up_lists[m_menu_up_labels[0]].bg_sprite, m_menu_up_lists[m_menu_up_labels[0]].transform);

        m_menu_up_lists[m_menu_up_labels[1]].transform = transform::make();
        m_menu_up_lists[m_menu_up_labels[1]].bg_sprite = sprite::make("res/texture/editor/menu_bg.png");        
        sprite::layer(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_menu_up_lists[m_menu_up_labels[1]].bg_sprite, m_menu_up_lists[m_menu_up_labels[1]].transform);

        load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
        load_menu_up_list(m_menu_up_labels[1], std::filesystem::current_path() / "res" / "prefab");                

        m_menu_down_transform = transform::make();
        transform::position(m_menu_down_transform, { 0.0F, 160.0F });
        m_menu_down_bg_sprite = sprite::make("res/texture/editor/menu_bg.png");
        sprite::offset(m_menu_down_bg_sprite, { 0.0F, 0.0F });
        sprite::rect(m_menu_down_bg_sprite, { 0, 0, 512, 32 });
        sprite::layer(m_menu_down_bg_sprite, MENU_BG_LAYER);
        sprite::transform(m_menu_down_bg_sprite, m_menu_down_transform);

        m_current_tile_sprite = sprite::make(tile_set_texture_path(0));
        sprite::rect(m_current_tile_sprite, { 0, 0, 16, 16 });
        sprite::layer(m_current_tile_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_current_tile_sprite, m_menu_down_transform);
        sprite::offset_x(m_current_tile_sprite, view().w - 32.0f);

        m_grid_icon_sprite = sprite::make("res/texture/editor/tile_grid.png");
        sprite::rect(m_grid_icon_sprite, { 0, 0, 16, 16 });
        sprite::layer(m_grid_icon_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_grid_icon_sprite, m_menu_down_transform);

        m_menu_right_transform = transform::make();
        transform::position(m_menu_right_transform, { 288.0F, 0.0F });
        m_menu_right_bg_sprite = sprite::make("res/texture/editor/menu_bg.png");
        sprite::offset(m_menu_right_bg_sprite, { 0, 0 });
        sprite::rect(m_menu_right_bg_sprite, { 0, 0, 32, 512 });
        sprite::layer(m_menu_right_bg_sprite, MENU_BG_LAYER);
        sprite::transform(m_menu_right_bg_sprite, m_menu_right_transform);

        m_save_sprite = sprite::make("res/texture/editor/save.png");
        sprite::offset(m_save_sprite, { 272.0F, 0.0F });
        sprite::layer(m_save_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_save_sprite, m_menu_down_transform);

        m_active_layer_sprite = sprite::make("res/texture/editor/layer_active.png");
        sprite::offset(m_active_layer_sprite, { 0.0F, 0.0F });
        sprite::rect(m_active_layer_sprite, { 0, 0, 16, 16 });
        sprite::layer(m_active_layer_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_active_layer_sprite, m_menu_right_transform);

        for (U8 i = 0; i < 10; ++i) {
            m_is_hidden_layer_sprites.emplace_back(sprite::make("res/texture/editor/layer_hidden.png"));
            sprite::offset(m_is_hidden_layer_sprites.back(), { 16.0F, i * 16.0F });
            sprite::rect(m_is_hidden_layer_sprites.back(), { 0, 0, 16, 16 });
            sprite::layer(m_is_hidden_layer_sprites.back(), MENU_TEXT_LAYER);
            sprite::transform(m_is_hidden_layer_sprites.back(), m_menu_right_transform);
        }


        m_text_bar_bg_sprite = sprite::make("res/texture/editor/text_bar_bg.png");
        sprite::offset(m_text_bar_bg_sprite, { 0.0F, 0.0F });
        sprite::rect(m_text_bar_bg_sprite, { 0, 0, 272, 16 });
        sprite::layer(m_text_bar_bg_sprite, MENU_TEXT_LAYER);
        sprite::transform(m_text_bar_bg_sprite, m_menu_down_transform);

        m_text_bar.transform(m_menu_down_transform);
        m_text_bar.offset({ 0.0F, 4.0F });
        m_text_bar.layer(MENU_TEXT_LAYER);
        m_text_bar.texture("res/texture/font/8_black.png");
        m_text_bar.is_hidden(false);
        //m_text_bar.clear_text();
        //m_text_bar.set_text(m_level_path.string());

        m_text_current_tile_set_bg_sprite = sprite::make("res/texture/editor/text_current_tile_set_bg.png");
        sprite::offset(m_text_current_tile_set_bg_sprite, { 0.0F, 0.0F });
        sprite::rect(m_text_current_tile_set_bg_sprite, { 0, 0, 32, 16 });
        sprite::layer(m_text_current_tile_set_bg_sprite, MENU_BG_LAYER);
        sprite::transform(m_text_current_tile_set_bg_sprite, m_menu_right_transform);
        sprite::is_hidden(m_text_current_tile_set_bg_sprite, true);

        m_text_current_tile_set.transform(m_menu_right_transform);
        m_text_current_tile_set.layer(MENU_TEXT_LAYER);
        m_text_current_tile_set.texture("res/texture/font/8_black.png");
        m_text_current_tile_set.is_hidden(true);
        m_text_current_tile_set.offset( {4.0F, 4.0F} );
        m_text_current_tile_set.set_text("0");

        m_info_message.transform(m_menu_up_transform);
        m_info_message.is_hidden(true);


        m_grid_transform = transform::make();
        m_grid_map_transform = transform::make();
        add_grid_at_offset({ 0.0F, 0.0F });


        m_position_on_grid_map_sprite = sprite::make("res/texture/tile_yellow.png");
        sprite::rect(m_position_on_grid_map_sprite, { 0, 0, 1, 1 });
        sprite::offset(m_position_on_grid_map_sprite, Vec2F{ 7.0F, 7.0F });
        sprite::transform(m_position_on_grid_map_sprite, m_grid_map_transform);
        sprite::layer(m_position_on_grid_map_sprite, GRID_LAYER + 1);

        sprite::is_hidden(m_position_on_grid_map_sprite, m_is_hidden_grid_map);

        m_mouse_transform = transform::make();
        m_mouse_sprites.emplace_back(sprite::make(m_mouse_texture_path));
        sprite::transform(m_mouse_sprites.back(), m_mouse_transform);
        sprite::layer(m_mouse_sprites.back(), MOUSE_LAYER);

        load_level_sprites(level_path);

        m_level_path = level_path;
    }
    Edit::~Edit() {
        console::log("sheet::Edit::~Edit()\n");
        input::erase(m_input);

        transform::erase(m_mouse_transform);
        transform::erase(m_level_transform);
        transform::erase(m_tile_set_transform);
        transform::erase(m_tile_set_bg_transform);
        transform::erase(m_grid_transform);
        transform::erase(m_grid_map_transform);
        transform::erase(m_menu_up_transform);
        transform::erase(m_menu_down_transform);
        transform::erase(m_menu_right_transform);

        sprite::erase(m_menu_down_bg_sprite);
        sprite::erase(m_menu_right_bg_sprite);
        sprite::erase(m_position_on_grid_map_sprite);
        sprite::erase(m_save_sprite);
        sprite::erase(m_tile_set_sprite);
        sprite::erase(m_tile_set_bg_sprite);
        sprite::erase(m_current_tile_sprite);
        sprite::erase(m_grid_icon_sprite);
        sprite::erase(m_active_layer_sprite);
        sprite::erase(m_text_bar_bg_sprite);
        sprite::erase(m_text_current_tile_set_bg_sprite);

        for (auto& i : m_is_hidden_layer_sprites)       sprite::erase(i);
        for (auto& i : m_mouse_sprites)                 sprite::erase(i);
        for (auto& i : m_selection_on_tile_set_sprites) sprite::erase(i);
        for (auto& i : m_selection_on_level_sprites)    sprite::erase(i);
        for (auto& i : m_moving_sprites)                sprite::erase(i);

        for (auto& i : m_level_sprites)                 sprite::erase(i);
        
        for (auto& i : m_grid_sprites)                  sprite::erase(i);
        for (auto& i : m_grid_map_sprites)              sprite::erase(i);

        for (auto& i : m_menu_up_bar_bg_sprites)        sprite::erase(i);
                
        m_menu_up_lists.clear();

        m_types.clear();
    }
    /*void Edit::clear_mouse_sprites() {
        console::log("sheet::Edit::clear_mouse_sprites\n");
        for (auto& i : m_mouse_sprites) {
            sprite::erase(i);
        }
        m_mouse_sprites.clear();        
    }*/     
}