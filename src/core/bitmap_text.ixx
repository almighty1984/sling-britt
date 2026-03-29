export module bitmap_text;
import std;
import console;
import sprite;
import transform;
import transform.trait;
import types;
import window;

export class BitmapText : public transform::Trait {
    bool m_is_hidden = false;

    U8  m_layer     = 0,
        m_font_size = 8;

    Vec2F               m_offset        = {  0.0F,  0.0F },
                        m_start_offset  = { -1.0F, -1.0F };
    std::string         m_text;

    std::filesystem::path m_texture_path = "res/texture/font/8_gray.png";
    std::vector<I32>    m_sprite_ids;

public:
    BitmapText() {
        m_transform_id = transform::make();
    }
    //BitmapText(const BitmapText& other) {
    //    m_is_hidden = other.m_is_hidden;
    //    m_layer = other.m_layer;
    //    m_font_size = other.m_font_size;
    //    m_offset = other.m_offset;
    //    m_start_offset = other.m_start_offset;
    //    m_text = other.m_text;
    //    m_texture_path = other.m_texture_path;
    //    m_sprite_ids = other.m_sprite_ids; // TODO: create new sprites
    //}
    //BitmapText(const BitmapTextand other) {
    //    m_sprite_ids = other.m_sprite_ids;
    //    //other.m_sprite_ids.clear();
    //}
    ~BitmapText() {
        clear_text();
        transform::erase(m_transform_id);
    }
    U8 layer()         const { return m_layer;        } void layer(cU8 l)         { m_layer        = l; }
    U8 font_size()     const { return m_font_size;    } void font_size(cU8 s)     { m_font_size    = s; }
    
    /*void transform_id(cI32 id) override {
        transform::erase(m_transform_id);
        m_transform_id = id;
        for (auto& i : m_sprite_ids) {
            sprite::transform_id(i, id);
        }
    }*/
    std::filesystem::path texture_path() const { return m_texture_path; }
    void texture(const std::filesystem::path path) {
        if (path == m_texture_path) return;
        m_texture_path = path;
        for (auto& i : m_sprite_ids) {
            sprite::texture(i, path);
        }
    }

    const std::string& get_text() const { return m_text; }
    void clear_text() {
        //if (m_sprite_ids.empty()) return;

        ////console::log("BitmapText::clear_text ids: ");
        ////for (auto& i : m_sprite_ids) {
        //    //console::log(" ", i);
        ////}
        ////console::log("\n");
        //
        for (auto& i : m_sprite_ids) {
            sprite::erase(i);
        }
        m_sprite_ids.clear();
        m_text.clear();
    }
    bool insert(size_t pos, std::string s) {
        if (pos < m_text.size()) {
            std::string text = m_text;
            text.insert(pos, s);
            console::log("BitmapText::insert(", pos, ") ", text, "\n");
            set_text(text);
            return true;
        }
        return false;
    }
    bool erase_char(size_t pos) {
        if (pos < m_text.size()) {
            std::string text = m_text;
            text.erase(text.begin() + pos, text.begin() + pos + 1);
            console::log("BitmapText::erase_char(", pos, ") ", text, "\n");
            set_text(text);
            return true;
        }
        return false;
    }
    void erase_char(char c) {
        std::string text = m_text;
        while (text.find(c, 0) != std::string::npos) {
            text.erase(text.begin() + text.find(c, 0), text.begin() + text.find(c, 0) + 1);
            console::log("BitmapText::erase_char(", c, ") text: ", text, "\n");
        }
        set_text(text);
        return;        
    }    
    bool is_hidden() { return m_is_hidden; }
    void is_hidden(bool q) {
        if (q == m_is_hidden) return;
        m_is_hidden = q;
        for (auto& i : m_sprite_ids) {
            sprite::is_hidden(i, q);
        }
    }
    cVec2F offset() const { return m_offset; }
    void offset(cVec2F offset) {
        if (offset == m_offset) return;
        for (auto& i : m_sprite_ids) {
            sprite::offset_add(i, -m_offset);
            sprite::offset_add(i, offset);
        }
        m_offset = offset;
    }
    std::vector<I32>& get_sprite_ids() { return m_sprite_ids; }

    void set_text(const std::string& text) {
        if (text == m_text or text.empty()) return;
        clear_text();
        m_text = text;
        F32 x = 0.0F, y = 0.0F;
        //console::log("BitmapText::set_text ");
        for (size_t i = 0; i < m_text.size(); ++i) {
            if (m_text.at(i) == '\n') {
                //console::log("BitmapText::set_set() found new line\n");
                y += m_font_size, x = 0.0F;
                continue;
            }
            cI32 sprite_id = sprite::make(m_texture_path);

            m_sprite_ids.emplace_back(sprite_id);
            //console::log(sprite_id, " ");
            //sprite::id(sprite_id, sprite_id);
            sprite::transform_id(sprite_id, m_transform_id);
            sprite::layer(sprite_id, m_layer);
            sprite::offset(sprite_id, m_offset + Vec2F{ x, y });

            cRectI texture_rect{ ((32 + m_text.at(i)) % 32) * m_font_size,
                                 ((m_text.at(i) - m_text.at(i) % 32 - 32) / 32) * m_font_size,
                                 m_font_size,
                                 m_font_size };
            
            sprite::source_rect(sprite_id, texture_rect);
            sprite::is_hidden(sprite_id, m_is_hidden);

            x += (F32)(m_font_size);
            //console::log("BitmapText add sprite id: ", id, "\n");
        }
        /*console::log("BitmapText::set_text ids: ");
        for (auto& i : m_sprite_ids) {
            console::log(" ", i);
        }
        console::log("\n");*/
    }
    template<typename T> requires std::is_convertible_v<T, std::string_view>
    void set_text(T t) {
        std::ostringstream oss{};
        oss << t;
        set_text(oss.str());
    }

    void draw(std::unique_ptr<Window>& window) {
        for (auto& i : m_sprite_ids) {
            sprite::draw(window, i);
        }
    }
};
