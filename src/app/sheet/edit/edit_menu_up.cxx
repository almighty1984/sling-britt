module sheet.edit;

namespace sheet {
    bool Edit::remove_level(const std::filesystem::path& path) {
        try {
            if (std::filesystem::remove(path)) {
                console::log("sheet::Edit::remove_level: ", path, " deleted\n");
            } else {
                console::error("sheet::Edit::remove_level: ", path, " not found\n");
            }
        } catch (const std::filesystem::filesystem_error& err) {
            console::error("sheet::Edit::remove_level filesystem error : ", err.what(), "\n");
            return false;
        }
        //load_menu_up_open_list(std::filesystem::current_path() / "res" / "level");
        return true;
    }
    void Edit::handle_menu_up() {
        size_t mouse_on_menu_bar_item = -1;

        for (auto& i : m_menu_up_bar_bg_sprites) {
            if (input::mouse.x >= sprite::offset(i.second).x and input::mouse.x < sprite::offset(i.second).x + sprite::rect(i.second).w and
                input::mouse.y >= sprite::offset(i.second).y and input::mouse.y < sprite::offset(i.second).y + sprite::rect(i.second).h) {
                mouse_on_menu_bar_item = (size_t)i.first;
                break;
            }
        }
        m_is_mouse_on_menu_up_list = false;
        bool is_mouse_on_menu_up_list[2] = { false };
        if (!is_pressed(input::Key::ctrl)) {
            if (input::mouse.y >= transform::position(m_menu_up_lists[menu::Label::open].transform).y - 2 and
                input::mouse.y <  transform::position(m_menu_up_lists[menu::Label::open].transform).y + m_menu_up_lists[menu::Label::open].bg_h and
                input::mouse.x >= transform::position(m_menu_up_lists[menu::Label::open].transform).x and
                input::mouse.x <  transform::position(m_menu_up_lists[menu::Label::open].transform).x + m_menu_up_lists[menu::Label::open].bg_w) {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[0] = true;
            } else if (input::mouse.y >= transform::position(m_menu_up_lists[menu::Label::import].transform).y - 2 and
                input::mouse.y <  transform::position(m_menu_up_lists[menu::Label::import].transform).y + m_menu_up_lists[menu::Label::import].bg_h and
                input::mouse.x >= transform::position(m_menu_up_lists[menu::Label::import].transform).x and
                input::mouse.x <  transform::position(m_menu_up_lists[menu::Label::import].transform).x + m_menu_up_lists[menu::Label::import].bg_w)

            {
                m_is_mouse_on_menu_up_list = true;
                is_mouse_on_menu_up_list[1] = true;
            }
        }

        for (auto& i : m_menu_up_bar_texts) {
            i.second->texture("res/texture/font/8_gray.png");
        }
        //console::log("m_is_mouse_on_menu_up_open_list: ", m_is_mouse_on_menu_up_open_list, "\n");
        if (mouse_on_menu_bar_item == 0 or is_mouse_on_menu_up_list[0]) {
            m_menu_up_bar_texts[menu::Label::open].get()->texture("res/texture/font/8_white.png");
            transform::position(m_menu_up_lists[menu::Label::open].transform, { 0.0F, 8.0F });

            cF32 level_list_entry_y = input::mouse.y - transform::position(m_menu_up_lists[menu::Label::open].transform).y;
            const size_t level_list_entry = (size_t)(level_list_entry_y - std::fmodf(level_list_entry_y, 8.0F)) / 8;
            if (level_list_entry < m_menu_up_lists[menu::Label::open].text_items.size()) {
                m_menu_up_bar_texts[menu::Label::open].get()->texture("res/texture/font/8_white.png");
                for (size_t i = 0; i < m_menu_up_lists[menu::Label::open].text_items.size(); ++i) {
                    if (i != level_list_entry) {
                        m_menu_up_lists[menu::Label::open].text_items.at(i).get()->texture("res/texture/font/8_gray.png");
                    }
                }
                m_menu_up_lists[menu::Label::open].text_items.at(level_list_entry).get()->texture("res/texture/font/8_white.png");

                if (is_pressed(input::Button::left)) {
                    release(input::Button::left);
                    console::log("sheet::Edit::update menu up clicked on level list ", level_list_entry, " ", m_menu_up_lists[menu::Label::open].text_items.at(level_list_entry).get()->string(), "\n");

                    m_level_path = m_menu_up_lists[menu::Label::open].text_items.at(level_list_entry).get()->string();

                    clear_grid_sprites();
                    clear_level_sprites();

                    load_level_sprites(m_level_path);

                    if (m_grid_sprites.empty()) {
                        add_grid_at_offset({ 0.0F, 0.0F });                            
                    }
                }
                if (is_pressed(input::Key::del)) {
                    release(input::Key::del);
                    m_info_message.offset({ m_menu_up_lists[menu::Label::open].text_items.at(level_list_entry).get()->offset().x + m_menu_up_lists[menu::Label::open].bg_w,
                                                m_menu_up_lists[menu::Label::open].text_items.at(level_list_entry).get()->offset().y + 8.0F });

                    m_info_message.is_hidden(false);
                    m_info_message.set_text("delete (Y/n)");

                    m_is_asked_to_remove_level = { true, level_list_entry };
                    return;
                }
            }
            transform::position(m_menu_up_lists[menu::Label::import].transform, { 0.0F, m_menu_up_lists[menu::Label::import].bg_h * -1.0F });
        }
        if (mouse_on_menu_bar_item == 1 or is_mouse_on_menu_up_list[1]) {
            m_menu_up_bar_texts[menu::Label::import].get()->texture("res/texture/font/8_white.png");
            transform::position(m_menu_up_lists[menu::Label::open].transform, { 0.0F, m_menu_up_lists[menu::Label::open].bg_h * -1.0F });
            transform::position(m_menu_up_lists[menu::Label::import].transform, { (F32)sprite::rect(m_menu_up_bar_bg_sprites[menu::Label::open]).w, 8.0F});

            cF32 list_entry_y = input::mouse.y - transform::position(m_menu_up_lists[menu::Label::import].transform).y;
            const size_t list_entry = (size_t)(list_entry_y - std::fmodf(list_entry_y, 8.0F)) / 8;

            if (list_entry < m_menu_up_lists[menu::Label::import].text_items.size()) {
                for (size_t i = 0; i < m_menu_up_lists[menu::Label::import].text_items.size(); ++i) {
                    if (i != list_entry) {
                        m_menu_up_lists[menu::Label::import].text_items.at(i).get()->texture("res/texture/font/8_gray.png");
                    }
                }
                m_menu_up_lists[menu::Label::import].text_items.at(list_entry).get()->texture("res/texture/font/8_white.png");

                if (is_pressed(input::Button::left)) {
                    release(input::Button::left);

                    m_prefab_path = m_menu_up_lists[menu::Label::import].text_items.at(list_entry)->string();

                    import_prefab_sprites(m_prefab_path);


                    console::log("sheet::Edit::handle_menu_up() clicked on import ", m_prefab_path, "\n");
                }
            }

            
        }
    }
    bool Edit::add_to_menu_up_bar(menu::Label label) {
        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();

        F32 offset_x = 0.0F;
        for (auto& i : m_menu_up_bar_bg_sprites) {
            offset_x += (F32)sprite::rect(i.second).w;
        }

        const std::string text = menu::to_string(label);

        bitmap_text->transform(m_menu_up_transform);
        bitmap_text->layer(MENU_TEXT_LAYER);
        bitmap_text->offset({ offset_x, 0.0F });
        bitmap_text->set_text(text);

        cU16 bg_w = (text.size() + 1) * (int)bitmap_text->font_size();
        cU16 bg_h = (int)bitmap_text->font_size();

        m_menu_up_w += bg_w;

        m_menu_up_bar_bg_sprites.emplace(label, sprite::make("res/texture/editor/menu_bg.png"));
        sprite::rect(m_menu_up_bar_bg_sprites.at(label), {0, 0, bg_w, bg_h});
        sprite::layer(m_menu_up_bar_bg_sprites.at(label), MENU_BG_LAYER);
        sprite::transform(m_menu_up_bar_bg_sprites.at(label), m_menu_up_transform);
        sprite::offset(m_menu_up_bar_bg_sprites.at(label), { offset_x, 0.0F });

        m_menu_up_bar_texts[label] = std::move(bitmap_text);
        return true;
    }

