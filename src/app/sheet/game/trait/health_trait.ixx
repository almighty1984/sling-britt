export module health.trait;
import health;
import types;

export namespace health {    
    class Trait {
    protected:
        I32 m_health_id = -1;
    public:
        I32   health_id()      const { return m_health_id;                  } void health_id(cI32 id)      { m_health_id = id;               }
        U8    health_layer()   const { return health::layer(m_health_id);   } void health_layer(cU8 l)     { health::layer(m_health_id, l);  }
        F32   health_amount()  const { return health::amount(m_health_id);  } void health_amount(cF32 a)   { health::amount(m_health_id, a); }
        F32   health_max()     const { return health::max(m_health_id);     } void health_max(cF32 m)      { health::max(m_health_id, m);    }
        F32   health_regen()   const { return health::regen(m_health_id);   } void health_regen(cF32 r)    { health::regen(m_health_id, r);  }
        Vec2F health_offset()  const { return health::offset(m_health_id);  } void health_offset(cVec2F o) { health::offset(m_health_id, o); }
        bool  health_is_zero() const { return health::is_zero(m_health_id); }
        bool  health_is_max()  const { return health::is_max(m_health_id);  }
        void  health_reset()            { health::reset(m_health_id); }
        void  health_amount_add(cF32 a) { health::amount_add(m_health_id, a); }
        bool  health_is_hidden() const { return health::is_hidden(m_health_id); }  void health_is_hidden(bool q) { health::is_hidden(m_health_id, q); }
    };
}