export module entity.bug;
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
            m_time_to_interact = 15;
        }
        const char* class_name() override { return "entity::Bug"; }
        bool hurt(entity::Object* culprit) override;

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void state_bounce()  override;
        void state_carried() override;
        void state_dead()    override;
        void state_hurt()    override;
        void state_swim()    override;
        void state_tossed()  override;
        void state_upended() override;
        void state_walk()    override;

        void update() override {
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

            if (m_time_left_hurt > 0) --m_time_left_hurt;
            //if (m_time_left_turning     > 0) --m_time_left_turning;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            for (auto& i : m_time_left_colliding_with) {
                if (i.second > 0) {
                    --i.second;
                }
            }

            if (!is_dead()) {
                velocity_add_y(acceleration().y);
            }

            state_update();

            sprite::rect(m_sprite, anim::source(m_current_anim));

            if (health::amount(m_health_id) <= 0.0f) {
                m_next_state = state::Type::dead;
            }
            health::layer(m_health_id, m_start_layer);
        }
    };
}