    void Edit::load_menu_up_list(menu::Label menu, const std::filesystem::path& dir_path) {
        m_menu_up_lists[menu].text_items.clear();
        for (auto const& dir_entry : std::filesystem::directory_iterator{ dir_path }) {            
            add_to_menu_up_list(menu, dir_entry);
        }
        transform::position(m_menu_up_lists[menu].transform, { 0.0F, m_menu_up_lists[menu].bg_h * -1.0F });
    }
    bool Edit::add_to_menu_up_list(menu::Label menu, const std::filesystem::path& path) {
        if (path.string().substr(path.string().size() - 3, 3) != "bin") {
            return false;
        }
        if (menu != menu::Label::open and menu != menu::Label::import) {
            console::log("sheet::Edit::add_to_menu_up_list() ", menu::to_string(menu), " not implemented\n");
            return false;
        }
        std::string path_str = path.string();
        std::string separator_str;
        separator_str = (char)std::filesystem::path::preferred_separator;

        const std::string folder_name = menu == menu::Label::import ? "prefab" : "level";        

        size_t res_path_pos = path_str.find("res" + separator_str, 0);
        if (res_path_pos == std::string::npos) {
            console::log("sheet::Edit::add_to_menu_up_list() res" + separator_str + " not found!\n");
            return false;
        }        
        path_str.erase(0, res_path_pos);    // keep part after "res/level/"

        std::unique_ptr<BitmapText> bitmap_text = std::make_unique<BitmapText>();
        bitmap_text->transform(m_menu_up_lists[menu].transform);
        bitmap_text->layer(MENU_TEXT_LAYER);
        bitmap_text->offset({ 0.0F, m_menu_up_lists[menu].text_items.size() * (F32)bitmap_text->font_size()});
        bitmap_text->set_text(path_str);

        cU16 bg_w = path_str.size() * (int)bitmap_text->font_size();
        if (bg_w > m_menu_up_lists[menu].bg_w) {
            m_menu_up_lists[menu].bg_w = bg_w;
        }
        m_menu_up_lists[menu].bg_h = (int)(bitmap_text->font_size() * (m_menu_up_lists[menu].text_items.size() + 1));
        sprite::rect(m_menu_up_lists[menu].bg_sprite, { 0, 0, m_menu_up_lists[menu].bg_w, m_menu_up_lists[menu].bg_h });

        m_menu_up_lists[menu].text_items.emplace_back(std::move(bitmap_text));
        return true;
    }
}