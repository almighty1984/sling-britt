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
        void state_dive(cF32 dt)    override;
        void state_heal(cF32 dt)    override;
        void state_hurt(cF32 dt)    override;
        void state_idle(cF32 dt)    override;
        void state_jump(cF32 dt)    override;
        void state_melee(cF32 dt)   override;
        void state_stunned(cF32 dt) override;
        void state_swim(cF32 dt)    override;

        bool hurt(Object* culprit) override {
            if (m_time_left_hurt > 0) return false;
            m_time_left_hurt = m_config.time_to_hurt();

            if (!culprit or culprit->is_dead()) return false;

            m_time_left_until_next_state = 0;
            //m_time_left_in_next_state = 0;

            Vec2F add_to_position = { 0.0F, 0.0F };

            state::Type next_state = state::Type::hurt;

            console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");

            F32 vel_factor = 1.0F;

            switch (culprit->type()) {
                case Type::brick:
                case Type::bug:
                case Type::bee: {
                    m_sensed.clear();
                    m_sensed.emplace_back(culprit);

                    add_enemy(culprit);
                                        
                    //cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y) * 3.0F;
                    
                    health_amount_add(-96.0F);
                    //health::get(m_health_id)->amount -= 32.0F;

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
                    break;
                }
                case Type::particle_brick: {
                    m_sensed.clear();
                    m_sensed.emplace_back(culprit);

                    //console::log("amount: ", amount, "\n");

                    health_amount_add(-4.0F);
                    next_state = state::Type::hurt;
                    break;
                }
                case Type::particle_down_thrust: {
                    velocity_x(culprit->velocity().x * 0.7F);
                    if (m_is_on_ground) {
                        velocity_y(-2.0f);
                    }
                    next_state = state::Type::stunned;
                    break;
                }
                case Type::particle_melee: {                    
                    m_sensed.clear();
                    m_sensed.emplace_back(culprit->parent());
                    m_time_left_until_next_state = 0;
                    //m_time_left_in_next_state = 0;
                                        
                    health_amount_add(-8.0F);
                    
                    next_state = state::Type::hurt;
                    
                    //m_time_left_to_react += 10;

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
                    
                    break;
                }
                case Type::particle_rock: {
                    //m_time_left_hurt = 0;
                    if (culprit->parent()) {
                        add_enemy(culprit->parent());
                        console::log(class_name(), "::hurt() particle_rock parent: ", to_string(culprit->parent()->type()), "\n");
                    } else {
                        console::log(class_name(), "::hurt() parent is nullptr\n");
                    }
                    m_sensed.clear();
                    m_sensed.emplace_back(culprit->parent());

                    console::log(class_name(), "::hurt() rock velocity: ", culprit->velocity().x, " ", culprit->velocity().y, "\n");

                    next_state = state::Type::hurt;
                    health_amount_add(-8.0F);
                    break;
                }
                case Type::player: {
                    switch (culprit->state()) {
                        case state::Type::run: {
                            if (culprit->velocity().y >= 6.0F) {                        
                                next_state = state::Type::stunned;
                                m_time_to_be_in_state = 100;                                
                                //console::log("Frog::hurt() m_time_to_be_in_state: ", m_time_to_be_in_state, "\n");

                                sound_position("bounce", { position().x - app::config::extent().x / 2.0F,
                                                           position().y - app::config::extent().y / 2.0F });
                                sound_play("bounce");

                                particle::spawn(this, particle::Type::hit, position(), {});
                            }
                            break;
                        } 
                        case state::Type::sling: {
                            m_sensed.clear();

                            sound_position("hit", { position().x - app::config::extent().x / 2.0F,
                                                    position().y - app::config::extent().y / 2.0F });
                            sound_play("hit");

                            health_amount_add(-64.0f);
                            //next_state = state::Type::stunned;
                            //m_time_to_be_in_state = 20;
                            break;
                        }
                    }
                    break;
                }
                case Type::train_saw: {
                    health::amount_add(m_health_id, -16.0f);
                    break;
                }
            }

            /*sound_position("hurt", { position().x - app::config::extent().x / 2.0F,
                                       position().y - app::config::extent().y / 2.0F });
            sound_play("hurt");*/
            

            if (!(culprit->type() == Type::player and culprit->state() == state::Type::sling)) {
                cVec2F vel_normal = Vec2F::normalize(culprit->velocity());
                if (health_amount() <= 0.0F) {
                    vel_factor = 4.0F;
                }
                velocity(vel_normal * vel_factor);
            }

            if (health_amount() <= 0.0F) {
                max_velocity({ 10.0F, 10.0F });
                //velocity(culprit->velocity() * 1.0F);
                m_next_state = state::Type::dead;
                return true;
            }
            position_add(add_to_position);

            if (m_state != state::Type::swim) {
                m_next_state = next_state;
            }
            return true;
        }
        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
                sprite_is_leftward(random::number(0, 1) ? true : false);
            }

            if (velocity().x < -0.5F) {
                sprite_is_leftward(true);
            } else if (velocity().x > 0.5F) {
                sprite_is_leftward(false);
            }
            /*if (velocity().y < 0.0F and !m_is_on_slope) {
                m_is_on_ground = false;
            }*/
            //deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

            state_update(dt);

            //console::log(class_name(), "::update() state: ", to_string(m_state), "\n");

            //console::log(class_name(), "::update() time left alive: ", m_time_left_alive, "\n");

            sprite_rect(anim::source(m_current_anim));

            //if (health_amount() <= 0.0F) {
            //    m_next_state = state::Type::dead;
            //}
            health_layer(m_start_layer);
        }
        //void draw(std::unique_ptr<Window>& window) override {
        //    sprite::draw(window, m_sprite);
        //    line::draw(window, m_tounge_line);
        //}
    };
}