export module entity.bee_hive;
import aabb;
import app.config;
import console;
import entity;
import particle_system;

export namespace entity {
    class BeeHive : public Object {
    public:
        BeeHive() {
            m_state = m_next_state = m_start_state = state::Type::idle;
            m_type = Type::bee_hive;            
        }
        const char* class_name() override { return "entity::BeeHive"; }

        bool hurt(Object* culprit) override {
            if (!culprit or culprit->is_hurting() or culprit->is_dead() or is_hurting() or is_dead()) return false;
            m_time_left_hurt = m_config.time_to_hurt();

            console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");

            switch (culprit->type()) {
                case Type::brick:
                case Type::bug: {
                    health_amount_add(-64.0F);
                    if (culprit->parent()) {
                        m_parent = culprit->parent();                        
                    }
                    break;
                }
                case Type::particle_melee:
                case Type::particle_rock: {
                    health_amount_add(-16.0F);
                    if (culprit->parent()) {
                        m_parent = culprit->parent();
                    }
                    break;
                }
            }

            //console::log(class_name(), "::hurt() health: ", health_amount(), "\n");
            if (health_amount() <= 0.0F) {
                velocity(culprit->velocity());
                m_next_state = state::Type::dead;
            }
            return true;
        }

        void state_dead(cF32 dt) override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sprite_is_hidden(true);
                m_time_left_dead = U16_MAX;
                m_time_left_alive = 0;
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }
                Vec2F blood_vel = velocity() + move_velocity();
                particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                    particle::Type::drop_blood,
                                    position() + Vec2F{ 6.0F, -4.0F },
                                    blood_vel * 1.0F, 1.0F,
                                    state::Type::idle);
                console::log(class_name(), "::state_dead() blood vel: ", blood_vel.x, " ", blood_vel.y, "\n");

                particle::spawn(this,
                                particle::Type::health,
                                position() + Vec2F{ 4.0F, 0.0F }, {});

                sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                         position().y - app::config::extent().y / 2.0F });
                sound_play("dead");
            }
        }

        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
            }
            if (!is_hurting()) {
                m_parent = nullptr;
            }

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }
            state_update(dt);

            //console::log("layer: ", (int)sprite_layer(), "\n");

            health_layer(m_start_layer);
        }
    };
}