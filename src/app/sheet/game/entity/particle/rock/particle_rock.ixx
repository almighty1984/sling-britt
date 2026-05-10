export module entity.particle.rock;
import console;
import entity.particle;
import aabb;
import sound;
import sprite;

export namespace entity {
    class ParticleRock : public Particle {
    public:
        ParticleRock() {
            m_type = Type::particle_rock;
            m_state = m_next_state = m_start_state = state::Type::idle;
        }
        const char* class_name() override { return "entity::ParticleRock"; }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        bool hurt(Object* culprit) override;

        void state_idle(cF32 dt) override;
        void state_dead(cF32 dt) override;
    };
}