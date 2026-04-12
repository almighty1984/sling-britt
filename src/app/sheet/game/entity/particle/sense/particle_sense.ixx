export module entity.particle.sense;
import camera;
import console;
import entity;
import entity.particle;
import aabb;

export namespace entity {
    class ParticleSense : public Particle {
    public:
        ParticleSense() {
            m_type = Type::particle_sense;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}