export module entity.particle.drop;
import console;
import entity.particle;
import aabb;
import sprite;
import types;

export namespace entity {
    class ParticleDrop : public Particle {
        F32 m_wall_x = 0.0F;
        F32 m_wall_drop_y = 0.0F;
    public:
        ParticleDrop() {
            m_type = Type::particle_drop;
            m_state = m_next_state = state::Type::idle;
        }
        void state_idle(cF32 dt)       override;
        void state_dead(cF32 dt)       override;
        void state_slide_wall(cF32 dt) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}