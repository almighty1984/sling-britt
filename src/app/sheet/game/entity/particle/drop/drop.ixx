export module entity.particle.drop;
import app.config;
import camera;
import console;
import entity.particle;
import aabb;
import random;
import sprite;
import types;

export namespace entity {
    class ParticleDrop : public Particle {        
        F32 m_wall_drop_on_level_y = 0.0F;

        Vec2F m_original_parent_position_on_level = {};
        Vec2F m_original_position_on_level = {};
    public:
        ParticleDrop() {
            m_type = Type::particle_drop;
            m_state = m_next_state = state::Type::idle;
        }
        const char* class_name() override { return "entity::ParticleDrop"; }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_idle(cF32 dt)       override;
        void state_ledge(cF32 dt)      override;
        void state_dead(cF32 dt)       override;
        void state_wall_slide(cF32 dt) override;
        void state_sling(cF32 dt)      override;
    };
}