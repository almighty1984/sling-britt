export module entity.bee;
import app.config;
import aabb;
import anim;
import camera;
import console;
import entity;
import particle_system;
import sound;
import sprite;
import random;

export namespace entity {
    class Bee : public Object {
        Vec2F m_target = {};
        Vec2F m_range = { 16.0F, 1.0F };
        Vec2I m_timer = {};
        F32 m_idle_pitch = 1.0F;
    public:
        Bee() {
            m_state = m_next_state = m_start_state = state::Type::idle;
            m_type = Type::bee;
            m_idle_pitch = 1.0F + random::number(-5, 5) / 100.0F;
        }        
        const char* class_name() override { return "entity::Bee"; }

        bool hurt(Object* culprit) override {
            if (m_time_left_hurt > 0) return false;
            //m_time_left_hurt = m_time_to_hurt;

            if (!culprit) return false;

            switch (culprit->type()) {
                case Type::brick:
                case Type::particle_rock:
                case Type::particle_melee: {
                    velocity(culprit->velocity() * 0.9F);
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

        void state_idle(cF32 dt) override {
            Vec2F acc = { 0.05F, 0.05F };
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                m_time_to_be_in_state = 60 + random::number(0, 20);
                console::log(class_name(), "::state_idle() time to be in state: ", m_time_to_be_in_state, "\n");
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, true);
                }
                sprite_is_hidden(false);
                sprite_angle(0.0F);
                sprite_is_upended(false);
                console::log(class_name(), "::state_idle()\n");
                m_time_in_state = 0;
                m_sensed_position = {};
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;
                set_anim("idle");
                m_sensed_objects.clear();
                acceleration(acc);                

                m_target.y = start_position_on_level().y + 16.0F;
                m_target.x = start_position_on_level().x + 16.0F;

                max_velocity({ 1.0F, 1.0F });
                sound_pitch("idle", m_idle_pitch);
            }
            sound_position("idle", { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F });

            sound_is_looped("idle", true);
            if (!sound_is_playing("idle")) {     
                sound_play("idle");
            }

            ++m_time_in_state;
            //m_radians += dt;
            //console::log("dt: ", std::asinf(m_radians), "\n");
            
            //console::log("time: ", m_time_in_state, "\n");
            
            if (position_on_level().x <= start_position_on_level().x - m_range.x) {
                m_target.x = start_position_on_level().x + m_range.x;
                //console::log("target right\n");
            } else if (position_on_level().x >= start_position_on_level().x + m_range.x) {
                m_target.x = start_position_on_level().x - m_range.x;
                //console::log("target left\n");
            }
            if (m_target.x < position_on_level().x) {
                acceleration_x(-acc.x);
            } else if (m_target.x >= position_on_level().x) {
                acceleration_x(acc.x);
            }

            if (position_on_level().y <= start_position_on_level().y - m_range.y) {
                m_target.y = start_position_on_level().y + m_range.y;
                //console::log("target down\n");
            } else if (position_on_level().y >= start_position_on_level().y + m_range.y) {
                m_target.y = start_position_on_level().y - m_range.y;
                //console::log("target up\n");
            }
            if (m_target.y < position_on_level().y) {
                acceleration_y(-acc.y);
            } else if (m_target.y >= position_on_level().y) {
                acceleration_y(acc.y);
            }
            //console::log(position_on_level().x, " ", position_on_level().y, " ", start_position_on_level().x, " ", start_position_on_level().y, "\n");

            //console::log("acc: ", acceleration().x, " ", acceleration().y, "\n");
            velocity_add(acceleration());

            sprite_is_leftward(velocity().x < 0.0F);

            /*F32 radians = std::atan2(velocity().y, velocity().x);
            if (radians < 0.0F) radians += PI * 2.0F;
            cF32 degrees = radians * 180.0F / PI;
            sprite_angle(degrees + 0.0F);
            sprite_is_upended(velocity().x < 0.0F);*/            

            //return;

            if (m_time_left_to_react > 0) {
                --m_time_left_to_react;
                if (m_time_left_to_react == 0) {
                    m_next_state = state::Type::charge;
                }
                return;
            }

            if (m_time_in_state < m_time_to_be_in_state) {
                return;
            }

            if (m_time_left_to_spawn_sense > 0) {
                --m_time_left_to_spawn_sense;
                if (m_time_left_to_spawn_sense == 0) {
                    //console::log("Frog::idle() spawn sense\n");

                    m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                    particle::spawn_fan(this, 0.0F, 360.0F, 32,
                                        particle::Type::sense,
                                        position() + Vec2F{ 4.0F, 0.0F }, {}, 4.0F);
                }
            }

            if (m_sensed_objects.empty()) return;
            for (auto& i : m_sensed_objects) {
                if (!i or i->is_dead()) continue;
                if (i->type() == Type::player) {
                    console::log(class_name(), "::state_idle() sensed player\n");
                    //m_sensed_state = state::Type::attack;
                    m_sensed_position = i->position() + Vec2F{8.0F, 8.0F};
                    m_time_left_to_react = 1;
                    m_sensed_objects.clear();
                    return;
                }
            }
        }
        void state_charge(cF32 dt) override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sound_stop("idle");
                console::log(class_name(), "::state_charge()\n");
                m_time_in_state = 0;
                reset_anim("charge");
                sprite_angle(0.0F);
                sprite_is_upended(false);
                
