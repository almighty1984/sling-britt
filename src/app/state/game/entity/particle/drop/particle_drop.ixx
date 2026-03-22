module;
#include <cmath>

export module entity.particle.drop;
import console;
import entity.particle;
import aabb;
import sprite;

export namespace entity {
    class ParticleDrop : public Particle {
    public:
        ParticleDrop() {
            m_type = Type::particle_drop;
            m_state = m_next_state = State::idle;
        }
        void state_idle() override;
        void state_dead() override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}