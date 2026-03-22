module;
#include <sstream>

export module entity.mole;
import anim;
import camera;
import console;
import aabb;
import entity;
import health;
import line;
import random;
import sound;
import sprite;
import types;

F32 added_degrees = 0.0F;

export namespace entity {
    class Mole : public Object {
        U16   m_time_to_turn = 40;

        Vec2F m_sensed_position;
    public:
        Mole() {
            m_state = m_next_state = m_start_state = entity::State::idle;
        }
        const char* class_name() override { return "entity::Mole"; }

        bool hurt(entity::Object* culprit) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_dead()  override;
        void state_enter() override;
        void state_exit()  override;
        void state_idle()  override;
        void state_jump()  override;
        void state_shoot() override;
        void state_swim()  override;

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
            }

            if (velocity().x + moved_velocity().x < -acceleration().x) {
                m_is_near_wall_R = false;
            }
            if (velocity().x + moved_velocity().x > acceleration().x) {
                m_is_near_wall_L = false;
            }

            //console::log("near wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");

            
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) --m_time_left_hurt;

            for (auto& i : m_time_left_colliding_with) {
                if (i.second > 0) {
                    --i.second;
                }
            }

            if (m_next_state != m_state) {
                if (m_time_left_in_state > 0) {
                    --m_time_left_in_state;
                    console::log(class_name(), "::update() m_time_left_in_state: ", m_time_left_in_state, "\n");
                }
                if (m_time_left_in_state == 0) {
                    m_prev_state = m_state;
                    m_state = m_next_state;
                    m_is_first_state_update = true;
                }
            
            }

            switch (m_state) {
                case State::dead:  state_dead();  break;
                case State::enter: state_enter(); break;
                case State::exit:  state_exit();  break;
                case State::idle:  state_idle();  break;
                case State::jump:  state_jump();  break;
                case State::shoot: state_shoot(); break;
                case State::swim:  state_swim();  break;
                default:                          break;
            }

            sprite_source_rect(anim::source(m_current_anim_id));

            if (!m_is_on_slope && (velocity().y < 0.0F || velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
            if (health::amount(m_health_id) <= 0.0f) {                
                m_time_left_in_state = 0;
                m_next_state = State::dead;                
            }
            health::layer(m_health_id, m_start_layer);
        }
    };
}