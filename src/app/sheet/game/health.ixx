export module health;
import console;
import line;
import types;
import window;
import std;

class Bar {
    I32 m_bar_line     = -1,
        m_bar_hit_line = -1,
        m_bar_bg_line  = -1;

public:
    I32 id = -1;

    U8 layer = 0;

    F32 amount = 16.0F,
        max    = 16.0F,
        regen  = 0.1F;

    Vec2F offset = { 0.0F, 0.0F };

    bool is_hidden = false;

    Bar() = delete;
    Bar(cI32 transform) {
        m_bar_bg_line = line::make({ 0.0F, 0.0F }, { max, 0.0F });
        line::size(m_bar_bg_line, 2);
        line::layer(m_bar_bg_line, layer);
        line::transform(m_bar_bg_line, transform);

        m_bar_hit_line = line::make({ 0.0F, 0.0F }, { max, 0.0F });
        line::transform(m_bar_hit_line, transform);
        line::size(m_bar_hit_line, 2);
        line::layer(m_bar_hit_line, layer);
        line::color(m_bar_hit_line, { 191, 191, 191 });

        m_bar_line = line::make({ 0.0F, 0.0F }, { max, 0.0F });
        line::transform(m_bar_line, transform);
        line::size(m_bar_line, 2);
        line::layer(m_bar_line, layer);
        line::color(m_bar_line, { 255, 0, 0 });
    }
    ~Bar() {
        line::erase(m_bar_bg_line);
        line::erase(m_bar_hit_line);
        line::erase(m_bar_line);
    }
    void reset() {
        amount = max;
    }
    void update() {
        if      (amount < 0.0f) amount = 0.0f;
        else if (amount > max)  amount = max;

        line::end_x(m_bar_bg_line, max);
        line::end_x(m_bar_line, amount);
        if (line::end(m_bar_line).x < 0.0F) {
            line::end_x(m_bar_line, 0.0F);
        }
        if (line::end(m_bar_hit_line).x < 0.0F) {
            line::end_x(m_bar_hit_line, 0.0F);
        }

        if (line::end(m_bar_hit_line).x < line::end(m_bar_line).x) {
            line::end_x(m_bar_hit_line, line::end(m_bar_line).x);
        } else if (line::end(m_bar_hit_line).x > line::end(m_bar_line).x) {
            cF32 diff_x = line::end(m_bar_hit_line).x - line::end(m_bar_line).x;
            line::add_end_x(m_bar_hit_line, -diff_x / 5.0f);
        }

        line::offset(m_bar_bg_line, offset);
        line::offset(m_bar_hit_line, offset);
        line::offset(m_bar_line, offset);

        line::layer(m_bar_bg_line, layer);
        line::layer(m_bar_hit_line, layer);
        line::layer(m_bar_line, layer);

        if (amount > 0.0f and amount < max) {
            amount += regen;
        }
    }
    void draw(std::unique_ptr<Window>& window) {
        if (amount <= 0.0F or amount >= max or is_hidden) return;
        line::draw(window, m_bar_bg_line);
        line::draw(window, m_bar_hit_line);
        line::draw(window, m_bar_line);
    }
};

std::vector<Bar*>  s_bars;
std::vector<I32>   s_unused_ids;

export namespace health {
    constexpr bool is_valid(size_t i) { return (i < s_bars.size() and s_bars.at(i)) ? true : false; }

    size_t  size() { return s_bars.size(); }

    bool  is_max(cI32 i) { return is_valid(i) ? s_bars.at(i)->amount >= s_bars.at(i)->max : false; }

    U8    layer(cI32 i)     { return is_valid(i) ? s_bars.at(i)->layer     :       0; }
    F32   amount(cI32 i)    { return is_valid(i) ? s_bars.at(i)->amount    :    0.0F; }
    F32   max(cI32 i)       { return is_valid(i) ? s_bars.at(i)->max       :    0.0F; }
    F32   regen(cI32 i)     { return is_valid(i) ? s_bars.at(i)->regen     :    0.0F; }
    Vec2F offset(cI32 i)    { return is_valid(i) ? s_bars.at(i)->offset    : Vec2F{}; }
    bool  is_hidden(cI32 i) { return is_valid(i) ? s_bars.at(i)->is_hidden :   false; }

    void  layer(cI32 i, cU8 l)       { if (is_valid(i)) s_bars.at(i)->layer     = l; }
    void  amount(cI32 i, cF32 a)     { if (is_valid(i)) s_bars.at(i)->amount    = a; }
    void  amount_add(cI32 i, cF32 a) { if (is_valid(i)) s_bars.at(i)->amount   += a; }
    void  max(cI32 i, cF32 m)        { if (is_valid(i)) s_bars.at(i)->max       = m; }
    void  regen(cI32 i, cF32 r)      { if (is_valid(i)) s_bars.at(i)->regen     = r; }
    void  offset(cI32 i, cVec2F o)   { if (is_valid(i)) s_bars.at(i)->offset    = o; }
    void  is_hidden(cI32 i, bool q)  { if (is_valid(i)) s_bars.at(i)->is_hidden = q; }

    void reset(cI32 i) { if (is_valid(i)) s_bars.at(i)->reset(); }

    I32 make(cI32 transform) {
        Bar* object = new Bar(transform);
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
        } else {
            object->id = s_bars.size();
            s_bars.emplace_back(nullptr);
        }
        if (object->id >= 0 and object->id < s_bars.size() and s_bars.at(object->id)) {
            delete s_bars.at(object->id);
        }
        s_bars.at(object->id) = object;
        return object->id;
    }
    bool erase(cI32 i) {            
        if (!is_valid(i)) {
            //console::log("health::erase ", i, " can't do it\n");
            return false;
        }
        delete s_bars.at(i);
        s_bars.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;            
    }
    void update() {
        for (auto& i : s_bars) {
            if (i) i->update();
        }
    }
    void draw(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : s_bars) {
            if (i) {
                i->draw(window);
            }
        }
    }
    void clear() {
        for (size_t i = 0; i < s_bars.size(); ++i) {
            erase(i);
        }
        s_bars.clear();
        s_unused_ids.clear();
    }
}
