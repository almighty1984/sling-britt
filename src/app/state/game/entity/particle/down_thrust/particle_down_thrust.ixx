module;
#include <sstream>

export module entity.particle.down_thrust;
import camera;
import console;
import entity;
import entity.particle;
import aabb;
//import particle_system;
import sound;

export namespace entity {
    class ParticleDownThrust : public Particle {
    public:
        ParticleDownThrust() {
            m_type = Type::particle_down_thrust;
            m_time_left_dead = 0;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}