export module entity.frog;
import app.config;
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
import particle_system;

export namespace entity {
    class Frog : public Object {
        I32 m_tounge_line = -1;

        Vec2F m_tounge_start,
              m_tounge_end;

        state::Type m_sensed_state;        
    public:
        //const char* class_name() override { return "entity::\033[0;32mFrog\033[0m"; }
        const char* class_name() override { return "entity::Frog"; }

        Frog() {            
            m_state = m_next_state = m_start_state = state::Type::idle;
            m_tounge_line = line::make(m_tounge_start, m_tounge_end);
            line::is_hidden(m_tounge_line, true);
        }
        ~Frog() {            
            line::erase(m_tounge_line);
        }        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_blocked(cF32 dt) override;
        void state_dead(cF32 dt)    override;
        void state_heal(cF32 dt)    override;
        void state_hurt(cF32 dt)    override;
        void state_idle(cF32 dt)    override;
        void state_jump(cF32 dt)    override;
        void state_melee(cF32 dt)   override;
        void state_stunned(cF32 dt) override;
        void state_swim(cF32 dt)    override;

        bool hurt(Object* culprit) override {
            if (m_time_left_hurt > 0) return false;
            //m_time_left_hurt = m_time_to_hurt;

            if (!culprit) return false;

            Vec2F add_to_position = { 0.0F, 0.0F };

            switch (culprit->type()) {
                case Type::brick:
                case Type::bug: {
                    m_sensed_objects.clear();
                    m_sensed_objects.emplace_back(culprit);
                                        
                    //cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y) * 3.0F;
                    cF32 amount = 64.0F;

                    console::warning(class_name(), "::hurt() amount: ", amount, "\n");
                    health_amount_add(-amount);
                    //health::get(m_health_id)->amount -= 32.0F;

                    m_next_state = health_amount() > 0.0F ? state::Type::hurt : state::Type::dead;

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
                    break;
                }
                case Type::particle_brick: {
                    m_sensed_objects.clear();
                    m_sensed_objects.emplace_back(culprit);

                    //console::log("amount: ", amount, "\n");

                    health_amount_add(-4.0F);
                    m_next_state = state::Type::hurt;
                    break;
                }
                case Type::particle_down_thrust: {
                    velocity_x(culprit->velocity().x * 0.7F);
                    if (m_is_on_ground) {
                        velocity_y(-2.0f);
                    }
                    m_next_state = state::Type::stunned;
                    break;
                }
                case Type::particle_melee: {
                    m_sensed_objects.clear();
                    m_sensed_objects.emplace_back(culprit->parent());

                    velocity(culprit->velocity() * 0.25F);
                    health_amount_add(-8.0F);
                    m_next_state = state::Type::hurt;
                    //m_time_left_to_react += 10;

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x = 2.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -2.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y = 2.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -2.0F;
                    
                    break;
                }
                case Type::particle_rock: {
                    //m_time_left_hurt = 0;
                    if (culprit->parent()) {
                        console::log(class_name(), "::hurt() particle_rock parent: ", to_string(culprit->parent()->type()), "\n");
                    } else {
                        console::log(class_name(), "::hurt() parent is nullptr\n");
                    }
                    m_sensed_objects.clear();
                    m_sensed_objects.emplace_back(culprit->parent());
                    m_next_state = state::Type::hurt;
                    health_amount_add(-8.0F);
                    break;
                }
                case Type::player: {
                    if (culprit->state() == state::Type::run) {
                        if (culprit->velocity().y >= 6.0F) {
                            velocity(culprit->velocity() * 0.5F);
                            m_next_state = state::Type::stunned;
                            m_time_to_be_in_state = 100;

                            console::log("Frog::hurt() m_time_to_be_in_state: ", m_time_to_be_in_state, "\n");

                            sound_position("bounce", { position().x - app::config::extent().x / 2.0F,
                                                       position().y - app::config::extent().y / 2.0F });
                            sound_play("bounce");

                            particle::spawn(this, particle::Type::hit, position(), {});
                        }
                    } else if (culprit->state() == state::Type::sling) {
                        m_sensed_objects.clear();
                        health_amount_add(-8.0f);
                        m_next_state = state::Type::stunned;
                        m_time_to_be_in_state = 20;                        
                    }
                    break;
                }
            }            

            /*sound_position("hurt", { position().x - app::config::extent().x / 2.0F,
                                       position().y - app::config::extent().y / 2.0F });
            sound_play("hurt");*/

            position_add(add_to_position);
            return true;
        }
        void update(cF32 dt) override {
            if (m_time_left_blocked > 0) --m_time_left_blocked;
            if (m_time_left_hurt > 0) --m_time_left_hurt;

            if (m_is_first_update) {
                m_is_first_update = false;
                sprite::is_leftward(m_sprite, random::number(0, 1) ? true : false);
            }

            if (velocity().x < -0.5F) {
                sprite::is_leftward(m_sprite, true);
            } else if (velocity().x > 0.5F) {
                sprite::is_leftward(m_sprite, false);
            }
            /*if (velocity().y < 0.0F and !m_is_on_slope) {
                m_is_on_ground = false;
            }*/
            deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

            state_update(dt);

            sprite::rect(m_sprite, anim::source(m_current_anim));

            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = state::Type::dead;
            }
            health::layer(m_health_id, m_start_layer);
        }
        void draw(std::unique_ptr<Window>& window) override {
            sprite::draw(window, m_sprite);
            line::draw(window, m_tounge_line);
        }
    };
}