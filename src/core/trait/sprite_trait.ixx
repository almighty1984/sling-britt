module;
#include <filesystem>
#include <string>

export module sprite.trait;
import sprite;
import types;

export namespace sprite {
    class Trait {
    protected:
        I32 m_sprite_id = -1;
    public:
        I32   sprite_id()   const { return m_sprite_id;                      } void sprite_id(cI32 id) { m_sprite_id = id; }
        RectI sprite_source_rect() const { return sprite::source_rect(m_sprite_id); } void sprite_source_rect(cRectI r) { sprite::source_rect(m_sprite_id, r); }

        F32 sprite_angle()          const { return sprite::angle(m_sprite_id);        } void sprite_angle(cF32 a)          { sprite::angle(m_sprite_id, a);        }
        Vec2F sprite_offset()       const { return sprite::offset(m_sprite_id);       } void sprite_offset(cVec2F o)       { sprite::offset(m_sprite_id, o);       }
        Vec2F sprite_start_offset() const { return sprite::start_offset(m_sprite_id); } void sprite_start_offset(cVec2F o) { sprite::start_offset(m_sprite_id, o); }
        Vec2F sprite_origin()       const { return sprite::origin(m_sprite_id);       } void sprite_origin(cVec2F o)       { sprite::origin(m_sprite_id, o);       }

        bool sprite_is_leftward() const { return sprite::is_leftward(m_sprite_id); } void sprite_is_leftward(bool q) { sprite::is_leftward(m_sprite_id, q); }
        bool sprite_is_upended()  const { return sprite::is_upended(m_sprite_id);  } void sprite_is_upended(bool q)  { sprite::is_upended(m_sprite_id, q);  }
        bool sprite_is_hidden()   const { return sprite::is_hidden(m_sprite_id);   } void sprite_is_hidden(bool q)   { sprite::is_hidden(m_sprite_id, q); }

        //void set_layer(cU8 layer) { sprite::layer(m_sprite_id, layer); }
    };

    bool parse_config(const std::string& text, Trait* owner, cI32 transform_id, cU8 layer, cVec2F start_offset);
}