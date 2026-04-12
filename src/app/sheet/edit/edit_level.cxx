module sheet.edit;
import console;
import transform;
import sprite;
import types;

namespace sheet {
    void Edit::move_level(cVec2F amount) {
        transform::position_add(m_level_transform, amount);
        transform::position_add(m_grid_transform, amount);

        sprite::offset(m_position_on_grid_map_sprite, Vec2F{ 7.0F, 7.0F } - transform::position(m_grid_transform) / 16.0F);
    }
    void Edit::move_level_to(cVec2F position) {
        transform::position(m_level_transform, position);
        transform::position(m_grid_transform, position);
        
        sprite::offset(m_position_on_grid_map_sprite, Vec2F{ 7.0F, 7.0F } - transform::position(m_grid_transform) / 16.0F);
    }
    void Edit::move_selected_on_level(cVec2F amount) {
        for (auto& i : m_selection_on_level_sprites) {
            sprite::offset_add(i, amount);
            
            if (amount.x < 0.0F and sprite::offset(i).x + transform::position(m_level_transform).x < view().x or
                amount.x > 0.0F and sprite::offset(i).x + transform::position(m_level_transform).x > view().w - 48.0F) {
                transform::position_add(m_level_transform, { -amount.x, 0.0F });
                transform::position_add(m_grid_transform, { -amount.x, 0.0F });
            }
            if (amount.y < 0.0F and sprite::offset(i).y + transform::position(m_level_transform).y < view().y or
                amount.y > 0.0F and sprite::offset(i).y + transform::position(m_level_transform).y > view().h - 32.0F) {
                transform::position_add(m_level_transform, { 0.0F, -amount.y });
                transform::position_add(m_grid_transform, { 0.0F, -amount.y });
            }
        }
        for (auto& i : m_moving_sprites) {
            sprite::offset_add(i, amount);
        }
        console::log("sheet::Edit::move_selected_on_level view: ", view().w, " ", view().h, "\n");
    }
    void Edit::finish_moving_selected_on_level() {
        console::log("sheet::Edit::finish_moving_on_level layer: ", (int)m_layer, "\n");
                
        U16 undo_count = 0;
        for (auto& moving_sprite : m_moving_sprites) {            
            cVec2F offset       = sprite::offset(moving_sprite);
            cRectI rect  = sprite::rect(moving_sprite);
            cU8    tile_set     = sprite::tile_set(moving_sprite);
            sprite::erase(moving_sprite);

            for (size_t i = 0; i < sprite::size(); ++i) {                
                if (sprite::transform(i) == m_level_transform and
                    sprite::offset(i)       == offset               and
                    sprite::layer(i)        == m_layer) {
                    history::Act prev_act = erase_sprite_on_level(m_layer, offset);
                    if (prev_act != history::Act::none) {
                        m_undo_acts.push_back(prev_act);
                        ++undo_count;
                    }
                    break;
                }
            }
            history::Act undo_act = place_sprite_on_level(m_layer, tile_set, rect, offset);
            if (undo_act != history::Act::none) {
                m_undo_acts.push_back(undo_act);
                ++undo_count;
            }
        }        
        if (undo_count > 0) {
            m_undo_counts.push_back(undo_count);
        }
        m_moving_sprites.clear();
    }
    void Edit::deselect_all_on_level() {
        for (auto& i : m_selection_on_level_sprites) {
            sprite::erase(i);
        }
        m_selection_on_level_sprites.clear();
    } 
    
