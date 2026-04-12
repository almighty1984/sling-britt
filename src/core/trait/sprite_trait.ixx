module;
#include <filesystem>
#include <string>

export module sprite.trait;
import sprite;
import types;

export namespace sprite {
    class Trait {
    protected:
        I32 m_sprite = -1;
    public:
        I32   sprite()           const { return m_sprite;                       }
        RectI sprite_rect()         const { return sprite::rect(m_sprite);         }
        F32   sprite_angle()        const { return sprite::angle(m_sprite);        }        
        Vec2F sprite_offset()       const { return sprite::offset(m_sprite);       }
        Vec2F sprite_start_offset() const { return sprite::start_offset(m_sprite); }
        Vec2F sprite_origin()       const { return sprite::origin(m_sprite);       }
        bool  sprite_is_leftward()  const { return sprite::is_leftward(m_sprite);  }
        bool  sprite_is_upended()   const { return sprite::is_upended(m_sprite);   }
        bool  sprite_is_hidden()    const { return sprite::is_hidden(m_sprite);    }
        bool  sprite_layer()        const { return sprite::layer(m_sprite);        }

        void sprite(cI32 id)                  { m_sprite = id; }
        void sprite_rect(cRectI r)         const { sprite::rect(m_sprite, r);         }
        void sprite_angle(cF32 a)          const { sprite::angle(m_sprite, a);        }
        void sprite_angle_add(cF32 a)      const { sprite::angle_add(m_sprite, a);    }
        void sprite_offset(cVec2F o)       const { sprite::offset(m_sprite, o);       }
        void sprite_offset_x(cF32 x)       const { sprite::offset_x(m_sprite, x);     }
        void sprite_offset_y(cF32 y)       const { sprite::offset_y(m_sprite, y);     }
        void sprite_start_offset(cVec2F o) const { sprite::start_offset(m_sprite, o); }
        void sprite_origin(cVec2F o)       const { sprite::origin(m_sprite, o);       }
        void sprite_is_leftward(bool q)    const { sprite::is_leftward(m_sprite, q);  }
        void sprite_is_upended(bool q)     const { sprite::is_upended(m_sprite, q);   }
        void sprite_is_hidden(bool q)      const { sprite::is_hidden(m_sprite, q);    }
        void sprite_layer(cU8 l)           const { sprite::layer(m_sprite, l);        }        
    };
    bool parse_config(const std::string& text, Trait* owner, cI32 transform, cU8 layer, cVec2F start_offset);
}