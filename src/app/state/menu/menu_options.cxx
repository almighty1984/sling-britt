module sheet.menu;
import app.config;
import console;
import input;

namespace sheet {
    MenuOptions::MenuOptions(cU16 window_w, cU16 window_h) {
        m_sheet = m_next_sheet = Type::menu_options;
        m_input_id = input::make();

        m_options_text.set_text("OPTIONS");
        m_options_text.position({ view().w / 2.0F - 32.0F, view().h / 2.0F - 64.0F });
    }
    MenuOptions::~MenuOptions() {
        input::erase(m_input_id);
    }

    void MenuOptions::update(cF32 ts) {
        if (is_pressed(input::Key::esc)) {
            release(input::Key::esc);
            m_next_sheet = Type::menu_start;
        }        
    }
    void MenuOptions::draw(std::unique_ptr<Window>& window, cU8 layer) {        
        m_options_text.draw(window);

    }
}