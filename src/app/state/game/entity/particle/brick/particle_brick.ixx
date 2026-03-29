export module entity.particle.brick;
import app.config;
import console;
import entity.particle;
import aabb;
import random;
import sound;
import sprite;

export namespace entity {
    class ParticleBrick : public Particle {
    public:
        ParticleBrick() {
            m_type = Type::particle_brick;
        }
        void state_idle() override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                //m_time_left_alive = m_time_to_be_alive + random::number(0, 50);
                //console::log(class_name(), "::state_idle() m_time_left_alive: ", m_time_left_alive, "\n");
                
                for (auto& i : m_aabb_ids) {
                    aabb::is_active(i, false);
                }
            }
            m_parent = nullptr;

            if (m_time_in_state <= 5) {
                ++m_time_in_state;
            }
            if (m_time_in_state == 5) {
                for (auto& i : m_aabb_ids) {                    
                    aabb::is_active(i, true);
                }
            }

            if (velocity().x > acceleration().x or velocity().x < -acceleration().x or
                velocity().y > acceleration().y or velocity().y < -acceleration().y) {
                F32 radians = std::atan2(velocity().y, velocity().x);
                if (radians < 0.0F) radians += (3.1415926535F * 2.0F);

                cF32 degrees = radians * 180.0F / 3.1415926535F;

                sprite::angle(m_sprite_id, degrees);
            }
            
            if (position().x < -8.0F or position().x > app::config::extent().x or
                position().y < -8.0F or position().y > app::config::extent().y) {
                m_is_to_erase = true;
            }
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
    };
}