                sprite_is_leftward(m_sensed_position.x < position().x);                
            }
            ++m_time_in_state;

            velocity({});

            if (m_time_in_state > 20) {
                m_time_in_state = 0;
                m_next_state = state::Type::attack;
            }
        }
        void state_attack(cF32 dt) override {
            Vec2F vel = m_sensed_position - position();
            sound_position("attack", { position().x - app::config::extent().x / 2.0F,
                                       position().y - app::config::extent().y / 2.0F });
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                console::log(class_name(), "::state_attack()\n");
                m_time_in_state = 0;
                reset_anim("attack");                
                console::log("vel vec: ", vel.x, " ", vel.y, "\n");
                max_velocity({ 10.0F, 10.0F });

                vel.normalize();
                velocity(vel * 5.0F);

                sound_play("attack");
                sound_pitch("attack", 1.0F + random::number(-2, 2) / 100.0F);
            }            
            ++m_time_in_state;

            //sprite_is_leftward(velocity().x < 0.0F);
            sprite_is_leftward(false);
            sprite_is_upended(velocity().x < 0.0F);

            F32 radians = std::atan2(velocity().y, velocity().x);
            if (radians < 0.0F) radians += PI * 2.0F;
            cF32 degrees = radians * 180.0F / PI;
            sprite_angle(degrees + 0.0F);
            

            if (m_time_in_state > 20) {
                m_time_in_state = 0;
                m_next_state = state::Type::idle;
            }
        }

        void state_dead(cF32 dt) override {
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                sound_stop("idle");
                //m_time_left_dead = m_time_to_be_dead;
                sprite_is_hidden(true);
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }
                Vec2F blood_vel = velocity() + moved_velocity();
                particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                    particle::Type::drop_blood,
                                    position() + Vec2F{ 6.0F, -4.0F },
                                    blood_vel * 1.0F, 1.0F,
                                    state::Type::idle);
                console::log(class_name(), "::state_dead() blood vel: ", blood_vel.x, " ", blood_vel.y, "\n");

                particle::spawn(this,
                                particle::Type::health,
                                position() + Vec2F{ 4.0F, 0.0F }, {});

                sound_position("dead", { position().x - app::config::extent().x / 2.0F,
                                         position().y - app::config::extent().y / 2.0F });
                sound_play("dead");

                m_sensed_objects.clear();
            }

            velocity(velocity() * 0.9F);
            moved_velocity({});

            if (m_time_left_dead > 0) {
                --m_time_left_dead;
                if (m_time_left_dead == 0) {
                    console::log("Frog::dead() done being dead\n");
                }
            }
        }

        void update(cF32 dt) override {
            if (m_is_first_update) {
                m_is_first_update = false;
                reset_anim("idle");
            }

            if (m_time_left_hurt > 0) --m_time_left_hurt;
            if (m_time_left_interacting > 0) --m_time_left_interacting;

            if (m_next_state != m_state) {
                m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;
            }

            state_update(dt);

            sprite::rect(m_sprite, anim::source(m_current_anim));
        }
    };
}