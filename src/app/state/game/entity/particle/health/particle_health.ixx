export module entity.particle.health;
import console;
import entity.particle;
import aabb;
import sound;
import health;

export namespace entity {
    class ParticleHealth : public Particle {
    public:
        ParticleHealth() {
            m_type = Type::particle_health;
            m_state = State::idle;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}