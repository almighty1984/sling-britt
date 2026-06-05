export module collision_grid;
import aabb;
import console;
import camera;
import line;
import window;
import types;
import std;

static U8 s_layer = NUM_VISIBLE_LAYERS - 1;
static Color s_color = { 255, 255, 255 };
static U8 s_size = 1;

static U8 s_max_depth = 4;
static U8 s_max_objects = 4;
static U8 s_max_points = 16;

std::mutex cell_collision_mutex;

class Cell {
    bool m_is_leaf = true;

    Cell* m_child[4] = { nullptr };

    Vec2F m_position{},
          m_extent{};
    I32 m_up_line    = -1,
        m_down_line  = -1,
        m_left_line  = -1,
        m_right_line = -1;

    std::set<I32> m_aabbs;

    U16 m_num_points = 0;
public:
    I32 id = 0, depth = 0;

    ~Cell() {
        clear();
    }
    Cell(cVec2F position, cVec2F extent) {
        m_position = position;
        m_extent   = extent;
        m_up_line    = line::make({ position.x, position.y },
                                  { position.x + extent.x, position.y });
        m_down_line  = line::make({ position.x, position.y + extent.y },
                                  { position.x + extent.x, position.y + extent.y });
        m_left_line  = line::make({ position.x, position.y },
                                  { position.x, position.y + extent.y });
        m_right_line = line::make({ position.x + extent.x, position.y },
                                  { position.x + extent.x, position.y + extent.y });

        line::layer(m_up_line, s_layer);
        line::layer(m_down_line, s_layer);
        line::layer(m_left_line, s_layer);
        line::layer(m_right_line, s_layer);

        line::size(m_up_line, s_size);
        line::size(m_down_line, s_size);
        line::size(m_left_line, s_size);
        line::size(m_right_line, s_size);

        line::color(m_up_line, s_color);
        line::color(m_down_line, s_color);
        line::color(m_left_line, s_color);
        line::color(m_right_line, s_color);
    }

