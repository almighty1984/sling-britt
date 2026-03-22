module;
#include <string>

export module health.trait;
import health;
import types;

export namespace health {    
    class Trait {
    protected:
        I32 m_health_id = -1;
    public:
        I32 health_id()     const { return m_health_id;                 } void health_id(cI32 id)    { m_health_id = id;               }
        U8  health_layer()  const { return health::layer(m_health_id);  } void health_layer(cU8 l)   { health::layer(m_health_id, l);  }
        F32 health_amount() const { return health::amount(m_health_id); } void health_amount(cF32 a) { health::amount(m_health_id, a); }
        F32 health_max()    const { return health::max(m_health_id);    } void health_max(cF32 m)    { health::max(m_health_id, m);    }
        void health_reset()            { health::reset(m_health_id); }
        void health_add_amount(cF32 a) { health::add_amount(m_health_id, a); }
    };
}