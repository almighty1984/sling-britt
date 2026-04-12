export module sheet.menu;
import sheet;
import types;
import circle;
import bitmap_text;
import std;

U8 s_selection = 0;

export namespace sheet {    
    class MenuStart : public Object {
    private:
        circle::Object m_circle;

        I32 m_transform = -1;

        I32 m_line = -1,
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