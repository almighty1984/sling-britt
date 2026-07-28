export module entity.brick;
import aabb;
import app.config;
import anim;
import camera;
import console;
import entity;
import particle_system;
import sound;
import sprite;

export namespace entity {
    class Brick : public Object {
        Vec2F m_break_velocity = { 6.0F, 6.0F };
    public:
        Brick() {            
            m_state = m_next_state = m_start_state = state::Type::idle;
        }
        //const char* class_name() override { return "entity::\033[0;60mBrick\033[0m"; }
        const char* class_name() override { return "entity::Brick"; }

        bool hurt(Object* culprit) override {
            if (!culprit or is_hurting() or is_dead()) return false;

            m_time_left_hurt = m_config.time_to_hurt();

            console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");
            
            //if (m_current_anim == anim("hurt") or culprit->velocity().y > 4.0F) {
                //m_next_state = state::Type::dead;
            //} else {
                //reset_anim(anim("hurt"));
                //m_break_velocity = { 1.5F, 1.5F };
            //}

            cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F };
            
            switch (culprit->type()) {
                case Type::brick:
                case Type::particle_melee:
                case Type::particle_rock: {
                    break;
                }
                case Type::bug:
                case Type::frog: {
                    sound_position("hit", sound_pos);
                    sound_play("hit");
                    m_next_state = state::Type::dead;
                    break;
                }                
                default: {
                    sound_position("hit", sound_pos);
                    sound_play("hit");
                    sound_position("dead", sound_pos);
                    sound_play("dead");
                    m_next_state = state::Type::dead;
                    break;
                }
            }            
            return true;
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_carried(cF32 dt) override;
        void state_dead(cF32 dt)    override;
        void state_idle(cF32 dt)    override;
        void state_swim(cF32 dt)    override;
        void state_tossed(cF32 dt)  override;

        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
                reset_anim("idle");
            }
            if (velocity().x <= 0.0F) {
                m_is_near_wall_R = false;
            }
            if (velocity().x >= 0.0F) {
                m_is_near_wall_L = false;
            }
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            state_update(dt);

            sprite::rect(m_sprite, anim::source(m_current_anim));

            if (!is_dead()) {
                velocity_add_y(acceleration().y);
            }
            m_is_near_wall_L = m_is_near_wall_R = false;
            if (!m_is_on_slope and (velocity().y < 0.0F or velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
            m_weight = m_start_weight;
            if (m_inputs.empty()) return;
            //if (m_weight == m_start_weight) {                
                for (auto& i : m_inputs) {
                    if (i) m_weight += i->weight();
                }
                //console::log(class_name(), " weight: ", m_weight, "\n");
            //}

                //console::log(class_name(), " inputs: ", m_inputs.size(), "\n");
            //++m_time_in_state;
            //if (m_time_in_state > 2) {
                //m_time_in_state = 0;
                m_inputs.clear();
                //m_weight = m_start_weight;
            //}
        }
    };
}