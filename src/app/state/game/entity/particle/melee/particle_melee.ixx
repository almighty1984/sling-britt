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
            m_next_state = State::idle;
        }
        bool hurt(Object* culprit) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_idle() override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                console::log("run this\n");
                sound_play("idle");
            }
        }
    };
}