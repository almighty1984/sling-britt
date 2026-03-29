module;
#include <fstream>
#include <filesystem>
#include <string>
#include <set>

export module sheet;
import console;
import input.trait;
import types;
import window;

export namespace sheet {
    std::filesystem::path tile_set_texture_path(cU8 tile_set_number) { return "res/texture/set_" + std::to_string((int)tile_set_number) + ".png"; }

    class Object : public input::Trait {
    protected:
        Type m_sheet      = Type::none,
             m_prev_sheet = Type::none,
             m_next_sheet = Type::none;

        std::set<U8> m_visible_layers;

        start::Info m_start_info = { start::Type::center, 0 };

        RectU m_view = { 0, 0, 320, 240 };

        start::Type m_next_level = start::Type::center;

        bool m_is_drawing_debug     = false,
             m_is_drawing_quad_tree = false,
             m_is_to_change_view    = false,
             m_is_to_player_save    = false;
    public:
        bool is_drawing_debug()     const { return m_is_drawing_debug;     } void is_drawing_debug(bool q)     { m_is_drawing_debug     = q; }
        bool is_drawing_quad_tree() const { return m_is_drawing_quad_tree; } void is_drawing_quad_tree(bool q) { m_is_drawing_quad_tree = q; }
        bool is_to_change_view()    const { return m_is_to_change_view;    } void is_to_change_view(bool q)    { m_is_to_change_view    = q; }
        bool is_to_player_save()    const { return m_is_to_player_save;    } void is_to_player_save(bool q)    { m_is_to_player_save    = q; }

        virtual std::filesystem::path level_path_to_save() { return {}; }
        virtual ~Object() {
            //input::erase(m_input_id);
            m_visible_layers.clear();
        }
        virtual void update(cF32 ts) {}
        virtual void update_unlocked() {}

        std::set<U8>& get_visible_layers() { return m_visible_layers; }
        
        Type sheet()      const { return m_sheet;      } void sheet(cType t)      { m_sheet = m_next_sheet = t; }
        Type next_sheet() const { return m_next_sheet; } void next_sheet(cType t) { m_next_sheet           = t; }
        Type prev_sheet() const { return m_prev_sheet; } void prev_sheet(cType t) { m_prev_sheet           = t; }

        bool is_to_transition() const { return m_next_sheet != Type::none and m_next_sheet != m_sheet; }

        RectU view() const { return m_view; } void view(cRectU v) { m_view = v; }

        //start::cType next_level() const { return m_next_level; } void next_level(start::cType n) { m_next_level = n; }
        start::Info start_info() const { return m_start_info; } void start_info(start::cInfo i) { m_start_info = i; }

        //entity::Type next_level_start() const { return m_next_level_start; }  void next_level_start(entity::Type t) { m_next_level_start = t; }        


        virtual void draw(std::unique_ptr<Window>& window, cU8 layer) {}
    };
}