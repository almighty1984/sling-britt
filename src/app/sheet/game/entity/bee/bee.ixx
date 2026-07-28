export module entity.bee;
import aabb;
import anim;
import app.config;
import console;
import entity;
import particle_system;
import random;

export namespace entity {
    class Bee : public Object {
        Vec2F m_target = {};
        Vec2F m_range = { 16.0F, 1.0F };
        Vec2I m_timer = {};
        F32 m_idle_pitch = 1.0F;
    public:
        Bee() {
            m_prev_state = state::Type::none;
            m_state = m_next_state = m_start_state = state::Type::idle;
            m_type = Type::bee;
            m_idle_pitch = 1.0F + random::number(-5, 5) / 100.0F;
        }        
        const char* class_name() override { return "entity::Bee"; }

        bool hurt(Object* culprit) override {
            if (!culprit or culprit->is_hurting() or culprit->is_dead() or is_hurting() or is_dead()) return false;
            //m_time_left_hurt = m_config.time_to_hurt;

            switch (culprit->type()) {                
                case Type::brick:
                case Type::bug:
                case Type::particle_rock:
                case Type::particle_melee:
                case Type::player: {
                    max_velocity({ 10.0F, 10.0F });

                    if (culprit->type() == Type::player) {
                        velocity_x(-culprit->rotation_speed() * 0.5F);
                        velocity_y(culprit->rotation_speed() * 0.5F);
                    } else {
                        cVec2F vel_normal = Vec2F::normalize(culprit->velocity());
                        //console::log(class_name(), "::hurt() normal: ", vel_normal.x, " ", vel_normal.y, "\n");
                        velocity(vel_normal * 5.0F);
                    }

                    health_amount_add(-64.0F);
                    m_next_state = state::Type::dead;

                    /*sound_position("hit", { position().x - app::config::extent().x / 2.0F,
                                            position().y - app::config::extent().y / 2.0F });
                    sound_play("hit");*/

                    break;
                }                
            }


            /*sound_position("hurt", { position().x - app::config::extent().x / 2.0F,
                                       position().y - app::config::extent().y / 2.0F });
            sound_play("hurt");*/

            return true;
        }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_attack(cF32 dt) override;
        void state_charge(cF32 dt) override;
        void state_dead(cF32 dt) override;
        void state_idle(cF32 dt) override;

        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
                reset_anim("idle");
            }

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            state_update(dt);

            sprite_rect(anim::source(m_current_anim));
        }
    };
}