module;
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <vector>
#include <set>

export module sprite;
import console;
import texture;
import transform;
import types;
import window;

static U8 s_scale = 1;

struct Sprite {
private:
    I32   m_id            = -1,
          m_transform_id  = -1;
    U8    m_layer         = 0,
          m_tile_set      = 0;
    F32   m_angle         = 0.0F,
          m_start_angle   = 0.0F;
    Vec2F m_origin        = { 0.0F, 0.0F };
    bool  m_is_bg         = false,
          m_is_debug      = false,
          m_is_hidden     = false,
          m_is_leftward   = false,
          m_is_upended    = false;
    Vec2F m_offset        = { 0.0F, 0.0F },
          m_start_offset  = { 0.0F, 0.0F },
          m_prev_position = { 0.0F, 0.0F },
          m_level         = { 0.0F, 0.0F },
          m_center        = { 0.0F, 0.0F };
    Color m_color         = { 127, 127, 127 },
          m_start_color   = { 127, 127, 127 };
    RectI m_source_rect   = { 0, 0, 16, 16 };
    std::filesystem::path m_texture_path;
    sf::Sprite m_sf_sprite;
    sf::Texture* m_sf_texture;
public:
    Sprite() = delete;
    Sprite(std::filesystem::path path) : m_texture_path(path), m_sf_sprite(*texture::load(path)), m_sf_texture(texture::load(path)) { }
    
