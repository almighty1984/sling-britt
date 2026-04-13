export module sheet.overlay;
import sheet;
import console;
import bitmap_text;
import input;
import line;
import transform;
import sprite;
import sheet.game.save;
import std;

export namespace sheet {
    class Overlay : public Object {
        BitmapText m_fps_text,
                   m_console_input_text;

        I32 m_console_bg_sprite = -1,
            m_console_transform = -1;

        F32 m_console_up_y   = -512.0F,
            m_console_down_y = -336.0F;

        F32 m_console_y     = 0.0F,
            m_console_vel_y = 0.0F;

        U16 m_console_max_lines = 35;

        std::deque<std::unique_ptr<BitmapText>> m_console_lines;
        bool m_is_console_down = false;
        std::string m_prev_log;        

        U8 m_time_left_to_update_console = 5;




    public:
        Overlay(cU16 window_w, cU16 window_h) {
            m_type = m_transition_to = m_transition_from = Type::overlay;
            m_input = input::make();

            m_console_transform = transform::make();
            transform::position_y(m_console_transform, m_console_up_y);

            m_console_bg_sprite = sprite::make("res/texture/console_bg.png");
            sprite::transform(m_console_bg_sprite, m_console_transform);
            sprite::rect(m_console_bg_sprite, { 0, 0, 512, 512 });
            sprite::layer(m_console_bg_sprite, NUM_VISIBLE_LAYERS - 1);


            m_fps_text.layer(NUM_VISIBLE_LAYERS - 1);
            m_fps_text.texture("res/texture/font/5_white.png");
            m_fps_text.font_size(5);



            m_console_input_text.transform(m_console_transform);
            m_console_input_text.layer(NUM_VISIBLE_LAYERS - 1);
            m_console_input_text.texture("res/texture/font/5_white.png");
            m_console_input_text.font_size(5);
            m_console_input_text.set_text("_");

            m_visible_layers.insert(m_fps_text.layer());
        }
        ~Overlay() {
            console::log("~Overlay()\n");
            input::erase(m_input);

            sprite::erase(m_console_bg_sprite);
            transform::erase(m_console_transform);

            m_fps_text.clear_text();
            //m_console_text.clear_text();
        }

        void update_console_lines();
        void handle_console_input();

        void update(cF32 ts) override {
            if (is_pressed(input::Key::grave)) {
                release(input::Key::grave);
                if (is_pressed(input::Key::shift)) {
                    m_console_max_lines = 35;
                    m_console_down_y = -336.0F;
                }
                else {
                    m_console_max_lines = 19;
                    m_console_down_y = -416.0F;
                }
                if (!m_is_console_down) {
                    if (transform::position(m_console_transform).y <= m_console_down_y) {
                        m_is_console_down = true;
                        m_console_vel_y = 1.0F;
                    }
                }
                else if (m_is_console_down) {
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
            }
            else if (m_console_vel_y > 0.0F) {
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
            }
            else {
                --m_time_left_to_update_console;
            }



            cF32 current_fps = 1.0F / ts;
            m_fps_text.position({ (F32)(view().w - m_fps_text.string().size() * m_fps_text.font_size()),
                                  (F32)(view().h - m_fps_text.font_size()) });
            m_fps_text.set_text(std::to_string((int)current_fps));



            if (m_is_console_down) {
                handle_console_input();
                return;
            }


            if (is_pressed(input::Key::del)) {
                release(input::Key::del);

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
        void draw(std::unique_ptr<Window>& window, cU8 layer) override {
            //console::log("fps text layer: ", (int)m_fps_text.layer, " ", (int)layer, "\n");
            sprite::draw(window, m_console_bg_sprite);
            if (m_fps_text.layer() == layer) {
                m_fps_text.draw(window);
            }


            for (auto& i : m_console_lines) {
                if (i->layer() == layer) {
                    i->draw(window);
                }
            }
            if (m_console_input_text.layer() == layer) {
                m_console_input_text.draw(window);
            }
        }
    };
}