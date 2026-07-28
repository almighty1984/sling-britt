export module collision_grid.cell_grid;
import aabb;
import camera;
import console;
import camera;
import line;
import window;
import types;
import std;

import collision_grid.cell;

export namespace collision_grid {
    class CellGrid {
        U8    m_layer = NUM_VISIBLE_LAYERS - 1;
        Color m_color = { 0, 0, 0 };
        U8    m_size  = 4;

        std::vector<std::unique_ptr<Cell>> m_cells;

        Vec2F m_offset    = {  0.0F,  0.0F };        
        Vec2I m_num_cells = {     4,     4 };

        I32 m_up_line    = -1,
            m_down_line  = -1,
            m_left_line  = -1,
            m_right_line = -1;

        bool m_is_hidden = true;
    public:
        void init(cVec2F offset) {
            m_offset = offset;
            cVec2F num_cells = { (F32)m_num_cells.x, (F32)m_num_cells.y };
            cVec2F cell_size = { 256.0F / num_cells.x, 256.0F / num_cells.y };

            for (I32 cell_y = 0; cell_y < m_num_cells.y; ++cell_y) {
                for (I32 cell_x = 0; cell_x < m_num_cells.x; ++cell_x) {
                    m_cells.emplace_back(std::make_unique<Cell>(
                        m_offset + cell_size * Vec2F{ (F32)cell_x , (F32)cell_y }, cell_size));
                }
            }

            cVec2F extent = cell_size * num_cells;
            m_up_line     = line::make({ m_offset.x, m_offset.y }           , { m_offset.x + extent.x, m_offset.y });
            m_down_line   = line::make({ m_offset.x, m_offset.y + extent.y }, { m_offset.x + extent.x, m_offset.y + extent.y });
            m_left_line   = line::make({ m_offset.x, m_offset.y }           , { m_offset.x, m_offset.y + extent.y });
            m_right_line  = line::make({ m_offset.x + extent.x, m_offset.y }, { m_offset.x + extent.x, m_offset.y + extent.y });

            line::layer(m_up_line, m_layer);
            line::layer(m_down_line, m_layer);
            line::layer(m_left_line, m_layer);
            line::layer(m_right_line, m_layer);

            line::size(m_up_line, m_size);
            line::size(m_down_line, m_size);
            line::size(m_left_line, m_size);
            line::size(m_right_line, m_size);

            line::color(m_up_line, m_color);
            line::color(m_down_line, m_color);
            line::color(m_left_line, m_color);
            line::color(m_right_line, m_color);

            line::is_hidden(m_up_line, m_is_hidden);
            line::is_hidden(m_down_line, m_is_hidden);
            line::is_hidden(m_left_line, m_is_hidden);
            line::is_hidden(m_right_line, m_is_hidden);
        }
        ~CellGrid() {
            line::erase(m_up_line);
            line::erase(m_down_line);
            line::erase(m_left_line);
            line::erase(m_right_line);
        }
        I32 id_at(cVec2F position) {
            for (auto& cell : m_cells) {
                if (cell->is_within(position + camera::position)) {
                    return cell->id();
                }
            }
            return -1;
        }
        bool is_within(cI32 aabb) {
            for (auto& cell : m_cells) {
                if (cell and cell->is_some_point_within(aabb)) {
                    return true;
                }
            }
            return false;
        }
        void insert(cI32 aabb) {
            for (auto& cell : m_cells) {
                if (cell) cell->insert(aabb);                
            }            
        }
        void clear_aabbs() {
            for (auto& cell : m_cells) {
                if (cell) cell->clear_aabbs();                
            }
        }
        void reset() {
            for (auto& cell : m_cells) {
                if (cell) cell.reset();
            }
        }
        bool is_hidden() const { return m_is_hidden; }
        void is_hidden(bool q) {
            if (m_is_hidden == q) return;
            console::log("CellGrid is hidden: ", m_is_hidden, "\n");
            m_is_hidden = q;
            for (auto& cell : m_cells) {
                if (cell) {
                    cell->is_hidden(q);
                }
            }
            line::is_hidden(m_up_line, q);
            line::is_hidden(m_down_line, q);
            line::is_hidden(m_left_line, q);
            line::is_hidden(m_right_line, q);
        }
        void draw(std::unique_ptr<Window>& window, cU8 layer) {
            if (m_layer != layer or m_is_hidden) return;
            for (auto& i : m_cells) {
                if (i) i->draw(window);
            }
            line::draw(window, m_up_line);
            line::draw(window, m_down_line);
            line::draw(window, m_left_line);
            line::draw(window, m_right_line);
        }
        void update() {
            for (auto& i : m_cells) {
                if (i) {
                    i->check_collision();
                    i->update();
                }
            }
            line::offset(m_up_line,    -camera::position);
            line::offset(m_down_line,  -camera::position);
            line::offset(m_left_line,  -camera::position);
            line::offset(m_right_line, -camera::position);
        }
    };
}