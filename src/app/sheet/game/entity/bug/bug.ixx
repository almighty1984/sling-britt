export module entity.bug;
import app.config;
import anim;
import console;
import entity;
import aabb;
import health;
import sound;
import transform;
import particle_system;
import random;
import sprite;
import types;

F32 added_degrees = 0.0F;

export namespace entity {
    class Bug : public Object {
        //U8 m_time_left_turning = 0;
    public:
        Bug() {
            m_state = m_next_state = m_start_state = state::Type::walk;
            m_config.time_to_interact(15);
        }
        const char* class_name() override { return "entity::Bug"; }
        bool hurt(entity::Object* culprit) override {
            if (!culprit or m_time_left_hurt > 0) {
                return false;
            }
            m_time_left_hurt = m_config.time_to_hurt();

            Vec2F add_to_position = { 0.0F, 0.0F };

            F32 vel_factor = 1.0F;

            /*if (m_state == state::Type::upended or m_state == state::Type::bounce) {
                switch (culprit->type()) {
                    case Type::brick:
                    case Type::player: {
                        console::log("entity::Bug::hurt player vel y: ", culprit->velocity().y, "\n");
                        sound_position("bounce", { position().x - app::config::extent().x / 2.0F,
                                                   position().y - app::config::extent().y / 2.0F });
                        sound_play("bounce");
                        return true;
                    }
                }
            }*/
            //m_state = state::Type::upended;
            m_next_state = state::Type::hurt;

            switch (culprit->type()) {
                case Type::brick:
                case Type::bug: {
                    //cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y) * 2.5F;
                    health_amount_add(-16.0F);

                    m_next_state = health_amount() > 0.0F ? state::Type::hurt : state::Type::dead;

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
                    break;
                }
                case Type::particle_melee: {
                    velocity_x(culprit->velocity().x * 0.1F + velocity().x * 0.5F);
                    velocity_y(-1.0F);
                    position_add_y(-2.0F);
                    health_amount_add(-16.0f);

                    if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  2.0F;
                    else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -2.0F;
                    if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  2.0F;
                    else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -2.0F;
                    break;
                }
                case Type::particle_rock: {
                    if (culprit->parent()) {
                        console::log(class_name(), "::hurt() particle_rock parent: ", to_string(culprit->parent()->type()), "\n");
                    } else {
                        console::log(class_name(), "::hurt() parent is nullptr\n");
                    }
                    m_sensed.clear();
                    m_sensed.emplace_back(culprit->parent());
                    m_next_state = state::Type::hurt;
                    health_amount_add(-16.0F);
                    break;
                }
                case Type::player: {
                    console::log("entity::Bug::hurt player vel y: ", culprit->velocity().y, "\n");
                    sound_position("bounce", { position().x - app::config::extent().x / 2.0F,
                                               position().y - app::config::extent().y / 2.0F });
                    sound_play("bounce");
                    break;
                }
                default: {
                    health_amount_add(-2.0F);
                    sound_position("hit", { position().x - app::config::extent().x / 2.0F,
                                            position().y - app::config::extent().y / 2.0F });
                    sound_play("hit");
                    break;
                }
            }
            sprite_is_leftward(!sprite_is_leftward());

            position_add(add_to_position);

            if (health_amount() <= 0.0F) {
                vel_factor = 4.0F;
                max_velocity({ 10.0F, 10.0F });
                cVec2F vel_normal = Vec2F::normalize(culprit->velocity());
                velocity(vel_normal * vel_factor);
                m_time_left_until_next_state = 0;
                m_next_state = state::Type::dead;
            }
            return true;
        }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_bounce(cF32 dt)  override;
        void state_carried(cF32 dt) override;
        void state_dead(cF32 dt)    override;
        void state_dive(cF32 dt)    override;
        void state_hurt(cF32 dt)    override;
        void state_swim(cF32 dt)    override;
        void state_tossed(cF32 dt)  override;
        void state_upended(cF32 dt) override;
        void state_walk(cF32 dt)    override;

        void update(cF32 dt) override {
            reduce_time_left(1);

            if (m_is_first_update) {
                m_is_first_update = false;
            }
            if (!m_is_on_slope and (velocity().y < 0.0F or velocity().y > acceleration().y)) {
                m_is_on_ground = false;
            }
            if (m_state == state::Type::upended) {
                //console::log("entity::Bug::update is_near_wall: ", m_is_near_wall_L, " ", m_is_near_wall_R, "\n");
            }
            if (velocity().x <= 0.0F) {
                m_is_near_wall_R = false;
            }
            if (velocity().x >= 0.0F) {
                m_is_near_wall_L = false;
            }
            //console::log("state: ", entity::to_string(m_state), " ", m_is_on_ground, "\n");


            if (!is_dead()) {
                velocity_add_y(acceleration().y);
            }

            state_update(dt);

            sprite_rect(anim::source(m_current_anim));

            //if (health_amount() <= 0.0F) {
            //    m_next_state = state::Type::dead;
            //}
            health_layer(m_start_layer);
        }
    };
}