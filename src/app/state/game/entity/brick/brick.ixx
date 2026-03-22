module;
#include <cmath>
#include <sstream>
export module entity.brick;
import anim;
import camera;
import console;
import entity;
//import particle_system;
import sound;
import sprite;

export namespace entity {
    class Brick : public Object {
        Vec2F m_break_velocity = { 3.5F, 3.5F };
    public:
        Brick() {            
            m_state = m_next_state = m_start_state = entity::State::idle;
        }
        bool hurt(Object* culprit) override {
            if (!culprit || m_time_left_hurt > 0) return false;

            m_time_left_hurt = m_time_to_hurt;

            console::log("entity::Brick::hurt() culprit: ", to_string(culprit->type()), "\n");
            
            //if (m_current_anim_id == anim_id("hurt") || culprit->velocity().y > 4.0F) {
                m_next_state = entity::State::dead;
            //} else {
                //reset_anim(anim_id("hurt"));
                //m_break_velocity = { 1.5F, 1.5F };
            //}            

            sound_position("melee", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("melee");            
            
            return true;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_carried() override;
        void state_dead()    override;
        void state_idle()    override;
        void state_swim()    override;
        void state_tossed()  override;

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                reset_anim("idle");
            }
                        
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_time_left_hurt > 0) --m_time_left_hurt;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            switch (m_state) {
                case State::carried: state_carried(); break;
                case State::dead:    state_dead();    break;
                case State::idle:    state_idle();    break;
                case State::swim:    state_swim();    break;
                case State::tossed:  state_tossed();  break;
                default: break;
            }

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (!is_dead()) {
                add_velocity_y(acceleration().y);
            }
            m_is_near_wall_L = m_is_near_wall_R = false;
            if (!m_is_on_slope && (velocity().y < 0.0F || velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
        }
    };
}