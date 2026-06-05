export module quad_tree;
import aabb;
import console;
import types;
import line;
import sprite;
import transform;
import window;
import entity;
import aabb.trait;
import std;

cF32 size_at_depth(cU8 d) {
    F32 size = 256.0F;
    for (U8 i = 0; i < d; ++i) {
        size /= 2.0F;
    }
    return size;
}

export class QuadTreeNode {
    friend class QuadTreeNode;

    I32 m_id = 0;
    U16 m_depth = 0;

    static inline U16 s_max_depth = 3, s_max_objects = 4;
    static inline U8  s_layer = NUM_VISIBLE_LAYERS - 1;

    bool m_is_parent = false;

    QuadTreeNode* m_child[4] = { nullptr };

    RectF m_rect{ 0, 0, 0, 0 };
    std::vector<I32> m_aabbs;

    I32 m_up_line    = -1,
        m_down_line  = -1,
        m_left_line  = -1,
        m_right_line = -1;

    void split() {
        cF32 half_w = m_rect.w / 2.0F, half_h = m_rect.h / 2.0F;

        m_child[0] = new QuadTreeNode({ m_rect.x,          m_rect.y,          half_w, half_h });
        m_child[0]->m_id = m_id;

        m_child[1] = new QuadTreeNode({ m_rect.x + half_w, m_rect.y,          half_w, half_h });
        m_child[1]->m_id = m_id + 1;

        m_child[2] = new QuadTreeNode({ m_rect.x,          m_rect.y + half_h, half_w, half_h });
        m_child[2]->m_id = m_id + 2;

        m_child[3] = new QuadTreeNode({ m_rect.x + half_w, m_rect.y + half_h, half_w, half_h });
        m_child[3]->m_id = m_id + 3;

        m_child[0]->m_depth = m_child[1]->m_depth = m_child[2]->m_depth = m_child[3]->m_depth = m_depth + 1;
    }
public:
    QuadTreeNode(cRectF rect) {
        init(rect);
    }
    ~QuadTreeNode() {
        clear();
    }
    void draw(std::unique_ptr<Window>& window) {
        Color c = { 40, 40, 40 };
        for (int i = 0; i < m_depth; ++i) {
            c.r += 40;
            c.g += 40;
            c.b += 40;
        }

        line::color(m_up_line,    c);
        line::color(m_down_line,  c);
        line::color(m_left_line,  c);
        line::color(m_right_line, c);

        line::update(m_up_line);
        line::update(m_down_line);
        line::update(m_left_line);
        line::update(m_right_line);

        line::draw(window, m_up_line);
        line::draw(window, m_down_line);
        line::draw(window, m_left_line);
        line::draw(window, m_right_line);

        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->draw(window);
            }
        }
    }

    cF32 x() const { return m_rect.x; }
    cF32 y() const { return m_rect.y; }
    cF32 w() const { return m_rect.w; }
    cF32 h() const { return m_rect.h; }

    size_t depth_at(cVec2F position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] and
                    position.x >= m_child[i]->x() and position.x <= m_child[i]->x() + m_child[i]->w() and
                    position.y >= m_child[i]->y() and position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->depth_at(position);
                }
            }
        }
        return m_depth;
    }
    size_t id_at(cVec2F position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] and
                    position.x >= m_child[i]->x() and position.x <= m_child[i]->x() + m_child[i]->w() and
                    position.y >= m_child[i]->y() and position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->id_at(position);
                }
            }
        }
        return m_id;
    }
    std::vector<I32>& aabbs_at(cVec2F position) {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i] and
                    position.x >= m_child[i]->x() and position.x <= m_child[i]->x() + m_child[i]->w() and
                    position.y >= m_child[i]->y() and position.y <= m_child[i]->y() + m_child[i]->h()) {
                    return m_child[i]->aabbs_at(position);
                }
            }
        }
        return m_aabbs;
    }
    void clear() {
        line::erase(m_up_line);
        line::erase(m_down_line);
        line::erase(m_left_line);
        line::erase(m_right_line);

        m_aabbs.clear();
        m_id = 0;
        m_depth = 0;
        m_is_parent = false;
        for (int i = 0; i < 4; ++i) {
            if (m_child[i]) {
                delete m_child[i];
                m_child[i] = nullptr;
            }
        }
    }
    void init(cRectF rect) {        
        m_is_parent = false;
        m_rect = rect;

        m_up_line    = line::make({ m_rect.x, m_rect.y }, { m_rect.x + m_rect.w, m_rect.y });
        m_down_line  = line::make({ m_rect.x, m_rect.y + m_rect.h }, { m_rect.x + m_rect.w, m_rect.y + m_rect.h });
        m_left_line  = line::make({ m_rect.x, m_rect.y }, { m_rect.x,            m_rect.y + m_rect.h });
        m_right_line = line::make({ m_rect.x + m_rect.w, m_rect.y }, { m_rect.x + m_rect.w, m_rect.y + m_rect.h });

        line::layer(m_up_line,    s_layer);
        line::layer(m_down_line,  s_layer);
        line::layer(m_left_line,  s_layer);
        line::layer(m_right_line, s_layer);
    }
    static bool is_within(cRectF boundary, cVec2F point) {
        return point.x >= boundary.x and point.x <= boundary.x + boundary.w and
               point.y >= boundary.y and point.y <= boundary.y + boundary.h;
    }
    bool insert_point(cI32 aabb, cVec2F point) {
        if (!aabb::is_active(aabb)) return false;

        if (!is_within(m_rect, point)) {
            return false;
        }
        bool is_inserted = false;
        if (m_is_parent) {     
            for (size_t i = 0; i < 4; ++i) {
                if (m_child[i]->insert_point(aabb, point)) is_inserted = true;                
            }
            return is_inserted;
        }
        else {
            if (m_aabbs.size() >= s_max_objects and m_depth < s_max_depth) {
                m_is_parent = true;
                split();
                bool is_inserted = false;
                for (auto& aabb : m_aabbs) {
                    for (size_t i = 0; i < 4; ++i) {
                        if (m_child[i]->insert_point(aabb, aabb::UL(aabb))) is_inserted = true;
                        if (m_child[i]->insert_point(aabb, aabb::UR(aabb))) is_inserted = true;
                        if (m_child[i]->insert_point(aabb, aabb::DL(aabb))) is_inserted = true;
                        if (m_child[i]->insert_point(aabb, aabb::DR(aabb))) is_inserted = true;
                    }
                }
                return is_inserted;
            } else {
                if (std::find(m_aabbs.begin(), m_aabbs.end(), aabb) == m_aabbs.end()) {
                    m_aabbs.emplace_back(aabb);
                    return true;
                }
            }
        }
        return false;
    }

    void check_collision() {
        if (m_is_parent) {
            for (size_t i = 0; i < 4; ++i) {
                m_child[i]->check_collision();
            }
            return;
        }
        for (auto& a : m_aabbs) {
            if (!aabb::owner(a) or !aabb::is_active(a)) continue;
            for (auto& b : m_aabbs) {
                if (a == b or !aabb::owner(b) or aabb::owner(a) == aabb::owner(b) or !aabb::is_active(b)) continue;

                cVec2F relative_vel = aabb::owner(a)->velocity() - aabb::owner(b)->velocity();

                bool is_x_larger = std::abs(relative_vel.x) > std::abs(relative_vel.y);

                auto lambda_check_overlap_x = [&]() {
                    if (is_aabb_overlap_x(a, b)) {
                        aabb::cInfo a_info = { a, aabb::owner(a) };
                        aabb::cInfo b_info = { b, aabb::owner(b) };

                        aabb::owner(a)->collide_x(a_info, b_info);
                        aabb::owner(b)->collide_x(b_info, a_info);
                    }
                };
                auto lambda_check_overlap_y = [&]() {
                    if (is_aabb_overlap_y(a, b)) {
                        aabb::cInfo a_info = { a, aabb::owner(a) };
                        aabb::cInfo b_info = { b, aabb::owner(b) };
                                                  
                        aabb::owner(a)->collide_y(a_info, b_info);
                        aabb::owner(b)->collide_y(b_info, a_info);
                    }
                };

                if (is_x_larger) {
                    lambda_check_overlap_x();
                    lambda_check_overlap_y();
                } else {
                    lambda_check_overlap_y();
                    lambda_check_overlap_x();
                }
            }
        }
    }
    bool is_aabb_overlap_x(cI32 a, cI32 b) {
        //F32 y_dec = 0.0F;
        F32 y_dec = (aabb::DL(a).y - aabb::UL(a).y) / 16.0F;
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
        //F32 x_dec = 0.0F;
        F32 x_dec = (aabb::UR(a).x - aabb::UL(a).x) / 16.0F;
        if      (x_dec < 1.0F) x_dec = 1.0F;
        else if (x_dec > 4.0F) x_dec = 4.0F;

        cVec2F a_vel = (aabb::owner(a)->velocity() + aabb::owner(a)->move_velocity()) * 1.0F;
        cVec2F b_vel = (aabb::owner(b)->velocity() + aabb::owner(b)->move_velocity()) * 1.0F;

        return aabb::DL(a).y + a_vel.y > aabb::UL(b).y + b_vel.y and
               aabb::UL(a).y + a_vel.y < aabb::DL(b).y + b_vel.y and
               aabb::UR(a).x + a_vel.x - x_dec > aabb::UL(b).x + b_vel.x and
               aabb::UL(a).x + a_vel.x + x_dec < aabb::UR(b).x + b_vel.x;
    }
};