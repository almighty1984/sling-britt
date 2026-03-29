export module entity.frog;
import anim;
import console;
import entity;
import aabb;
import health;
import line;
import random;
import sound;
import transform;
import sprite;

export namespace entity {
    class Frog : public Object {
        I32 m_tounge_line_id = -1;

        Vec2F m_tounge_start,
              m_tounge_end,
              m_sensed_position;

        state::Type m_sensed_state;        
    public:
        const char* class_name() override { return "entity::\033[0;32mFrog\033[0m"; }

        Frog() {            
            m_state = m_next_state = m_start_state = state::Type::idle;
            m_tounge_line_id = line::make(m_tounge_start, m_tounge_end);
            line::is_hidden(m_tounge_line_id, true);
        }
        ~Frog() {            
            line::erase(m_tounge_line_id);
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_blocked() override;
        void state_dead()    override;
        void state_heal()    override;
        void state_hurt()    override;
        void state_idle()    override;
        void state_jump()    override;
        void state_melee()   override;
        void state_stunned() override;
        void state_swim()    override;

        bool hurt(Object* culprit) override;
        void update() override {
            if (m_time_left_blocked > 0) --m_time_left_blocked;
            if (m_time_left_hurt > 0) --m_time_left_hurt;

            if (m_is_first_update) {
                m_is_first_update = false;
                sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
            }

            if (velocity().x < -0.5F) {
                sprite::is_leftward(m_sprite_id, true);
            } else if (velocity().x > 0.5F) {
                sprite::is_leftward(m_sprite_id, false);
            }

            /*if (velocity().y < 0.0F and !m_is_on_slope) {
                m_is_on_ground = false;
            }*/

            deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

            state_update();

            //console::log("Frog::update() state: ", to_string(m_state), "\n");

            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = state::Type::dead;
            }
            health::layer(m_health_id, m_start_layer);
        }
        void draw(std::unique_ptr<Window>& window) override {
            sprite::draw(window, m_sprite_id);
            line::draw(window, m_tounge_line_id);
        }
    };
}