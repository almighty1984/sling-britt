export module entity.brick;
import app.config;
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
            m_state = m_next_state = m_start_state = state::Type::idle;
        }
        //const char* class_name() override { return "entity::\033[0;60mBrick\033[0m"; }
        const char* class_name() override { return "entity::Brick"; }

        bool hurt(Object* culprit) override {
            if (!culprit or m_time_left_hurt > 0) return false;

            m_time_left_hurt = m_time_to_hurt;

            console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");
            
            //if (m_current_anim == anim("hurt") or culprit->velocity().y > 4.0F) {
                m_next_state = state::Type::dead;
            //} else {
                //reset_anim(anim("hurt"));
                //m_break_velocity = { 1.5F, 1.5F };
            //}            

            sound_position("melee", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
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

            state_update();

            sprite::rect(m_sprite, anim::source(m_current_anim));

            if (!is_dead()) {
                velocity_add_y(acceleration().y);
            }
            m_is_near_wall_L = m_is_near_wall_R = false;
            if (!m_is_on_slope and (velocity().y < 0.0F or velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
        }
    };
}