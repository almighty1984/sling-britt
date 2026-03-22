module;
#include <cmath>
#include <filesystem>
#include <sstream>

module state.edit;

import console;
import input;
import sprite;
import transform;
import types;

namespace state {
    void Edit::update(cF32 ts) {
        if (m_time_left_saving > 0) {
            sprite::source_rect_x(m_save_sprite_id, 16);
            --m_time_left_saving;
            if (m_time_left_saving == 0) {
                sprite::source_rect_x(m_save_sprite_id, 0);
            }
            if (!m_is_showing_tile_set && m_time_left_saving == m_time_to_save - 2) {
                sprite::save_level(m_level_path, m_grid_sprite_ids);
            }
        }

        if (is_pressed(input::Key::f1) || is_pressed(input::Key::equal)) {
            release(input::Key::f1);
            release(input::Key::equal);

            m_start_info = { .type   = start::Type::center,
                             .number = 0 };
            m_next_state = Type::game;
            return;
        }

        /*if (m_timer < 1) {
            m_timer += 1;
            return;
        }
        m_timer = 0;*/

        sprite::offset_x(m_current_tile_sprite_id, view().w - 32.0F);
        sprite::offset_x(m_grid_icon_sprite_id, view().w - 16.0f);

        if (is_pressed(input::Key::grave)) {
            release(input::Key::grave);
            m_is_to_change_view = true;
            if (view() == RectF{ 0.0F, 0.0F, 480.0F, 270.0F }) {
                view({ 0.0F, 0.0F, 320.0F, 180.0F });
                //move_selection_on_tile_set({})
            } else {
                view({ 0.0F, 0.0F, 480.0F, 270.0F });
            }
            
            //if (m_is_showing_tile_set) {
            //    //transform::get(m_tile_set_transform_id)->position = { 0.0F, 0.0F };

            //} else {
            //    transform::get(m_tile_set_transform_id)->position.x = view().w - 32.0F;
            //    transform::get(m_tile_set_transform_id)->position.y = view().w == 480.0F ? 240.0F : 160.0F;
            //}
            //move_tile_set({ 0.0F, 0.0F });            
        }

        //console::log("view: ", view().w, " ", view().h, "\n");

        transform::position_x(m_menu_right_transform_id, view().w - sprite::source_rect(m_menu_right_bg_sprite_id).w);
        transform::position_y(m_menu_down_transform_id, view().h + 16.0F - std::fmodf(view().h, 16.0F) - sprite::source_rect(m_menu_down_bg_sprite_id).h);

        if (is_pressed(input::Key::enter)) {
            release(input::Key::enter);
            m_is_hidden_grid = !m_is_hidden_grid;
            for (auto& i : m_grid_sprite_ids) {
                sprite::is_hidden(i, m_is_hidden_grid);
            }
        }

        if (!m_is_moving) {
            if (is_pressed(input::Key::ctrl) && is_pressed(input::Key::z)) {
                release(input::Key::z);
                is_pressed(input::Key::shift) ? redo_last_act() : undo_last_act();
                return;
            }
            
        }
        if (m_is_typing_text_bar) {
            handle_typing_text_bar();
        }
        if (m_is_asked_to_remove_level.first) {
            if (is_pressed(input::Key::y)) {
                release(input::Key::y);
                m_info_message.is_hidden(true);
                remove_level(m_menu_up_lists[m_menu_up_labels[0]].text_items.at(m_is_asked_to_remove_level.second).get()->get_text());
                m_menu_up_lists[m_menu_up_labels[0]].text_items.at(m_is_asked_to_remove_level.second).reset();
                m_menu_up_lists[m_menu_up_labels[0]].text_items.clear();
                load_menu_up_list(m_menu_up_labels[0], std::filesystem::current_path() / "res" / "level");
                m_is_asked_to_remove_level = { false, 0 };
            } else if (is_pressed(input::Key::n)) {
                release(input::Key::n);
                m_info_message.is_hidden(true);
                m_is_asked_to_remove_level = { false, 0 };
            }
            return;
        }
        if (is_pressed(input::Key::ctrl)) {
            if (is_pressed(input::Key::s)) {
                release(input::Key::s);
                if (is_pressed(input::Key::shift)) {
                    init_typing_text_bar();
                    m_is_typing_text_bar = true;
                } else {
                    if (m_is_typing_text_bar) {                        
                        quit_typing_text_bar();
                        m_is_typing_text_bar = false;
                    }
                    save_typed_text_bar();
                }
                return;
            }
        }

        cVec2F mouse_remainder_16 = { std::fmodf(input::mouse.x, 16.0F), std::fmodf(input::mouse.y, 16.0F) };
        m_mouse_tile_position = input::mouse - mouse_remainder_16;
        transform::position(m_mouse_transform_id, m_mouse_tile_position);
        //console::log("mouse tile: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");

        if (!m_is_hidden_grid_map && is_pressed(input::Key::ctrl)) {
            for (auto& i : m_grid_map_sprite_ids) {
                if (m_mouse_tile_position != sprite::offset(i)) continue;
                if (is_pressed(input::Button::left)) {
                    //console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                    move_level_to({ m_mouse_tile_position.x * -16.0F, m_mouse_tile_position.y * -16.0F});
                    return;
                }
            }
            if (is_pressed(input::Key::g)) {
                release(input::Key::g);
                //console::log("offset: ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
                Vec2F grid_pos = m_mouse_tile_position * 16.0F;
                if (!add_grid_at_offset(grid_pos)) {
                    erase_grid_at_offset(grid_pos);
                }
                return;
            }
        }
        if (is_pressed(input::Key::g)) {
            release(input::Key::g);

            Vec2F mouse_offset = m_mouse_tile_position - transform::position(m_grid_transform_id);
            Vec2F grid_remainder = { std::fmodf(mouse_offset.x, 256.0F), std::fmodf(mouse_offset.y, 256.0F) };
            Vec2F grid_pos = mouse_offset - grid_remainder;
            //console::log("state::Edit::update grid_pos: ", grid_pos.x, " ", grid_pos.y, "\n");
            if (!add_grid_at_offset(grid_pos)) {
                erase_grid_at_offset(grid_pos);
            }
        }
        
        if (is_pressed(input::Key::num_0)) {
            release(input::Key::num_0);
            //console::log("state::Edit::update sprite::erase_null...");
            //sprite::erase_null(m_level_transform_id);
            return;
        }
        
        m_is_mouse_on_menu_down = !m_is_hidden_menu_down && (m_mouse_tile_position.x > view().w - 48.0F || m_mouse_tile_position.y > view().h - 32.0F) ? true : false;
        if (m_is_mouse_on_menu_down) {
            if (m_mouse_sprite_ids.size() != 1) {
                init_brush(1);
            }
        } else {
            if (m_mouse_sprite_ids.size() == 1) {
                m_is_showing_tile_set ? init_brush(m_tile_set_brush_size) : init_brush(m_level_brush_size);
            }
        }

        //console::log("state::Edit::update is_mouse_on_level_list: ", m_is_mouse_on_menu_up_open_list, "\n");


        if (is_pressed(input::Key::m)) {
            release(input::Key::m);
            m_is_hidden_grid_map = !m_is_hidden_grid_map;
            for (auto& i : m_grid_map_sprite_ids) {
                sprite::is_hidden(i, !m_is_hidden_grid_map);
            }
            sprite::is_hidden(m_position_on_grid_map_sprite_id, !m_is_hidden_grid_map);
        }

        if (is_pressed(input::Key::num_1)) {
            release(input::Key::num_1);
            m_is_showing_tile_set ?  m_tile_set_brush_size = 1 : m_level_brush_size = 1;
            init_brush(1);
        }
        if (is_pressed(input::Key::num_2)) {
            release(input::Key::num_2);
            m_is_showing_tile_set ? m_tile_set_brush_size = 2 : m_level_brush_size = 2;
            init_brush(2);
        }
        if (is_pressed(input::Key::num_3)) {
            release(input::Key::num_3);
            m_is_showing_tile_set ? m_tile_set_brush_size = 3 : m_level_brush_size = 3;
            init_brush(3);
        }
        if (is_pressed(input::Key::num_4)) {
            release(input::Key::num_4);
            m_is_showing_tile_set ? m_tile_set_brush_size = 4 : m_level_brush_size = 4;
            init_brush(4);
        }
        if (is_pressed(input::Key::num_5)) {
            release(input::Key::num_5);
            m_is_showing_tile_set ? m_tile_set_brush_size = 5 : m_level_brush_size = 5;
            init_brush(5);
        }

        if (input::scroll < 0.0F) {
            input::scroll = 0.0F;
            if (m_is_showing_tile_set && m_tile_set_brush_size > 1) {
                m_tile_set_brush_size -= 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size > 1) {
                m_level_brush_size -= 1;
                init_brush(m_level_brush_size);
            }
        } else if (input::scroll > 0.0F) {
            input::scroll = 0.0F;
            if (m_is_showing_tile_set && m_tile_set_brush_size < 5) {
                m_tile_set_brush_size += 1;
                init_brush(m_tile_set_brush_size);
            } else if (!m_is_showing_tile_set && m_level_brush_size < 5) {
                m_level_brush_size += 1;
                init_brush(m_level_brush_size);
            }
        }        
        if (is_pressed(input::Key::space)) {
            release(input::Key::space);
            m_is_showing_tile_set = !m_is_showing_tile_set;
            if (m_is_showing_tile_set) {
                enter_tile_set();
            } else {
                exit_tile_set();
            }
            m_is_showing_tile_set ? init_brush(m_tile_set_brush_size) : init_brush(m_level_brush_size);
            //transform::get(m_tile_set_scroll_transform_id)->position = transform::get(m_tile_set_transform_id)->position;
        }
        if (is_pressed(input::Key::alt)) {
            release(input::Key::alt);
            m_is_hidden_menu_down  = !m_is_hidden_menu_down;
            m_is_hidden_menu_right = !m_is_hidden_menu_right;
        }

        m_is_mouse_on_menu_up_bar = !is_pressed(input::Key::ctrl) &&
                                    !m_is_showing_tile_set &&
                                    input::mouse.y > 0 && input::mouse.y <= 8 &&
                                    input::mouse.x <= m_menu_up_w ? true : false;

        if (m_is_mouse_on_menu_up_bar || m_is_mouse_on_menu_up_list) {
            transform::position(m_menu_up_transform_id, { 0.0F, 0.0F });
            handle_menu_up();
            for (auto& i : m_mouse_sprite_ids) {
                sprite::is_hidden(i, true);
            }
        } else {
            transform::position(m_menu_up_transform_id) = { 0.0F,-16.0F };
            transform::position(m_menu_up_lists[m_menu_up_labels[0]].transform_id, {0.0F, m_menu_up_lists[m_menu_up_labels[0]].bg_h * -1.0F});
            transform::position(m_menu_up_lists[m_menu_up_labels[1]].transform_id, { 0.0F, m_menu_up_lists[m_menu_up_labels[1]].bg_h * -1.0F });
            for (auto& i : m_mouse_sprite_ids) {
                sprite::is_hidden(i, false);
            }
        }

        //console::log("state::Edit::update tile_set position: ", transform::get(m_tile_set_transform_id)->position.x, " ", transform::get(m_tile_set_transform_id)->position.y, "\n");
        if (is_pressed(input::Key::a)) {
            if (!m_moving_sprite_ids.empty()) {
                release(input::Key::a);
                move_selected_on_level({ -16.0F, 0.0F });
                return;
            }
            if (is_pressed(input::Key::shift)) {
                release(input::Key::a);
                move_selection_on_tile_set({ -16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 16, 0 });
                } else {
                    move_level({ 16, 0 });                    
                }
            }
        }
        if (is_pressed(input::Key::d)) {
            if (!m_moving_sprite_ids.empty()) {
                release(input::Key::d);
                move_selected_on_level({ 16.0F, 0.0F });
                return;
            }
            if (is_pressed(input::Key::shift)) {
                release(input::Key::d);
                move_selection_on_tile_set({ 16, 0 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ -16, 0 });
                } else {
                    move_level({ -16, 0 });
                }
            }
        }
        if (is_pressed(input::Key::w)) {
            if (!m_moving_sprite_ids.empty()) {
                release(input::Key::w);
                move_selected_on_level({ 0.0F, -16.0F });
                return;
            }
            if (is_pressed(input::Key::shift)) {
                release(input::Key::w);
                move_selection_on_tile_set({ 0,-16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, 16 });                    
                } else {
                    move_level({ 0, 16 });                    
                }
            }
        }
        if (is_pressed(input::Key::s)) {
            if (!m_moving_sprite_ids.empty()) {
                release(input::Key::s);
                move_selected_on_level({ 0.0F, 16.0F });
                return;
            }
            if (is_pressed(input::Key::shift)) {
                release(input::Key::s);   
                move_selection_on_tile_set({ 0, 16 });
            } else {
                if (m_is_showing_tile_set) {
                    move_tile_set({ 0, -16 });
                } else {
                    move_level({ 0, -16 });
                }
            }
        }

        //console::log("tile set position: ", transform::get(m_tile_set_transform_id)->position.x, " ", transform::get(m_tile_set_transform_id)->position.y, "\n");

        if (is_pressed(input::Key::backspace)) {
            release(input::Key::backspace);
            //clear_grid_sprites();
            clear_level_sprites();
        }
        if (is_pressed(input::Key::tab)) {
            release(input::Key::tab);
            if (m_is_showing_tile_set) {
                m_tile_set == 255 ? init_tile_set(m_prev_tile_set) : init_tile_set(255);
            } else {
                if (is_pressed(input::Key::shift) && m_layer > 0) {
                    sprite::add_offset(m_active_layer_sprite_id, { 0.0F, -16.0F });
                    m_layer -= 1;
                } else if (!is_pressed(input::Key::shift) && m_layer < 9) {
                    sprite::add_offset(m_active_layer_sprite_id, { 0.0F,  16.0F });
                    m_layer += 1;
                }
            }
        }

        //console::log("state::Edit::update() m_layer: ", (int)m_layer, "\n");
        
        if (!m_is_hidden_menu_down &&
            m_mouse_tile_position.y >= transform::position(m_menu_down_transform_id).y &&
            m_mouse_tile_position.y <= transform::position(m_menu_down_transform_id).y + sprite::source_rect(m_menu_down_bg_sprite_id).h &&
            m_mouse_tile_position.x >= transform::position(m_menu_down_transform_id).x &&
            m_mouse_tile_position.x <= transform::position(m_menu_down_transform_id).x + sprite::source_rect(m_menu_down_bg_sprite_id).w
            ) {
            //console::log("state::Edit::handle_menu_down\n");
            handle_menu_down();
            return;
        }
        //console::log("is_showing_menu_right: ", m_is_hidden_menu_right, " ", m_mouse_tile_position.x, " ", m_mouse_tile_position.y, "\n");
        if (!m_is_hidden_menu_right &&
            m_mouse_tile_position.x >= transform::position(m_menu_right_transform_id).x &&            
            m_mouse_tile_position.x <= transform::position(m_menu_right_transform_id).x + sprite::source_rect(m_menu_right_bg_sprite_id).w &&
            m_mouse_tile_position.y >= transform::position(m_menu_right_transform_id).y &&
            m_mouse_tile_position.y <= transform::position(m_menu_right_transform_id).y + sprite::source_rect(m_menu_right_bg_sprite_id).h) {
            handle_menu_right();
            return;
        }
        
        bool is_placing_on_level = false;
        if (is_pressed(input::Button::left)) {
            if (m_is_showing_tile_set &&
                (m_is_hidden_menu_down || !(m_mouse_tile_position.x >= transform::position(m_menu_right_transform_id).x &&
                                            m_mouse_tile_position.x <= transform::position(m_menu_right_transform_id).x + sprite::source_rect(m_menu_right_bg_sprite_id).w &&
                                            m_mouse_tile_position.y >= transform::position(m_menu_down_transform_id).y)
                )
                ) {
                //console::log("state::Edit::update hello\n");
                if (!is_pressed(input::Key::ctrl)) {
                    deselect_all_on_tile_set();
                }
                for (auto& mouse_id : m_mouse_sprite_ids) {
                    cVec2F offset = transform::position(m_mouse_transform_id) -
                                    transform::position(m_tile_set_transform_id) +
                                    sprite::offset(mouse_id);
                    select_on_tile_set(offset);
                }
            } else {
                //release(input::Button::left);

                if (is_pressed(input::Key::ctrl)) {
                    for (auto& mouse_id : m_mouse_sprite_ids) {
                        cVec2F offset = transform::position(m_mouse_transform_id) -
                                        transform::position(m_level_transform_id) +
                                        sprite::offset(mouse_id);
                        select_on_level(offset);
                    }
                } else {
                    deselect_all_on_level();

                    is_placing_on_level = true;
                    for (auto& sel_id : m_selection_on_tile_set_sprite_ids) {
                        size_t first_sel_id = m_selection_on_tile_set_sprite_ids.front();
                        cRectI source_rect  = { (I16)sprite::offset(sel_id).x,
                                                (I16)sprite::offset(sel_id).y,
                                                16, 16 };
                        
                        U16 num_placed = 0;
                        for (auto& mouse_id : m_mouse_sprite_ids) {
                            cVec2F offset = sprite::offset(sel_id) + sprite::offset(mouse_id) -
                                            sprite::offset(first_sel_id) +
                                            transform::position(m_mouse_transform_id) -
                                            transform::position(m_level_transform_id);

                            history::Act undo_act = place_sprite_on_level(m_layer, m_tile_set, source_rect, offset);
                            if (undo_act != history::Act::none) {
                                m_undo_acts.emplace_back(undo_act);
                                ++num_placed;
                            }
                        }
                        m_last_undo_count += num_placed;
                    }
                }                
            }
        }
       
        if (is_pressed(input::Button::right)) {
            if (m_is_showing_tile_set && (m_is_hidden_menu_down || (m_mouse_tile_position.x < 288 && m_mouse_tile_position.y < 160))) {
                for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                    Vec2F offset = transform::position(m_mouse_transform_id) -
                                   transform::position(m_tile_set_transform_id) +
                                   sprite::offset(mouse_sprite_id);
                    deselect_on_tile_set(offset);
                }
            }
        }

        bool is_erasing_on_level = false;
        if (is_pressed(input::Button::right)) {
            is_erasing_on_level = true;
            U16 num_erased = 0;
            for (auto& mouse_sprite_id : m_mouse_sprite_ids) {
                Vec2F offset = transform::position(m_mouse_transform_id) -
                               transform::position(m_level_transform_id) +
                               sprite::offset(mouse_sprite_id);
                if (is_pressed(input::Key::ctrl)) {
                    deselect_on_level(offset);
                } else {
                    deselect_all_on_level();
                    history::Act undo_act = erase_sprite_on_level(m_layer, offset);
                    if (undo_act != history::Act::none) {
                        m_undo_acts.emplace_back(undo_act);
                        ++num_erased;
                    }
                }
            }
            m_last_undo_count += num_erased;
        }
        if (!is_placing_on_level && !is_erasing_on_level) {
            if (m_last_undo_count > 0) {
                m_undo_counts.emplace_back(m_last_undo_count);
                m_last_undo_count = 0;
            }
        }

        if (is_pressed(input::Button::middle)) {
            release(input::Button::middle);
            cVec2F offset = transform::position(m_mouse_transform_id) - transform::position(m_level_transform_id);
            eyedropper_on_level(offset);
            //m_is_showing_tile_set ? m_tile_set_brush_size = 1 : m_level_brush_size = 1;
            //init_brush(1);
        }

        //console::log("tile_set_transform : ", transform::position(m_tile_set_transform_id).x, " ", transform::position(m_tile_set_transform_id).y, "\n");
        //console::log("tile_set_scroll_transform :", transform::position(m_tile_set_scroll_transform_id).x, " ", transform::position(m_tile_set_scroll_transform_id).y, "\n");

        if (is_pressed(input::Key::h)) {
            release(input::Key::h);
            m_is_hidden_grid = !m_is_hidden_grid;
            for (auto& i : m_grid_sprite_ids) {
                sprite::is_hidden(i, m_is_hidden_grid);
            }
        }

        if (is_pressed(input::Key::del)) {
            release(input::Key::del);

            if (!m_selection_on_level_sprite_ids.empty()) {
                clear_selected_on_level();
                return;
            }
        }
        if (is_pressed(input::Key::c)) {
            release(input::Key::c);
            if (!m_selection_on_level_sprite_ids.empty()) {
                if (!m_is_moving) {
                    m_is_moving = true;
                    copy_selected_start_moving_on_level();
                } else {
                    finish_moving_selected_on_level();
                    copy_selected_start_moving_on_level();
                }
            }
        }
        if (is_pressed(input::Key::x)) {
            release(input::Key::x);
            if (!m_selection_on_level_sprite_ids.empty()) {
                m_is_moving ? finish_moving_selected_on_level() : start_moving_selected_on_level();
                m_is_moving = !m_is_moving;
            }
        }
        if (is_pressed(input::Key::esc)) {
            release(input::Key::esc);  
            if (m_is_typing_text_bar) {
                quit_typing_text_bar();
                return;
            }
            if (m_is_typing_text_bar) {
                quit_typing_text_bar();
                return;
            }
            if (m_is_moving) {
                m_is_moving = false;
                finish_moving_selected_on_level();
                if (!m_selection_on_level_sprite_ids.empty()) {
                    deselect_all_on_level();
                    return;
                }
            } else {
                if (!m_selection_on_level_sprite_ids.empty()) {
                    deselect_all_on_level();
                    return;
                }
                if (!m_selection_on_tile_set_sprite_ids.empty()) {
                    deselect_all_on_tile_set();
                    return;
                }
            }
        }
    }
}