    Sprite& operator=(const Sprite& other) {
        m_transform_id = other.m_transform_id;
        m_layer        = other.m_layer;
        m_tile_set     = other.m_tile_set;
        m_angle        = other.m_angle;
        m_start_angle  = other.m_start_angle;
        m_origin       = other.m_origin;
        m_is_debug     = other.m_is_debug;
        m_is_hidden    = other.m_is_hidden;
        m_is_leftward  = other.m_is_leftward;
        m_is_upended   = other.m_is_upended;
        m_offset       = other.m_offset;
        m_start_offset = other.m_start_offset;
        m_level        = other.m_level;
        m_center       = other.m_center;
        m_color        = other.m_color;
        m_start_color  = other.m_start_color;        
        m_source_rect  = other.m_source_rect;
        m_texture_path = other.m_texture_path;
        m_sf_sprite    = other.m_sf_sprite;
        m_sf_texture   = other.m_sf_texture;
        return *this;
    }
    I32 id()              const { return m_id;            } void id(cI32 id)             { m_id            = id;}
    I32 transform_id()    const { return m_transform_id;  } void transform_id(cI32 id)   { m_transform_id  = id;}
    U8 layer()            const { return m_layer;         } void layer(cU8 l)            { m_layer         = l; }
    U8 tile_set()         const { return m_tile_set;      } void tile_set(cU8 t)         { m_tile_set      = t; }
    bool is_bg()          const { return m_is_bg;         } void is_bg(bool b)           { m_is_bg         = b; }
    bool is_debug()       const { return m_is_debug;      } void is_debug(bool b)        { m_is_debug      = b; }
    bool is_hidden()      const { return m_is_hidden;     } void is_hidden(bool b)       { m_is_hidden     = b; }
    bool is_leftward()    const { return m_is_leftward;   } void is_leftward(bool b)     { m_is_leftward   = b; }
    bool is_upended()     const { return m_is_upended;    } void is_upended(bool b)      { m_is_upended    = b; }    
    Vec2F prev_position() const { return m_prev_position; } void prev_position(cVec2F p) { m_prev_position = p; }
    Vec2F level()         const { return m_level;         } void level(cVec2F l)         { m_level         = l; }
    Vec2F center()        const { return m_center;        } void center(cVec2F c)        { m_center        = c; }
    Vec2F offset()        const { return m_offset;        } void offset(cVec2F o)        { m_offset        = o; }
    void offset_x(cF32 x)     { m_offset.x  = x; }    
    void offset_y(cF32 y)     { m_offset.y  = y; }
    void add_offset(cVec2F o) { m_offset   += o; }
    void add_offset_x(cF32 x) { m_offset.x += x; }
    void add_offset_y(cF32 y) { m_offset.y += y; }
    Vec2F start_offset()  const { return m_start_offset;  } void start_offset(cVec2F s)  { m_start_offset  = s; }
    Color color()       const { return m_color;       } void color(Color c)       { m_color       = c; }
    Color start_color() const { return m_start_color; } void start_color(Color c) { m_start_color = c; }
    Vec2U texture_size() { return { m_sf_sprite.getTexture().getSize().x, m_sf_sprite.getTexture().getSize().y }; }
    sf::Sprite& sf_sprite() { return m_sf_sprite; }
    Vec2F origin() const { return m_origin; }
    void origin(cVec2F o) {
        m_origin = o;
        m_sf_sprite.setOrigin(sf::Vector2f(o.x, o.y));
    }
    F32 start_angle() const { return m_start_angle; } void start_angle(cF32 a) { m_start_angle = a; }
    F32 angle()       const { return m_angle; }
    void angle(F32 degrees) {
        if (degrees > 360.0F) degrees -= 360.0F;
        m_angle = degrees + m_start_angle;
        if (m_angle > 360.0F) m_angle -= 360.0F;

        m_sf_sprite.setRotation(sf::degrees(m_angle));
    }    
    void add_angle(cF32 degrees) {
        m_angle += degrees;
        if      (m_angle > 360.0F)  m_angle -= 360.0F;
        else if (m_angle < -360.0F) m_angle += 360.0F;
        m_sf_sprite.setRotation(m_sf_sprite.getRotation() + sf::degrees(degrees));
    }
    cF32 radians() const {
        return sf::degrees(m_angle).asRadians();
    }    
    bool texture(const std::filesystem::path path) {
        if (m_texture_path == path) return false;
        m_texture_path = path;
        m_sf_texture = texture::load(path);
        if (!m_sf_texture) return false;
        m_sf_sprite.setTexture(*m_sf_texture, true);
        m_sf_sprite.setTextureRect(sf::IntRect({ (I32)m_source_rect.x, (I32)m_source_rect.y }, { (I32)m_source_rect.w, (I32)m_source_rect.h }));
        return true;
    }
    RectI source_rect() const { return m_source_rect; }
    void source_rect(cRectI rect) {
        m_source_rect = rect;
        m_sf_sprite.setTextureRect(sf::IntRect({ rect.x, rect.y }, { rect.w, rect.h }));
    }
    void source_rect_x(cI32 x) {
        m_source_rect.x = x;
        m_sf_sprite.setTextureRect(sf::IntRect({ m_source_rect.x, m_source_rect.y }, { m_source_rect.w, m_source_rect.h }));
    }
    void source_rect_y(cI32 y) {
        m_source_rect.y = y;
        m_sf_sprite.setTextureRect(sf::IntRect({ m_source_rect.x, m_source_rect.y }, { m_source_rect.w, m_source_rect.h }));
    }
    void source_rect_w(cI32 w) {
        m_source_rect.w = w;
        m_sf_sprite.setTextureRect(sf::IntRect({ m_source_rect.x, m_source_rect.y }, { m_source_rect.w, m_source_rect.h }));
    }
    void source_rect_h(cI32 h) {
        m_source_rect.h = h;
        m_sf_sprite.setTextureRect(sf::IntRect({ m_source_rect.x, m_source_rect.y }, { m_source_rect.w, m_source_rect.h }));
    }
    void update() {
        source_rect(m_source_rect);
        m_sf_sprite.setOrigin({ m_origin.x, m_origin.y });

        m_sf_sprite.setScale({ (m_is_leftward ? -1.0F : 1.0F) * s_scale, (m_is_upended ? -1.0F : 1.0F) * s_scale});

        //if (position != prev_position) {
            //m_transformed_position = position + position();
            //prev_position = position;
            //center = { position.x + source_rect.w / 2.0F, position.y + source_rect.h / 2.0F };
            //console::log("sprite::Sprite::update position: ", prev_position.x, " ", position().x, "  ", prev_position.y, " ", position().y, "\n");        
        m_sf_sprite.setPosition({ (transform::position(m_transform_id).x + m_offset.x + m_origin.x) * s_scale,
                                  (transform::position(m_transform_id).y + m_offset.y + m_origin.y) * s_scale });
        //}
    }
};

std::vector<Sprite*> s_sprites;
std::vector<I32>     s_unused_ids;

export namespace sprite {
    constexpr bool   is_valid(size_t i) { return (i >= 0 && i < s_sprites.size() && s_sprites.at(i)) ? true : false; }

    struct Data {
        U8 tile_set, layer;
        U16 source_y, source_x, y, x;
        Data() : tile_set(0), layer(0), source_y(0), source_x(0), y(0), x(0) {}
    };