    bool is_within(cVec2F point) {
        return point.x >= m_position.x and point.x <= m_position.x + m_extent.x and
               point.y >= m_position.y and point.y <= m_position.y + m_extent.y;
    }
    bool insert(cVec2F point, cI32 aabb) {
        if (!is_within(point)) {
            return false;
        }
        if (m_is_leaf) {
            m_aabbs.insert(aabb);
            m_num_points += 4;
            //if (m_aabbs.size() >= s_max_objects and depth < s_max_depth) {
            if (m_num_points >= s_max_points and depth < s_max_depth) {
                split();
            }
        } else {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i]->insert(point, aabb)) {
                    return true;
                }
            }
        }
        return false;
    }
    void split() {
        m_is_leaf = false;
        cF32 half_w = m_extent.x / 2.0F,
             half_h = m_extent.y / 2.0F;

        m_child[0] = new Cell(m_position, m_extent / 2.0F);
        m_child[0]->id = 0;
        m_child[1] = new Cell({ m_position.x + half_w, m_position.y }, { half_w, half_h});        
        m_child[1]->id = 1;
        m_child[2] = new Cell({ m_position.x,          m_position.y + half_h }, { half_w, half_h });
        m_child[2]->id = 2;
        m_child[3] = new Cell({ m_position.x + half_w, m_position.y + half_h }, { half_w, half_h });
        m_child[3]->id = 3;
        m_child[0]->depth = m_child[1]->depth = m_child[2]->depth = m_child[3]->depth = depth + 1;

        for (auto& aabb : m_aabbs) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]) {
                    m_child[i]->insert(aabb::UL(aabb) + camera::position, aabb);
                    m_child[i]->insert(aabb::UR(aabb) + camera::position, aabb);
                    m_child[i]->insert(aabb::DL(aabb) + camera::position, aabb);
                    m_child[i]->insert(aabb::DR(aabb) + camera::position, aabb);
                }
            }
        }
    }

    void clear() {
        line::erase(m_up_line);
        line::erase(m_down_line);
        line::erase(m_left_line);
        line::erase(m_right_line);

        m_aabbs.clear();
        id = 0;
        depth = 0;
        m_is_leaf = true;
        for (int i = 0; i < 4; ++i) {
            if (m_child[i]) {
                delete m_child[i];
                m_child[i] = nullptr;
            }
        }
    }
    bool is_overlap(cI32 aabb) {
        cRectF range = { aabb::UL(aabb).x + camera::position.x,
                         aabb::UL(aabb).y + camera::position.y,
                         aabb::DR(aabb).x - aabb::UL(aabb).x,
                         aabb::DR(aabb).y - aabb::UL(aabb).y };

        //console::log("range: ", range.x, " ", range.y, " ", range.w, " ", range.h, "\n");

        return !(range.x           > m_position.x + m_extent.x or
                 range.x + range.w < m_position.x              or
                 range.y           > m_position.y + m_extent.y or
                 range.y + range.h < m_position.y);
    }
    /*bool is_overlap(cRectF range) {
        return !(range.x           > m_position.x + m_extent.x or
                 range.x + range.w < m_position.x              or
                 range.y           > m_position.y + m_extent.y or
                 range.y + range.h < m_position.y);
    }*/

    //std::vector<I32> query(cRectF range, std::vector<I32> found = {}) {
    std::set<I32> query(cI32 aabb, std::set<I32>& found) {
        if (!is_overlap(aabb)) {
            return {};
        }
        for (auto& i : m_aabbs) {
            found.insert(i);
        }

        if (!m_is_leaf) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]) {
                    m_child[i]->query(aabb, found);
                    //found = m_child[i]->query(range, found);
                }
            }
        }

        return found;
    }

    bool is_aabb_overlap_x(cI32 a, cI32 b) {
        F32 y_dec = (aabb::DL(a).y - aabb::UL(a).y) / 16.0F;
        //y_dec = 0.0F;
        if      (y_dec < 1.0F) y_dec = 1.0F;
        else if (y_dec > 4.0F) y_dec = 4.0F;

        cVec2F a_vel = (aabb::owner(a)->velocity() + aabb::owner(a)->move_velocity()) * 1.0F;
        cVec2F b_vel = (aabb::owner(b)->velocity() + aabb::owner(b)->move_velocity()) * 1.0F;

        return aabb::UR(a).x + a_vel.x > aabb::UL(b).x + b_vel.x and
               aabb::UL(a).x + a_vel.x < aabb::UR(b).x + b_vel.x and
               aabb::DL(a).y + a_vel.y - y_dec > aabb::UL(b).y + b_vel.y and
               aabb::UL(a).y + a_vel.y + y_dec < aabb::DL(b).y + b_vel.y;
    }
    bool is_aabb_overlap_y(cI32 a, cI32 b) {
        F32 x_dec = (aabb::UR(a).x - aabb::UL(a).x) / 16.0F;
        //x_dec = 0.0F;
        if      (x_dec < 1.0F) x_dec = 1.0F;
        else if (x_dec > 4.0F) x_dec = 4.0F;

        cVec2F a_vel = (aabb::owner(a)->velocity() + aabb::owner(a)->move_velocity()) * 1.0F;
        cVec2F b_vel = (aabb::owner(b)->velocity() + aabb::owner(b)->move_velocity()) * 1.0F;

        return aabb::DL(a).y + a_vel.y > aabb::UL(b).y + b_vel.y and
               aabb::UL(a).y + a_vel.y < aabb::DL(b).y + b_vel.y and
               aabb::UR(a).x + a_vel.x - x_dec > aabb::UL(b).x + b_vel.x and
               aabb::UL(a).x + a_vel.x + x_dec < aabb::UR(b).x + b_vel.x;
    }

    void check_collision() {
        if (!m_is_leaf) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]) {
                    m_child[i]->check_collision();
                }
            }
            return;
        }
        for (auto& a : m_aabbs) {
            if (!aabb::owner(a) or !aabb::is_active(a)) continue;
            for (auto& b : m_aabbs) {
                if (a == b or !aabb::owner(b) or aabb::owner(a) == aabb::owner(b) or !aabb::is_active(b)) continue;

                cVec2F relative_vel = aabb::owner(a)->velocity() - aabb::owner(b)->velocity();

                bool is_x_larger = std::abs(relative_vel.x) > std::abs(relative_vel.y);

                auto check_overlap_x = [&]() {
                    if (is_aabb_overlap_x(a, b)) {
                        aabb::cInfo a_info = { a, aabb::owner(a) };
                        aabb::cInfo b_info = { b, aabb::owner(b) };

                        aabb::owner(a)->collide_x(a_info, b_info);
                        aabb::owner(b)->collide_x(b_info, a_info);
                    }
                    };
                auto check_overlap_y = [&]() {
                    if (is_aabb_overlap_y(a, b)) {
                        aabb::cInfo a_info = { a, aabb::owner(a) };
                        aabb::cInfo b_info = { b, aabb::owner(b) };

                        aabb::owner(a)->collide_y(a_info, b_info);
                        aabb::owner(b)->collide_y(b_info, a_info);
                    }
                    };

                if (is_x_larger) {
                    check_overlap_x();
                    check_overlap_y();
                } else {
                    check_overlap_y();
                    check_overlap_x();
                }
            }
        }
    }

    void check_collision(cI32 aabb) {
        if (!aabb::owner(aabb) or !aabb::is_active(aabb)) return;

        std::set<I32> found;
        found = query(aabb, found);

        for (auto& b : found) {
            if (aabb == b or !aabb::owner(b) or aabb::owner(aabb) == aabb::owner(b) or !aabb::is_active(b)) continue;

            cVec2F relative_vel = aabb::owner(aabb)->velocity() - aabb::owner(b)->velocity();

            bool is_x_larger = std::abs(relative_vel.x) > std::abs(relative_vel.y);

            auto check_overlap_x = [&]() {
                if (is_aabb_overlap_x(aabb, b)) {
                    aabb::cInfo a_info = { aabb, aabb::owner(aabb) };
                    aabb::cInfo b_info = { b, aabb::owner(b) };

                    aabb::owner(aabb)->collide_x(a_info, b_info);
                    aabb::owner(b)->collide_x(b_info, a_info);
                }
                };
            auto check_overlap_y = [&]() {
                if (is_aabb_overlap_y(aabb, b)) {
                    aabb::cInfo a_info = { aabb, aabb::owner(aabb) };
                    aabb::cInfo b_info = { b, aabb::owner(b) };

                    aabb::owner(aabb)->collide_y(a_info, b_info);
                    aabb::owner(b)->collide_y(b_info, a_info);
                }
                };

            if (is_x_larger) {
                check_overlap_x();
                check_overlap_y();
            } else {
                check_overlap_y();
                check_overlap_x();
            }
        }
    }

    const std::set<I32> aabbs_at(cVec2F position) {
        if (!is_within(position)) return {};
        if (!m_is_leaf) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]->is_within(position)) {
                    return m_child[i]->m_aabbs;
                }
            }
        }
        return m_aabbs;
    }

    void update() {
        line::offset(m_up_line,    -camera::position);
        line::offset(m_down_line,  -camera::position);
        line::offset(m_left_line,  -camera::position);
        line::offset(m_right_line, -camera::position);

        //console::log("aabb size: ", m_aabbs.size(), "\n");

        if (!m_is_leaf) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]) {
                    m_child[i]->update();
                }
            }
        }
    }

    void draw(std::unique_ptr<Window>& window) {
        //console::log(line::start(m_up_line).x, "\n");
        line::draw(window, m_up_line);
        line::draw(window, m_down_line);
        line::draw(window, m_left_line);
        line::draw(window, m_right_line);
        if (!m_is_leaf) {
            for (int i = 0; i < 4; ++i) {
                if (m_child[i]) {
                    m_child[i]->draw(window);
                }
            }
        }        
    }
};

