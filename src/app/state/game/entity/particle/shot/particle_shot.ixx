export module entity.particle.shot;
import console;
import entity.particle;
import aabb;
import sound;
import sprite;

export namespace entity {
    class ParticleShot : public Particle {
    public:
        ParticleShot() {
            m_type = Type::particle_shot;
            m_state = m_next_state = m_start_state = State::idle;
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        bool hurt(Object* culprit) override;

        void state_idle() override;
        void state_dead() override;
    };
}