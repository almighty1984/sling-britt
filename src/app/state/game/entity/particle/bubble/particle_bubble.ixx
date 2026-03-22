module;
#include <cmath>

export module entity.particle.bubble;
import console;
import entity.particle;
import aabb;

export namespace entity {
    class ParticleBubble : public Particle {
    public:
        ParticleBubble() {
            m_type = Type::particle_bubble;
        }
        void state_idle() override;
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}