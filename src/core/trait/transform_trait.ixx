module;
#include <string>

export module transform.trait;
import transform;
import types;

export namespace transform {
    class Trait {
    protected:
        I32 m_transform = -1;
    public:
        I32   transform() const  { return m_transform; }
        virtual void  transform(cI32 id) { m_transform = id;   }

        Vec2F position()                   const { return transform::position(m_transform);           }
        void  position(cVec2F p)           const { transform::position(m_transform, p);               }
        void  position_x(cF32 x)           const { transform::position_x(m_transform, x);             }
        void  position_y(cF32 y)           const { transform::position_y(m_transform, y);             }
        void  position_add(cVec2F v)       const { transform::position_add(m_transform, v);           }
        void  position_add_x(cF32 x)       const { transform::position_add_x(m_transform, x);         }
        void  position_add_y(cF32 y)       const { transform::position_add_y(m_transform, y);         }
        Vec2F start_position()             const { return transform::start_position(m_transform);     }
        void  start_position(cVec2F p)     const { transform::start_position(m_transform, p);         }
        Vec2F velocity()                   const { return transform::velocity(m_transform);           }
        void  velocity(cVec2F v)           const { transform::velocity(m_transform, v);               }
        void  velocity_x(cF32 x)           const { transform::velocity_x(m_transform, x);             }
        void  velocity_y(cF32 y)           const { transform::velocity_y(m_transform, y);             }
        void  velocity_add(cVec2F v)       const { transform::velocity_add(m_transform, v);           }
        void  velocity_add_x(cF32 x)       const { transform::velocity_add_x(m_transform, x);         }
        void  velocity_add_y(cF32 y)       const { transform::velocity_add_y(m_transform, y);         }
        Vec2F moved_velocity()             const { return transform::moved_velocity(m_transform);     }
        void  moved_velocity(cVec2F v)     const { transform::moved_velocity(m_transform, v);         }
        void  moved_velocity_x(cF32 x)     const { transform::moved_velocity_x(m_transform, x);       }
        void  moved_velocity_y(cF32 y)     const { transform::moved_velocity_y(m_transform, y);       }
        void  moved_velocity_add(cVec2F v) const { transform::moved_velocity_add(m_transform, v);     }
        void  moved_velocity_add_x(cF32 x) const { transform::moved_velocity_add_x(m_transform, x);   }
        void  moved_velocity_add_y(cF32 y) const { transform::moved_velocity_add_y(m_transform, y);   }
        Vec2F acceleration()               const { return transform::acceleration(m_transform);       }
        void  acceleration(cVec2F a)       const { transform::acceleration(m_transform, a);           }
        void  acceleration_x(cF32 a)       const { transform::acceleration_x(m_transform, a);         }
        void  acceleration_y(cF32 a)       const { transform::acceleration_y(m_transform, a);         }
        Vec2F start_acceleration()         const { return transform::start_acceleration(m_transform); }
        void  start_acceleration(cVec2F m) const { transform::start_acceleration(m_transform, m);     }
        void  start_acceleration_x(cF32 x) const { transform::start_acceleration_x(m_transform, x);   }
        void  start_acceleration_y(cF32 y) const { transform::start_acceleration_y(m_transform, y);   }
        Vec2F deceleration()               const { return transform::deceleration(m_transform);       }
        void  deceleration(cVec2F d)       const { transform::deceleration(m_transform, d);           }
        void  deceleration_x(cF32 a)       const { transform::deceleration_x(m_transform, a);         }
        void  deceleration_y(cF32 a)       const { transform::deceleration_y(m_transform, a);         }
        Vec2F start_deceleration()         const { return transform::start_deceleration(m_transform); }
        void  start_deceleration(cVec2F m) const { transform::start_deceleration(m_transform, m);     }
        void  start_deceleration_x(cF32 x) const { transform::start_deceleration_x(m_transform, x);   }
        void  start_deceleration_y(cF32 y) const { transform::start_deceleration_y(m_transform, y);   }
        Vec2F max_velocity()               const { return transform::max_velocity(m_transform);       }
        void  max_velocity(cVec2F m)       const { transform::max_velocity(m_transform, m);           }
        void  max_velocity_x(cF32 x)       const { transform::max_velocity_x(m_transform, x);         }
        void  max_velocity_y(cF32 y)       const { transform::max_velocity_y(m_transform, y);         }
        Vec2F start_max_velocity()         const { return transform::start_max_velocity(m_transform); }
        void  start_max_velocity(cVec2F m) const { transform::start_max_velocity(m_transform, m);     }
        void  start_max_velocity_x(cF32 x) const { transform::start_max_velocity_x(m_transform, x);   }
        void  start_max_velocity_y(cF32 y) const { transform::start_max_velocity_y(m_transform, y);   }

        Vec2F direction() const   { return transform::direction(m_transform); }
        void  direction(cVec2F d) { transform::direction(m_transform, d);     }
        void  direction_x(cF32 x) { transform::direction_x(m_transform, x);   }
        void  direction_y(cF32 y) { transform::direction_y(m_transform, y);   }
    };
    bool parse_config(const std::string& text, Trait* owner);
}