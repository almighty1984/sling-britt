export module entity.particle.interact;
import console;
import entity;
import entity.particle;
import aabb;

export namespace entity {
    class ParticleInteract : public Particle {
    public:
        ParticleInteract() {
            m_type = Type::particle_interact;
        }
        const char* class_name() override { return "entity::ParticleInteract"; }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}