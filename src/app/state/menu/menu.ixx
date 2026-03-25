module;
#include <memory>
export module state.menu;
import state;
import types;
import circle;
import bitmap_text;


U8 s_selection = 0;

export namespace state {    
    class MenuStart : public Object {
    private:
        circle::Object m_circle;

        I32 m_transform_id = -1;

        I32 m_line_id = -1,
            m_normal_id = -1,
            m_proj_on_normal_id = -1;

        BitmapText m_edit_text,
                   m_game_text,
                   m_options_text;
    public:
        MenuStart() = delete;
        MenuStart(cU16 window_w, cU16 window_h);
        ~MenuStart();

        void update(cF32 ts) override;
        void draw(std::unique_ptr<Window>& window, cU8 layer) override;
    };

    class MenuOptions : public Object {
    private:            
        BitmapText m_options_text;
    public:
        MenuOptions() = delete;
        MenuOptions(cU16 window_w, cU16 window_h);
        ~MenuOptions();

        void update(cF32 ts) override;
        void draw(std::unique_ptr<Window>& window, cU8 layer) override;
    };    
}