    history::Act Edit::place_sprite_on_level(cU8 layer, cU8 tile_set, cRectI rect, cVec2F offset) {        
        if (tile_set == 255 and m_selection_on_tile_set_sprites.size() == 1) {
            cVec2F tile_offset = { rect.x / 16.0F, rect.y / 16.0F };
            m_tile_number = std::fmodf(tile_offset.x, 32.0F) + tile_offset.y * 32.0F;

            if (m_types.find(entity::Info{ 255, m_tile_number }) != m_types.end()) {
                const std::string type_str = m_types.at(entity::Info{ 255, m_tile_number });
                console::log("tile number: ", (int)m_tile_number, " type: ", type_str, "\n");
            }
        }

        const std::filesystem::path texture_path = "res/texture/set_" + std::to_string((int)tile_set) + ".png";

        // Replace if found
        for (auto& i : m_level_sprites) {
            
            if (sprite::transform(i) == m_level_transform and
                sprite::layer(i)        == layer and
                sprite::offset(i)       == offset) {

                if (sprite::tile_set(i) == tile_set and
                    sprite::rect(i) == rect
                    ) {
                    return history::Act::none;
                } else {
                    m_undo_info_replaced.push_back({ sprite::transform(i),
                                                sprite::layer(i),
                                                sprite::tile_set(i),
                                                sprite::rect(i),
                                                sprite::offset(i) });

                    //console::log("sheet::Edit::place_sprite_on_level replacing, previous act size: ", m_undo_acts.size(), "\n");
                    sprite::rect(i, rect);
                    if (sprite::tile_set(i) != tile_set) {
                        sprite::tile_set(i, tile_set);
                        sprite::texture(i, texture_path);
                    }

                    return history::Act::replace;
                }
            }
        }        

        // Place new
        //console::log("sheet::Edit::place_sprite_on_level_at_offset placing, previous acts size: ", m_undo_acts.size(), "\n");
        cI32 sprite = sprite::make(tile_set_texture_path(m_tile_set));
        //sprite::id(sprite, sprite);
        sprite::transform(sprite, m_level_transform);
        sprite::layer(sprite, layer);
        sprite::tile_set(sprite, tile_set);
        sprite::rect(sprite, rect);
        sprite::offset(sprite, offset);
        sprite::texture(sprite, texture_path);

        m_level_sprites.emplace_back(sprite);

        m_undo_info_placed.push_back({ sprite::transform(sprite),
                                       sprite::layer(sprite),
                                       sprite::tile_set(sprite),
                                       sprite::rect(sprite),
                                       sprite::offset(sprite) });
        
        return history::Act::place;
    }
    history::Act Edit::erase_sprite_on_level(cU8 layer, cVec2F offset) {
        if (m_is_showing_tile_set) return history::Act::none;
        for (auto it = m_level_sprites.begin(); it != m_level_sprites.end(); ++it) {
            cI32 i = *it;

            if (sprite::transform(i) == m_level_transform and
                sprite::offset(i)       == offset               and
                sprite::layer(i)        == layer) {
                console::log("sheet::Edit::erase_sprite_on_level_at_offset ", i, " ", " id: ", i, " previous_acts.size: ", m_undo_acts.size(), "\n");
                
                m_undo_info_erased.push_back({ sprite::transform(i),
                                               sprite::layer(i),
                                               sprite::tile_set(i),
                                               sprite::rect(i),
                                               sprite::offset(i) });

                sprite::erase(i);

                m_level_sprites.erase(it);

                return history::Act::erase;
            }
        }
        return history::Act::none;
    }
    bool Edit::select_on_level(cVec2F offset) {        
        for (auto& sel_id : m_selection_on_level_sprites) {
            if (sprite::transform(sel_id) != m_level_transform) continue;
            if (sprite::offset(sel_id) == offset) {
                return false;
            }
        }
        console::log("sheet::Edit::select_on_level offset: ", offset.x, " ", offset.y, "\n");
        m_selection_on_level_sprites.push_back(sprite::make(m_mouse_texture_path));
        sprite::transform(m_selection_on_level_sprites.back(), m_level_transform);
        sprite::layer(m_selection_on_level_sprites.back(), SELECTION_ON_LEVEL_LAYER);
        sprite::offset(m_selection_on_level_sprites.back(), offset);
        return true;
    }
    bool Edit::deselect_on_level(cVec2F offset) {
        for (auto& sel_id : m_selection_on_level_sprites) {
            if (sprite::transform(sel_id) != m_level_transform) continue;
            if (sprite::offset(sel_id) == offset) {
                console::log("sheet::Edit::deselect_on_level aready selected, deselecting\n");
                sprite::erase(sel_id);
                std::vector<I32> resized_selection;
                for (auto& i : m_selection_on_level_sprites) {
                    if (i != sel_id) {
                        resized_selection.push_back(i);
                    }
                }
                m_selection_on_level_sprites.clear();
                m_selection_on_level_sprites = resized_selection;
                return true;
            }
        }
        return false;
    }
    std::vector<I32> Edit::find_sprites_in_selection_on_level() {
        std::vector<I32> found_sprites;
        for (auto& sel_sprite : m_selection_on_level_sprites) {

            for (auto& sprite : m_level_sprites) {
                if (sprite == sel_sprite or sprite == -1 or
                    std::find(m_selection_on_level_sprites.begin(),
                        m_selection_on_level_sprites.end(), sprite) != m_selection_on_level_sprites.end()) {
                    continue;
                }
                if (sprite::offset(sprite) == sprite::offset(sel_sprite) and
                    sprite::layer(sprite) == m_layer and
                    sprite::transform(sprite) == m_level_transform) {
                    found_sprites.push_back(sprite);
                }
            }
        }
        return found_sprites;
    }
    void Edit::copy_selected_start_moving_on_level() {
        console::log("sheet::Edit::init_copy_selection_on_level\n");
        std::vector<I32> found_sprites = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprites) {
            sprite::erase(i);
        }
        m_moving_sprites.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprites) {
            cI32 sprite = sprite::make(tile_set_texture_path(sprite::tile_set(i)));
            m_moving_sprites.emplace_back(sprite);
            *sprite::get(sprite) = *sprite::get(i);
            sprite::get(sprite)->id(sprite);
        }
    }

    void Edit::start_moving_selected_on_level() {
        std::vector<I32> found_sprites = find_sprites_in_selection_on_level();
        
        for (auto it = m_level_sprites.begin(); it != m_level_sprites.end(); ++it) {
            if (std::find(found_sprites.begin(), found_sprites.end(), (*it)) != found_sprites.end()) {
                m_level_sprites.erase(it);
            }
        }
        
        for (auto& i : m_moving_sprites) {
            sprite::erase(i);
        }
        m_moving_sprites.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprites) {
            cI32 sprite = sprite::make(tile_set_texture_path(sprite::tile_set(i)));
            m_moving_sprites.push_back(sprite);
            *sprite::get(sprite) = *sprite::get(i);
            sprite::get(sprite)->id(sprite);
            
            m_undo_acts.push_back(history::Act::move);
            m_undo_info_moved.push_back({ sprite::transform(sprite),
                                          sprite::layer(sprite),
                                          sprite::tile_set(sprite),
                                          sprite::rect(sprite),
                                          sprite::offset(sprite) });
            sprite::erase(i);            
            ++undo_count;            
        }
        m_undo_counts.push_back(undo_count);
    }
    void Edit::clear_selected_on_level() {
        std::vector<I32> found_sprites = find_sprites_in_selection_on_level();

        for (auto& i : m_moving_sprites) {
            sprite::erase(i);
        }
        m_moving_sprites.clear();
        U16 undo_count = 0;
        for (auto& i : found_sprites) {
            history::Act undo_act = erase_sprite_on_level(sprite::layer(i), sprite::offset(i));
            m_undo_acts.push_back(undo_act);
            ++undo_count;
        }
        m_undo_counts.push_back(undo_count);
    }
    bool Edit::eyedropper_on_level(cVec2F offset) {
        if (m_is_showing_tile_set) return false;

        cI32 id = level_sprite_at_offset(m_layer, offset);
        if (id == -1) {
            console::log("sheet::Edit::eyedropper_o_level({", offset.x, ", ", offset.y, "}) tile empty!\n");
            return false;
        }
        m_tile_set = sprite::tile_set(id);
        m_layer = sprite::layer(id);
        init_tile_set(m_tile_set);

        for (auto& i : m_selection_on_tile_set_sprites) {
            sprite::erase(i);
        }
        m_selection_on_tile_set_sprites.clear();
        m_selection_on_tile_set_sprites.emplace_back(sprite::make("res/texture/tile_selection.png"));
        sprite::layer(m_selection_on_tile_set_sprites.back(), SELECTION_ON_TILE_SET_LAYER);
        sprite::transform(m_selection_on_tile_set_sprites.back(), m_tile_set_transform);
        sprite::offset(m_selection_on_tile_set_sprites.back(), { (F32)sprite::rect(id).x, (F32)sprite::rect(id).y });
        sprite::is_hidden(m_selection_on_tile_set_sprites.back(), true);

        sprite::rect(m_current_tile_sprite, { sprite::rect(id).x, sprite::rect(id).y, 16, 16});

        Vec2F tile_offset = sprite::offset(m_selection_on_tile_set_sprites.back());

        if (tile_offset.x > 224.0F) tile_offset.x = 224.0F;        
        if (tile_offset.y > 352.0F) tile_offset.y = 352.0F;

        console::log("sheet::Edit::eyedropper_on_level() tile_offset: ", tile_offset.x, " ", tile_offset.y, "\n");
        transform::position(m_tile_set_transform, -tile_offset);
        return true;
    }
    cI32 Edit::level_sprite_at_offset(cU8 layer, cVec2F offset) {
        for (auto& i : sprite::ids_in_layer(layer)) {
            if (sprite::offset(i) == offset) {
                return i;
            }
        }
        return -1;
    }
    void Edit::clear_level_sprites() {
        for (auto& i : m_level_sprites) {
            sprite::erase(i);
        }
        m_level_sprites.clear();        
    }
    void Edit::undo_last_act() {
        if (m_undo_acts.empty() or m_undo_counts.empty()) return;
        console::log("sheet::Edit::undo_last_act count: ", m_undo_counts.back(), "\n");

        for (U16 undo_count = 0; undo_count < m_undo_counts.back(); ++undo_count) {
            if (m_undo_acts.empty()) break;
            console::log("Edit::undo_last_act: ", history::to_string(m_undo_acts.back()), " ", m_undo_acts.size(), "\n");
            if (m_undo_acts.back() == history::Act::place and !m_undo_info_placed.empty()) {
                console::log("sheet::Edit::undo_last_act undo place\n");

                for (size_t i = 0; i < sprite::size(); ++i) {
                    if (sprite::transform(i) == m_level_transform        and
                        sprite::offset(i)       == m_undo_info_placed.back().offset and
                        sprite::layer(i)        == m_undo_info_placed.back().layer) {
                        sprite::erase(i);

                        for (auto it = m_level_sprites.begin(); it != m_level_sprites.end(); ++it) {
                            if ((*it) == i) {
                                m_level_sprites.erase(it);
                            }
                        }
                        break;
                    }
                }

                m_redo_info_placed.push_back(m_undo_info_placed.back());
                m_undo_info_placed.pop_back();
            } else if (m_undo_acts.back() == history::Act::replace and !m_undo_info_replaced.empty()) {
                cI32 id = level_sprite_at_offset(m_undo_info_replaced.back().layer, m_undo_info_replaced.back().offset);
                
                console::log("sheet::Edit::undo_last_act undo replace\n");
                                
                m_redo_info_replaced.push_back({ .transform = sprite::transform(id),
                                                 .layer        = sprite::layer(id),
                                                 .tile_set     = sprite::tile_set(id),
                                                 .rect  = sprite::rect(id),
                                                 .offset       = sprite::offset(id)        });

                sprite::transform(id, m_undo_info_replaced.back().transform);
                sprite::layer(id, m_undo_info_replaced.back().layer);
                sprite::tile_set(id, m_undo_info_replaced.back().tile_set);
                sprite::rect(id, m_undo_info_replaced.back().rect);
                sprite::offset(id, m_undo_info_replaced.back().offset);
                sprite::texture(id, tile_set_texture_path(sprite::tile_set(id)));

                m_undo_info_replaced.pop_back();
            } else if (m_undo_acts.back() == history::Act::erase and !m_undo_info_erased.empty()) {
                if (level_sprite_at_offset(m_undo_info_erased.back().layer, m_undo_info_erased.back().offset) == -1) {
                    console::log("sheet::Edit::undo_last_act undo erase\n");
                    cI32 id = sprite::make(tile_set_texture_path(m_undo_info_erased.back().tile_set));
                    //sprite::id(id, id);
                    sprite::transform(id, m_level_transform);
                    sprite::layer(id, m_undo_info_erased.back().layer);
                    sprite::tile_set(id, m_undo_info_erased.back().tile_set);
                    sprite::rect(id, m_undo_info_erased.back().rect);
                    sprite::offset(id, m_undo_info_erased.back().offset);
                    
                    m_level_sprites.emplace_back(id);
                    
                }

                m_redo_info_erased.push_back(m_undo_info_erased.back());
                m_undo_info_erased.pop_back();
            } else if (m_undo_acts.back() == history::Act::move and !m_undo_info_moved.empty()) {
                console::log("sheet::Edit::undo_last_act undo move\n");

                cI32 id = sprite::make(tile_set_texture_path(m_undo_info_moved.back().tile_set));
                //sprite::id(id, id);
                sprite::transform(id, m_level_transform);
                sprite::layer(id, m_undo_info_moved.back().layer);
                sprite::tile_set(id, m_undo_info_moved.back().tile_set);
                sprite::rect(id, m_undo_info_moved.back().rect);
                sprite::offset(id, m_undo_info_moved.back().offset);
                
                m_level_sprites.emplace_back(id);

                m_redo_info_moved.push_back(m_undo_info_moved.back());
                m_undo_info_moved.pop_back();
            }
            m_redo_acts.push_back(m_undo_acts.back());
            m_undo_acts.pop_back();            
        }


        m_redo_counts.push_back(m_undo_counts.back());
        m_undo_counts.pop_back();

        if (!m_undo_acts.empty()) {
            console::log("sheet::Edit::undo_last_act() next: ", history::to_string(m_undo_acts.back()), "\n");            
        } else {
            console::log("sheet::Edit::undo_last_act() next: none\n");
        }
        
    }
    void Edit::redo_last_act() {
        if (m_redo_acts.empty() or m_redo_counts.empty()) {
            console::log("sheet::Edit::redo_last_act() nothing to redo!\n");
            return;
        }
        console::log("sheet::Edit::redo_last_act: ", history::to_string(m_redo_acts.back()), " count: ", m_redo_counts.back(), "\n");

        for (U16 redo_count = 0; redo_count < m_redo_counts.back(); ++redo_count) {
            if (m_redo_acts.empty()) break;
            console::log("sheet::Edit::redo_last_act: ", history::to_string(m_redo_acts.back()), " ", m_redo_acts.size(), "\n");
            if (m_redo_acts.back() == history::Act::erase and !m_redo_info_erased.empty()) {
                console::log("sheet::Edit::redo_last_act redo place\n");

                for (size_t i = 0; i < sprite::size(); ++i) {
                    if (sprite::transform(i) == m_level_transform and
                        sprite::offset(i) == m_redo_info_erased.back().offset and
                        sprite::layer(i) == m_redo_info_erased.back().layer) {
                        sprite::erase(i);

                        for (auto it = m_level_sprites.begin(); it != m_level_sprites.end(); ++it) {
                            if ((*it) == i) {
                                m_level_sprites.erase(it);
                            }
                        }
                        break;
                    }
                }
                m_undo_info_erased.push_back(m_redo_info_erased.back());
                m_redo_info_erased.pop_back();
            } else if (m_redo_acts.back() == history::Act::place and !m_redo_info_placed.empty()) {
                if (level_sprite_at_offset(m_redo_info_placed.back().layer, m_redo_info_placed.back().offset) == -1) {
                    console::log("sheet::Edit::redo_last_act redo place\n");
                    cI32 id = sprite::make(tile_set_texture_path(m_redo_info_placed.back().tile_set));
                    //sprite::id(id, id);
                    sprite::transform(id, m_level_transform);
                    sprite::layer(id, m_redo_info_placed.back().layer);
                    sprite::tile_set(id, m_redo_info_placed.back().tile_set);
                    sprite::rect(id, m_redo_info_placed.back().rect);
                    sprite::offset(id, m_redo_info_placed.back().offset);

                    m_level_sprites.emplace_back(id);
                }                

                m_undo_info_placed.push_back(m_redo_info_placed.back());
                m_redo_info_placed.pop_back();
            } else if (m_redo_acts.back() == history::Act::replace and !m_redo_info_replaced.empty()) {
                cI32 id = level_sprite_at_offset(m_redo_info_replaced.back().layer, m_redo_info_replaced.back().offset);
                
                //m_undo_info_replaced.push_back(m_redo_info_replaced.back());
                m_undo_info_replaced.push_back({ .transform = sprite::transform(id),
                                                 .layer        = sprite::layer(id),
                                                 .tile_set     = sprite::tile_set(id),
                                                 .rect  = sprite::rect(id),
                                                 .offset       = sprite::offset(id) });

                console::log("sheet::Edit::redo_last_act redo replace\n");
                sprite::transform(id, m_redo_info_replaced.back().transform);
                sprite::layer(id, m_redo_info_replaced.back().layer);
                sprite::tile_set(id, m_redo_info_replaced.back().tile_set);
                sprite::rect(id, m_redo_info_replaced.back().rect);
                sprite::offset(id, m_redo_info_replaced.back().offset);
                sprite::texture(id, tile_set_texture_path(sprite::tile_set(id)));

                m_redo_info_replaced.pop_back();
            } else if (m_redo_acts.back() == history::Act::move and !m_redo_info_moved.empty()) {
                console::log("sheet::Edit::redo_last_act redo move\n");
                
                for (size_t i = 0; i < sprite::size(); ++i) {
                    if (sprite::transform(i) == m_level_transform and
                        sprite::offset(i) == m_redo_info_moved.back().offset and
                        sprite::layer(i) == m_redo_info_moved.back().layer) {
                        sprite::erase(i);

                        for (auto it = m_level_sprites.begin(); it != m_level_sprites.end(); ++it) {
                            if ((*it) == i) {
                                m_level_sprites.erase(it);
                            }
                        }
                        break;
                    }
                }

                m_undo_info_moved.push_back(m_redo_info_moved.back());
                m_redo_info_moved.pop_back();
            }
            m_undo_acts.push_back(m_redo_acts.back());
            m_redo_acts.pop_back();
        }

        m_undo_counts.push_back(m_redo_counts.back());
        m_redo_counts.pop_back();

        if (!m_redo_acts.empty()) {
            console::log("sheet::Edit::redo_last_act() next: ", history::to_string(m_redo_acts.back()), "\n");            
        } else {
            console::log("sheet::Edit::redo_last_act() next: none\n");
        }
    }
    void Edit::load_level_sprites(const std::filesystem::path& path) {
        console::log("Edit::load_level_sprites ", path, "\n");

        m_undo_info_erased.clear();
        m_undo_info_placed.clear();
        m_undo_info_replaced.clear();
        m_undo_info_moved.clear();
        m_undo_acts.clear();

        auto sprite_data = sprite::load_level(path);

        for (auto& i : sprite_data) {

            //m_visible_layers.insert(i.layer);

            cI32 id = sprite::make(tile_set_texture_path(i.tile_set));
            m_level_sprites.emplace_back(id);

            //sprite::id(id, id);
            sprite::tile_set(id, i.tile_set);
            sprite::layer(id, i.layer);
            sprite::offset(id, Vec2F{ (F32)i.x, (F32)i.y });
            sprite::rect(id, Rect<I32>{ i.source_x, i.source_y, 16, 16 });
            sprite::transform(id, m_level_transform);


            //console::log("sheet::Edit::load_level_sprites sprite ", sprite, " offset: ", i.x, " ", i.y, "\n");                                   
            Vec2F grid_remainder = { std::fmodf(sprite::offset(id).x, 256.0F), std::fmodf(sprite::offset(id).y, 256.0F) };
            Vec2F grid_pos = sprite::offset(id) - grid_remainder;
            add_grid_at_offset(grid_pos);
        }
        //console::log("\n\nsheet::Edit::load_level_sprites path: ", path.string(), "\n\n");
        m_text_bar.set_text(path.string());

        move_level_to({ 0.0F, 0.0F });
    }    
}