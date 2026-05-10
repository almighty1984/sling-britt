export module entity.particle.melee;
import app.config;
import console;
import entity;
import entity.particle;
import aabb;
//import particle_system;
import sound;

export namespace entity {
    class ParticleMelee : public Particle {
    public:
        const char* class_name() override { return "entity::ParticleMelee"; }

        ParticleMelee() {
            m_type = Type::particle_melee;
            m_next_state = state::Type::idle;
        }
        bool hurt(Object* culprit) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_idle(cF32 dt) override;
        void state_dead(cF32 dt) override;
    };
}