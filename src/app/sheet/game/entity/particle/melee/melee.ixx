export module entity.particle.melee;
import app.config;
import console;
import entity;
import entity.particle;
import aabb;
import sound;

export namespace entity {
    class ParticleMelee : public Particle {
    public:
        const char* class_name() override { return "entity::ParticleMelee"; }
        ParticleMelee() {
            m_type = Type::particle_melee;
            m_next_state = state::Type::idle;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_idle(cF32 dt) override;
        void state_dead(cF32 dt) override;

        bool hurt(Object* culprit) override {
            if (!culprit or is_dead()) return false;

            //console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");
            //console::log(class_name(), "::hurt() time to be dead: ", m_config.time_to_be_dead(), "\n");

            cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F };

            switch (culprit->type()) {
                case Type::bee:
                case Type::bee_hive:
                case Type::bug: {
                    m_time_left_alive = 0;
                    m_time_left_dead = m_config.time_to_be_dead();
                    sound_position("hit", sound_pos);
                    if (!sound_is_playing("hit")) {
                        sound_play("hit");
                    }
                    break;
                }
                case Type::brick: {
                    sound_stop("block");
                    sound_position("block", sound_pos);
                    sound_play("block");
                    break;
                }
                case Type::frog:
                case Type::mole: {
                    m_time_left_alive = 0;
                    m_time_left_dead = m_config.time_to_be_dead();
                    sound_position("hit", sound_pos);
                    if (!sound_is_playing("hit")) {
                        sound_play("hit");
                    }
                    break;
                }
                case Type::sling: {
                    //m_time_left_alive = 0;
                    //m_time_left_dead = m_config.time_to_be_dead();
                    sound_position("sling", sound_pos);
                    if (!sound_is_playing("sling")) {
                        sound_play("sling");
                    }
                    break;
                }
            }
            return true;
        }
    };
}