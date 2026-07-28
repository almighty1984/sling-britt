export module entity.mole;
import app.config;
import anim;
import camera;
import console;
import aabb;
import entity;
import health;
import line;
import particle_system;
import random;
import sound;
import sprite;
import types;

export namespace entity {
    class Mole : public Object {
        U16   m_time_to_turn = 40;

        Vec2F m_sensed_offset{};

        F32 m_added_degrees = 0.0F;
    public:
        Mole() {
            m_state = m_next_state = m_start_state = state::Type::idle;
        }
        //const char* class_name() override { return "entity::\033[0;36mMole\033[0m"; }
        const char* class_name() override { return "entity::Mole"; }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        bool hurt(entity::Object* culprit) override {
            if (!culprit or m_state == state::Type::swim) return false;

            if (m_time_left_hurt > 0) {
                return false;
            }

            Vec2F add_to_position = { 0.0F, 0.0F };

            F32 vel_factor = 1.0F;

            switch (culprit->type()) {
                case Type::bee:
                case Type::brick: {
                    health_amount_add(-health_max());
                    m_next_state = state::Type::enter;
                    break;
                }
                case Type::particle_brick: {
                    health_amount_add(-1.0f);
                    m_next_state = state::Type::enter;
                    break;
                }
                case Type::particle_down_thrust: {
                    if (culprit->is_dead()) return false;
                    if (m_state == state::Type::dive) return false;
                    if (culprit->parent()) {
                        F32 vel_x = (position().x - culprit->parent()->position().x) / 16.0F;
                        if (vel_x < -1.5F) vel_x = -1.5F;
                        else if (vel_x > 1.5F) vel_x = 1.5F;
                        console::log(class_name(), "::hurt() vel_x: ", vel_x, "\n");

                        velocity_x(vel_x);
                    }
                    health_amount_add(-1.0f);
                    m_next_state = state::Type::dive;
                    m_time_left_until_next_state = 0;
                    break;
                }
                case Type::particle_melee: {
                    if (culprit->type() == Type::frog) {
                        add_enemy(culprit);
                    }
                    if (m_state == state::Type::dive) {
                        if (culprit->parent()) {
                            if (sprite::is_leftward(culprit->parent()->sprite())) {
                                velocity_x(-5.0F);
                            } else {
                                velocity_x(5.0F);
                            }
                        }
                        //velocity_y(-1.0F);
                    }
                    health_amount_add(-16.0f);

                    if (culprit->position().x < position().x + 8.0F) {
                        add_to_position.x = 4.0F;
                    } else if (culprit->position().x + 8.0F > position().x) {
                        add_to_position.x = -4.0F;
                    }
                    break;
                }
                case Type::particle_rock: {
                    health_amount_add(-64.0f);
                    m_next_state = state::Type::dead;
                    break;
                }
                default: {
                    health_amount_add(-8.0f);
                    m_next_state = state::Type::enter;
                    break;
                }
            }

            m_time_left_hurt = m_config.time_to_hurt();

            cVec2F vel_normal = Vec2F::normalize(culprit->velocity());

            if (health_amount() <= 0.0F) {
                vel_factor = 4.0F;
                max_velocity({ 10.0F, 10.0F });
                m_time_left_until_next_state = 0;
                m_next_state = state::Type::dead;
            }
            velocity(vel_normal * vel_factor);

            console::log(class_name(), "::hurt() velocity: ", velocity().x, " ", velocity().y, "\n");

            sprite_is_leftward(!sprite_is_leftward());

            position_add(add_to_position);

            return true;
        }

        void state_dead(cF32 dt)  override;
        void state_enter(cF32 dt) override;
        void state_exit(cF32 dt)  override;
        void state_dive(cF32 dt)  override;
        void state_idle(cF32 dt)  override;
        void state_shoot(cF32 dt) override;
        void state_swim(cF32 dt)  override;

        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
            }
            if (velocity().x + move_velocity().x <= 0.0F) {
                m_is_near_wall_R = false;
            }
            if (velocity().x + move_velocity().x >= 0.0F) {
                m_is_near_wall_L = false;
            }

            //console::log("near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            state_update(dt);

            sprite_rect(anim::source(m_current_anim));

            if (!m_is_on_slope and (velocity().y < 0.0F or velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
            //if (health_amount() <= 0.0F) {                
            //    m_time_left_until_next_state = 0;
            //    m_next_state = state::Type::dead;                
            //}
            health_layer(m_start_layer);
        }
    };
}