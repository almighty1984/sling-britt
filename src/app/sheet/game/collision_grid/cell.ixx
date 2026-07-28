export module collision_grid.cell;
import aabb;
import console;
import camera;
import line;
import window;
import types;
import std;

export namespace collision_grid {
    class Cell {
        Vec2F m_position{},
            m_extent{};

        I32 m_id = -1,
            m_up_line = -1,
            m_down_line = -1,
            m_left_line = -1,
            m_right_line = -1;

        std::set<I32> m_aabbs;

        Color m_color = { 255, 255, 255 };

        U8 m_size = 1;
        U8 m_layer = NUM_VISIBLE_LAYERS - 2;

        bool m_is_hidden = true;
    public:
        I32 id() const { return m_id; }

        bool is_hidden() const { return m_is_hidden; }
        void is_hidden(bool q) {
            if (m_is_hidden == q) return;
            m_is_hidden = q;
            line::is_hidden(m_up_line, q);
            line::is_hidden(m_down_line, q);
            line::is_hidden(m_left_line, q);
            line::is_hidden(m_right_line, q);
        }

        ~Cell() {
            line::erase(m_up_line);
            line::erase(m_down_line);
            line::erase(m_left_line);
            line::erase(m_right_line);

            clear_aabbs();
        }
        Cell(cVec2F position, cVec2F extent) {
            I32 x_index = (I32)(position.x / 64.0F);
            I32 y_index = (I32)(position.y / 64.0F);
            m_id = x_index + y_index * 4;

            //console::log("id: ", m_id, "\n");
            m_position = position;
            m_extent = extent;
            m_up_line = line::make({ position.x, position.y },
                                   { position.x + extent.x, position.y });
            m_down_line = line::make({ position.x, position.y + extent.y },
                                     { position.x + extent.x, position.y + extent.y });
            m_left_line = line::make({ position.x, position.y },
                                     { position.x, position.y + extent.y });
            m_right_line = line::make({ position.x + extent.x, position.y },
                                      { position.x + extent.x, position.y + extent.y });

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

        bool is_within(cVec2F point) {
            return point.x >= m_position.x and point.x <= m_position.x + m_extent.x and
                   point.y >= m_position.y and point.y <= m_position.y + m_extent.y;
        }

        bool is_some_point_within(cI32 aabb) {
            return is_within(aabb::UL(aabb) + camera::position) or
                   is_within(aabb::UR(aabb) + camera::position) or
                   is_within(aabb::DL(aabb) + camera::position) or
                   is_within(aabb::DR(aabb) + camera::position);
        }

        bool insert(cI32 aabb) {
            if (aabb::is_active(aabb) and is_some_point_within(aabb)) {
                m_aabbs.insert(aabb);
                return true;
            }
            return false;
        }

        void clear_aabbs() {
            //line::erase(m_up_line);
            //line::erase(m_down_line);
            //line::erase(m_left_line);
            //line::erase(m_right_line);

            m_aabbs.clear();
        }
        bool is_overlap(cI32 aabb) {
            cRectF range = { aabb::UL(aabb).x + camera::position.x,
                             aabb::UL(aabb).y + camera::position.y,
                             aabb::DR(aabb).x - aabb::UL(aabb).x,
                             aabb::DR(aabb).y - aabb::UL(aabb).y };

            //console::log("range: ", range.x, " ", range.y, " ", range.w, " ", range.h, "\n");

            return !(range.x > m_position.x + m_extent.x or
                range.x + range.w < m_position.x or
                range.y           > m_position.y + m_extent.y or
                range.y + range.h < m_position.y);
        }
        /*bool is_overlap(cRectF range) {
            return !(range.x           > m_position.x + m_extent.x or
                     range.x + range.w < m_position.x              or
                     range.y           > m_position.y + m_extent.y or
                     range.y + range.h < m_position.y);
        }*/

        bool is_aabb_overlap_x(cI32 a, cI32 b) {
            F32 y_dec = (aabb::DL(a).y - aabb::UL(a).y) / 16.0F;
            //y_dec = 0.0F;
            if (y_dec < 1.0F) y_dec = 1.0F;
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
            if (x_dec < 1.0F) x_dec = 1.0F;
            else if (x_dec > 4.0F) x_dec = 4.0F;

            cVec2F a_vel = (aabb::owner(a)->velocity() + aabb::owner(a)->move_velocity()) * 1.0F;
            cVec2F b_vel = (aabb::owner(b)->velocity() + aabb::owner(b)->move_velocity()) * 1.0F;

            return aabb::DL(a).y + a_vel.y > aabb::UL(b).y + b_vel.y and
                aabb::UL(a).y + a_vel.y < aabb::DL(b).y + b_vel.y and
                aabb::UR(a).x + a_vel.x - x_dec > aabb::UL(b).x + b_vel.x and
                aabb::UL(a).x + a_vel.x + x_dec < aabb::UR(b).x + b_vel.x;
        }

        void check_collision() {
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

        void update() {
            line::offset(m_up_line,    -camera::position);
            line::offset(m_down_line,  -camera::position);
            line::offset(m_left_line,  -camera::position);
            line::offset(m_right_line, -camera::position);

            //console::log("aabb size: ", m_aabbs.size(), "\n");
        }
        void draw(std::unique_ptr<Window>& window) {
            if (m_is_hidden) return;
            //console::log(line::start(m_up_line).x, "\n");
            line::draw(window, m_up_line);
            line::draw(window, m_down_line);
            line::draw(window, m_left_line);
            line::draw(window, m_right_line);
        }
    };
}