static std::vector<std::unique_ptr<Cell>> s_cells;

export namespace collision_grid {    
    void init(cVec2F extent, cU8 max_depth) {
        s_max_depth = max_depth;
        s_cells.emplace_back(std::make_unique<Cell>(Vec2F{ 0, 0 }, extent));
    }    
    bool insert(cVec2F point, cI32 aabb) {
        for (auto& cell : s_cells) {
            if (!cell) continue;
            if (cell->insert(point, aabb)) {
                return true;
            }            
        }
        return false;
    }
    void clear() {
        for (auto& cell : s_cells) {
            if (cell) cell->clear();
        }
    }
    void reset() {
        for (auto& cell : s_cells) {
            if (cell) cell.reset();
        }
    }
    void update() {
        for (auto& i : s_cells) {
            if (i) i->update();
        }
    }
    void draw(std::unique_ptr<Window>& window, cU8 layer) {
        if (s_layer != layer) return;
        for (auto& i : s_cells) {
            if (i) i->draw(window);
        }
    }

    void check_collision() {
        for (auto& i : s_cells) {
            if (i) {
                i->check_collision();
            }
        }        
    }

    void check_collision(cI32 aabb) {
        for (auto& i : s_cells) {
            if (i) {
                i->check_collision(aabb);
            }
        }
    }

    std::set<I32> query(cI32 aabb, std::set<I32>& found) {
        for (auto& i : s_cells) {
            if (i) {
                found = i->query(aabb, found);
            }
        }
        return found;
    }
}