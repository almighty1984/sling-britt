export module collision_grid;
import aabb;
import console;
import camera;
import line;
import window;
import types;
import std;

import collision_grid.cell_grid;

static Vec2F s_cell_size = { 64.0F, 64.0F };
static Vec2I s_num_cells = { 4, 4 };

static bool s_is_hidden = true;

static std::vector<collision_grid::CellGrid*> s_cell_grids;

export namespace collision_grid {
    void init(std::vector<Vec2I>& grid_coords) {
        if (grid_coords.empty()) return;
        for (auto& grid_coord : grid_coords) {
            console::log("grid coord: ", grid_coord.x, " ", grid_coord.y, "\n");

            s_cell_grids.push_back(new CellGrid);
            s_cell_grids.back()->init(Vec2F{ (grid_coord.x - 1) * s_cell_size.x * s_num_cells.x,
                                             (grid_coord.y - 1) * s_cell_size.y * s_num_cells.y });
        }        
    }        
    void insert(cI32 aabb) {        
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) cell_grid->insert(aabb);
        }        
    }
    void reset() {
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) {
                cell_grid->clear_aabbs();
                cell_grid->reset();
            }
            delete cell_grid;
            cell_grid = nullptr;
        }
        s_cell_grids.clear();
    }
    void clear_aabbs() {
        std::vector<std::thread> threads;
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) {
                threads.emplace_back([&]() { cell_grid->clear_aabbs(); });
            }
        }
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    bool is_hidden() { return s_is_hidden; }
    void is_hidden(bool q) {
        if (s_is_hidden == q) return;
        s_is_hidden = q;
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) {
                cell_grid->is_hidden(q);
            }
        }
    }
    void draw(std::unique_ptr<Window>& window, cU8 layer) {
        if (s_is_hidden) return;
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) {
                cell_grid->draw(window, layer);
            }
        }
    }
    void update() {
        std::vector<std::thread> threads;
        for (auto& cell_grid : s_cell_grids) {
            if (cell_grid) {
                threads.emplace_back([&]() {
                    for (I32 aabb : aabb::aabb_ids()) {
                        if (aabb::is_active(aabb)) {
                            cell_grid->insert(aabb);
                        }
                    }
                    cell_grid->update();
                    cell_grid->clear_aabbs();
                    });
            }
        }
        //console::log("collision_grid::check_collision() num threads: ", threads.size(), " ", s_cell_grids.size(), "\n");
        for (auto& thread : threads) {
            if (thread.joinable()) thread.join();            
        }
        
    }
}