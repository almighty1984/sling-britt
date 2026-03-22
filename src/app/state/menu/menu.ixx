module;
#include <memory>
export module state.menu;
import state;
import types;
import circle;

export namespace state {
    class Menu : public Object {
    private:
        circle::Object m_circle;
        
        I32 m_transform_id = -1;
        
        I32 m_line = -1;
        I32 m_normal = -1;
        I32 m_proj_on_normal = -1;

    public:
        Menu() = delete;
        Menu(cU16 window_w, cU16 window_h);
        ~Menu();
        
        void update(cF32 ts) override;
        void draw(std::unique_ptr<Window>& window, cU8 layer) override;
    };
}