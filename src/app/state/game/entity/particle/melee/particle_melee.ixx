module;
#include <sstream>

export module entity.particle.melee;
import camera;
import console;
import entity;
import entity.particle;
import aabb;
//import particle_system;
import sound;

export namespace entity {
    class ParticleMelee : public Particle {
    public:
        ParticleMelee() {
            m_type = Type::particle_melee;
        }
        bool hurt(Object* culprit) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}