    std::vector<I32> ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_object_ids;
        std::for_each(s_sprites.cbegin(), s_sprites.cend(), [&](Sprite* i) {
                if (i && i->id() != -1 && !i->is_bg() && i->layer() == layer && !i->is_hidden()) {
                    same_layer_object_ids.emplace_back(i->id());
                }
            }
        );
        return same_layer_object_ids;
    }

    std::vector<I32> bg_ids_in_layer(cU8 layer) {
        std::vector<I32> same_layer_bg_ids;
        std::for_each(s_sprites.cbegin(), s_sprites.cend(), [&](Sprite* i) {
            if (i && i->id() != -1 && i->is_bg() && i->layer() == layer && !i->is_hidden()) {
                same_layer_bg_ids.emplace_back(i->id());
            }
            }
        );
        return same_layer_bg_ids;
    }

    size_t  size()        { return s_sprites.size();    }
    size_t  unused_size() { return s_unused_ids.size(); }

    Sprite* get(cI32 i) { return is_valid(i) ? s_sprites.at(i) : nullptr; }
    void update(cI32 i) { if (is_valid(i)) s_sprites.at(i)->update(); }
        
    I32   transform_id(cI32 i)   { return is_valid(i) ? s_sprites.at(i)->transform_id()  :     -1;  }
    U8    layer(cI32 i)          { return is_valid(i) ? s_sprites.at(i)->layer()         :      0;  }
    U8    tile_set(cI32 i)       { return is_valid(i) ? s_sprites.at(i)->tile_set()      :      0;  } 
    bool  is_bg(cI32 i)          { return is_valid(i) ? s_sprites.at(i)->is_bg()         :  false;  }
    bool  is_debug(cI32 i)       { return is_valid(i) ? s_sprites.at(i)->is_debug()      :  false;  }
    bool  is_hidden(cI32 i)      { return is_valid(i) ? s_sprites.at(i)->is_hidden()     :  false;  }
    bool  is_leftward(cI32 i)    { return is_valid(i) ? s_sprites.at(i)->is_leftward()   :  false;  }
    bool  is_upended(cI32 i)     { return is_valid(i) ? s_sprites.at(i)->is_upended()    :  false;  }
    Vec2F offset(cI32 i)         { return is_valid(i) ? s_sprites.at(i)->offset()        : Vec2F{}; }
    Vec2F start_offset(cI32 i)   { return is_valid(i) ? s_sprites.at(i)->start_offset()  : Vec2F{}; }
    Vec2F prev_position(cI32 i)  { return is_valid(i) ? s_sprites.at(i)->prev_position() : Vec2F{}; }
    Vec2F level(cI32 i)          { return is_valid(i) ? s_sprites.at(i)->level()         : Vec2F{}; }
    Vec2F origin(cI32 i)         { return is_valid(i) ? s_sprites.at(i)->origin()        : Vec2F{}; }
    Vec2F center(cI32 i)         { return is_valid(i) ? s_sprites.at(i)->center()        : Vec2F{}; }
    F32   angle(cI32 i)          { return is_valid(i) ? s_sprites.at(i)->angle()         :    0.0F; }
    F32   start_angle(cI32 i)    { return is_valid(i) ? s_sprites.at(i)->start_angle()   :    0.0F; }
    F32   radians(cI32 i)        { return is_valid(i) ? s_sprites.at(i)->radians()       :    0.0F; }
    Color color(cI32 i)          { return is_valid(i) ? s_sprites.at(i)->color()         : Color{}; }
    Color start_color(cI32 i)    { return is_valid(i) ? s_sprites.at(i)->start_color()   : Color{}; }
    RectI source_rect(cI32 i)    { return is_valid(i) ? s_sprites.at(i)->source_rect()   : RectI{}; }
    Vec2U texture_size(cI32 i)   { return is_valid(i) ? s_sprites.at(i)->texture_size()  : Vec2U{}; }

    void transform_id(cI32 i, cI32 t)    { if (is_valid(i)) s_sprites.at(i)->transform_id(t);  }
    void layer(cI32 i, cU8 l)            { if (is_valid(i)) s_sprites.at(i)->layer(l);         }
    void tile_set(cI32 i, cU8 t)         { if (is_valid(i)) s_sprites.at(i)->tile_set(t);      }
    void is_bg(cI32 i, bool q)           { if (is_valid(i)) s_sprites.at(i)->is_bg(q);         }
    void is_debug(cI32 i, bool q)        { if (is_valid(i)) s_sprites.at(i)->is_debug(q);      }
    void is_hidden(cI32 i, bool q)       { if (is_valid(i)) s_sprites.at(i)->is_hidden(q);     }
    void is_leftward(cI32 i, bool q)     { if (is_valid(i)) s_sprites.at(i)->is_leftward(q);   }
    void is_upended(cI32 i, bool q)      { if (is_valid(i)) s_sprites.at(i)->is_upended(q);    }
    void offset(cI32 i, cVec2F o)        { if (is_valid(i)) s_sprites.at(i)->offset(o);        }
    void offset_x(cI32 i, cF32 x)        { if (is_valid(i)) s_sprites.at(i)->offset_x(x);      }
    void offset_y(cI32 i, cF32 y)        { if (is_valid(i)) s_sprites.at(i)->offset_y(y);      }
    void add_offset(cI32 i, cVec2F o)    { if (is_valid(i)) s_sprites.at(i)->add_offset(o);    }
    void start_offset(cI32 i, cVec2F s)  { if (is_valid(i)) s_sprites.at(i)->start_offset(s);  }
    void origin(cI32 i, cVec2F o)        { if (is_valid(i)) s_sprites.at(i)->origin(o);        }
    void prev_position(cI32 i, cVec2F p) { if (is_valid(i)) s_sprites.at(i)->prev_position(p); }
    void level(cI32 i, cVec2F l)         { if (is_valid(i)) s_sprites.at(i)->level(l);         }    
    void add_angle(cI32 i, cF32 a)       { if (is_valid(i)) s_sprites.at(i)->add_angle(a);     }
    void angle(cI32 i, cF32 a)           { if (is_valid(i)) s_sprites.at(i)->angle(a);         }
    void start_angle(cI32 i, cF32 a)     { if (is_valid(i)) s_sprites.at(i)->start_angle(a);   }
    void color(cI32 i, Color c)          { if (is_valid(i)) s_sprites.at(i)->color(c);         }
    void start_color(cI32 i, Color c)    { if (is_valid(i)) s_sprites.at(i)->start_color(c);   }
    void source_rect(cI32 i, RectI r)    { if (is_valid(i)) s_sprites.at(i)->source_rect(r);   }
    void source_rect_x(cI32 i, cI32 x)   { if (is_valid(i)) s_sprites.at(i)->source_rect_x(x); }
    void source_rect_y(cI32 i, cI32 y)   { if (is_valid(i)) s_sprites.at(i)->source_rect_y(y); }
    void source_rect_w(cI32 i, cI32 w)   { if (is_valid(i)) s_sprites.at(i)->source_rect_w(w); }
    void source_rect_h(cI32 i, cI32 h)   { if (is_valid(i)) s_sprites.at(i)->source_rect_h(h); }
    void texture(cI32 i, const std::filesystem::path path) { if (is_valid(i)) s_sprites.at(i)->texture(path); }
    
    cI32 make(const std::filesystem::path& path) {
        Sprite* object = new Sprite(path);
        if (!s_unused_ids.empty()) {
            object->id(s_unused_ids.back());
            s_unused_ids.pop_back();
            //console::log("sprite::make unused: ", object->id, "\n");
            if (!s_sprites.empty() && object->id() >= 0 && object->id() < s_sprites.size() && s_sprites.at(object->id())) {
                delete s_sprites.at(object->id());
                //s_sprites.at(object->id) = nullptr;
            }
            s_sprites.at(object->id()) = object;
        }
        else {
            object->id(s_sprites.size());
            //console::log("sprite::make id: ", object->id, "\n");
            s_sprites.emplace_back(object);
        }
        return object->id();
    }
    bool erase(cI32 i) {
        if (!is_valid(i)) {
            //console::log("sprite::erase ", i, " can't do it! size: ", s_sprites.size(), "\n");
            return false;
        }
        //console::log("sprite::erase ", i, "\n");
        delete s_sprites.at(i);
        s_sprites.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;        
    }
    void clear() {
        for (I32 i = 0; i < s_sprites.size(); ++i) {
            erase(i);
        }
        s_sprites.clear();
        s_unused_ids.clear();
    }
    void update() {
        for (auto& i : s_sprites) {
            if (i) i->update();
        }
    }    
    void draw(std::unique_ptr<Window>& window, cI32 i) {
        if (!window || !(is_valid(i)) || s_sprites.at(i)->is_hidden()) return;

        s_scale = window->scale();

        if (transform::position(s_sprites.at(i)->transform_id()).x + s_sprites.at(i)->offset().x < window->view().x - s_sprites.at(i)->source_rect().w ||
            transform::position(s_sprites.at(i)->transform_id()).x + s_sprites.at(i)->offset().x > window->view().w ||
            transform::position(s_sprites.at(i)->transform_id()).y + s_sprites.at(i)->offset().y < window->view().y - s_sprites.at(i)->source_rect().h ||
            transform::position(s_sprites.at(i)->transform_id()).y + s_sprites.at(i)->offset().y > window->view().h) {
            return;
        }
        window->draw_sf_sprite(s_sprites.at(i)->sf_sprite());
    }
    void draw_in_layer(std::unique_ptr<Window>& window, cU8 layer) {        
        for (const auto& i : ids_in_layer(layer)) {
            if (is_valid(i)) {
                draw(window, i);
            }
        }
    }
    void draw_bg_in_layer(std::unique_ptr<Window>& window, cU8 layer) {
        for (const auto& i : bg_ids_in_layer(layer)) {
            if (is_valid(i)) {
                draw(window, i);
            }
        }        
    }
    void save_level(const std::filesystem::path& path, std::vector<I32>& grid_sprite_ids) {
        console::log("sprite::save_level...");
        std::vector<Sprite*> to_save;
        for (auto& i : grid_sprite_ids) {
            cU16   rows    = s_sprites.at(i)->source_rect().h / 16;
            cU16   columns = s_sprites.at(i)->source_rect().w / 16;
            cVec2F offset  = s_sprites.at(i)->offset();

            for (U8 layer = 0; layer < NUM_LEVEL_LAYERS; ++layer) {
                for (U16 y = 0; y < rows; ++y) {
                    for (U16 x = 0; x < columns; ++x) {                                                        
                        /*std::for_each(m_objects.cbegin(), m_objects.cend(),
                        [&](Sprite* i) { if (i && i->offset == offset + Vec2F{ x * 16.0F, y * 16.0F }) sprites_to_save.emplace_back(i); }
                        );*/
                        for (auto& i : s_sprites) {
                            if (!i) continue;
                            if (i->layer() == layer && i->offset() == offset + Vec2F{x * 16.0F, y * 16.0F}) {
                                to_save.emplace_back(i);
                            }
                        }
                    }
                }
            }
        }
        console::log("size: ", to_save.size(), "...");

        cU16 size = (U16)to_save.size();

        std::ofstream out_file(path, std::ios::out | std::ios::binary);
        out_file.write((I8*)&size, sizeof(U16));

        for (auto& i : to_save) {
            if (!i) continue;
            //console::log((int)i->layer, " ", i->offset.y, " ", i->offset.x, " ");

            cU8  tile_set = i->tile_set();
            cU8  layer    = i->layer();
            cU16 source_y = (U16)i->source_rect().y,
                 source_x = (U16)i->source_rect().x;
            cU16 y        = (U16)i->offset().y,
                 x        = (U16)i->offset().x;

            out_file.write((I8*)&tile_set, sizeof(U8));
            out_file.write((I8*)&layer, sizeof(U8));
            out_file.write((I8*)&source_y, sizeof(U16));
            out_file.write((I8*)&source_x, sizeof(U16));
            out_file.write((I8*)&y, sizeof(U16));
            out_file.write((I8*)&x, sizeof(U16));
        }
        console::log("done\n");
    }
    std::vector<Data> load_level(const std::filesystem::path& path) {
        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        U16 size = 0;
        in_file.read((I8*)&size, sizeof(U16));

        console::log("sprite::load_level(", path.string(), ") size: ", size, "\n");

        std::vector<Data> sprite_data_vec;
        for (U16 i = 0; i < size; ++i) {
            Data sprite_data{};
            in_file.read((I8*)&sprite_data.tile_set, sizeof(U8));
            in_file.read((I8*)&sprite_data.layer, sizeof(U8));
            in_file.read((I8*)&sprite_data.source_y, sizeof(U16));
            in_file.read((I8*)&sprite_data.source_x, sizeof(U16));
            in_file.read((I8*)&sprite_data.y, sizeof(U16));
            in_file.read((I8*)&sprite_data.x, sizeof(U16));
            sprite_data_vec.emplace_back(sprite_data);
        }

        in_file.close();

        return sprite_data_vec;
    }
}