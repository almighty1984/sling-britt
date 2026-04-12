module sheet.overlay;
import console;
import input;
import input.trait;
import types;

void sheet::Overlay::update_console_lines() {
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
            case console::Type::log:     m_console_lines.back()->texture("res/texture/font/5_gray.png");   break;
            case console::Type::warning: m_console_lines.back()->texture("res/texture/font/5_yellow.png"); break;
            case console::Type::error:   m_console_lines.back()->texture("res/texture/font/5_red.png");    break;
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

void sheet::Overlay::handle_console_input() {
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