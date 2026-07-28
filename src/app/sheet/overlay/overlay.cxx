module sheet.overlay;
import console;
import input;
import input.trait;
import types;

namespace sheet {
    void Overlay::update_console_lines() {
        if (m_prev_log != console::get_log() and m_is_console_down) {
            m_prev_log = console::get_log();

            m_console_lines.clear();

            const auto console_lines = console::get_lines();

            F32 line_y = 0.0F;
            for (auto& line : console_lines) {
                m_console_lines.push_back(std::make_unique<BitmapText>(m_console_transform));
                m_console_lines.back()->font_size(5);
                m_console_lines.back()->layer(NUM_VISIBLE_LAYERS - 1);

                switch (line.first) {
                    case console::Type::log:
                        m_console_lines.back()->texture("res/texture/font/5_gray.png");
                        break;
                    case console::Type::warning:
                        m_console_lines.back()->texture("res/texture/font/5_yellow.png");
                        break;
                    case console::Type::error:   m_console_lines.back()->texture("res/texture/font/5_red.png");
                        break;
                }
            
                m_console_lines.back()->offset({ 0.0F, line_y + m_console_y - console_lines.size() * 5.0F - transform::position(m_console_transform).y });

                m_console_lines.back()->set_text(line.second);
                line_y += 5.0F;

                if (m_console_lines.size() > m_console_max_lines) {
                    //m_console_lines.front().reset();
                    m_console_lines.pop_front();
                }
            }

            m_console_input_text.offset(m_console_lines.back()->offset());
            //m_console_text.set_text(s);

            //m_console_text.offset(transform::position(m_console_transform) +
            //                      Vec2F{ 0.0F, sprite::rect(m_console_bg_sprite).h - console_lines.size() * 5.0F - 6.0F });
        }
    }

    void Overlay::handle_console_input() {
        for (I32 k = input::alphabet_begin(); k < input::alphabet_end(); ++k) {
            input::Key key = (input::Key)k;
            if (is_pressed(key)) {
                release(key);
                console::log("pressed ", input::to_string(key, is_pressed(input::Key::shift)), "\n");


                m_console_input_text.insert(m_console_input_text.count() - 1, input::to_string(key));

            }
        }
        //if (is_pressed(input::Key::del)) {
        //    release(input::Key::del);
    }

    void Overlay::update(cF32 ts) {
        if (is_pressed(input::Key::grave)) {
            release(input::Key::grave);
            if (is_pressed(input::Key::shift)) {
                m_console_max_lines = 35;
                m_console_down_y = -336.0F;
            } else {
                m_console_max_lines = 19;
                m_console_down_y = -416.0F;
            }
            if (!m_is_console_down) {
                if (transform::position(m_console_transform).y <= m_console_down_y) {
                    m_is_console_down = true;
                    m_console_vel_y = 1.0F;
                }
            } else if (m_is_console_down) {
                if (transform::position(m_console_transform).y >= m_console_up_y) {
                    m_is_console_down = false;
                    m_console_vel_y = -1.0F;
                    m_console_input_text.set_text("_");
                }
            }
        }
        if (m_console_vel_y < 0.0F) {
            cF32 upper_diff_y = m_console_up_y - transform::position(m_console_transform).y;
            cF32 diff_vel_y = upper_diff_y * 0.2F;
            m_console_vel_y = diff_vel_y;
        } else if (m_console_vel_y > 0.0F) {
            cF32 lower_diff_y = m_console_down_y - transform::position(m_console_transform).y;
            cF32 diff_vel_y = lower_diff_y * 0.2F;
            m_console_vel_y = diff_vel_y;
        }
        transform::velocity_y(m_console_transform, m_console_vel_y);



        if (m_time_left_to_update_console == 0) {
            m_time_left_to_update_console = 5;
            m_console_y = transform::position(m_console_transform).y - m_console_up_y;
            //console::log("console y: ", m_console_y, "\n");
            update_console_lines();

            sprite::is_hidden(m_console_input_text.get_sprites().back(), !sprite::is_hidden(m_console_input_text.get_sprites().back()));
        } else {
            --m_time_left_to_update_console;
        }



        cF32 current_fps = 1.0F / ts;
        m_fps_text.position({ (F32)(view().w - m_fps_text.string().size() * m_fps_text.font_size()),
                              (F32)(view().h - m_fps_text.font_size()) });
        m_fps_text.set_text(std::to_string((int)current_fps));

        //console::log(class_name(), "::update() fps: ", (int)current_fps, "\n");



        if (m_is_console_down) {
            handle_console_input();
            return;
        }


        if (is_pressed(input::Key::del)) {
            release(input::Key::del);
            console::log(class_name(), "::update() clear save data\n");
            sheet::game::clear_current_save_data();
            sheet::game::write_save(0);
        }

        if (is_pressed(input::Key::t)) {
            release(input::Key::t);

            transition(Type::menu_start, Type::game);
            //m_transition_from = Type::menu_start;
            //m_transition_to = Type::game;
            //m_is_to_transition = true;
        }

        //console::log("overlay from: ", sheet::to_string(m_transition_from), " to: ", sheet::to_string(m_transition_to), "\n");

    }
}