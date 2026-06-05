export module entity.train;
import app.config;
import aabb;
import anim;
import camera;
import console;
import entity;
import std;

export namespace entity {
    class Train : public Object {
        F32 m_speed       = 0.0F,
            m_start_speed = 1.0F;

        Vec2F m_prev_velocity = { 0.0F, 0.0F };
    public:
        const char* class_name() override { return "entity::Train"; }

        //Train() {
            //m_speed = m_start_speed;
        //}
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;
        
        void update(cF32 dt) override {
            if (m_is_first_update) {
                m_is_first_update = false;
                //m_start_speed = -1.0F;
                //velocity_x(m_start_speed);
                //velocity({});
                m_prev_velocity.x = m_start_speed;
            }

            if (m_time_left_colliding > 0) --m_time_left_colliding;
            //console::log("train position: ", position().x, " ", position().y, "\n");
            //console::log("is_powered: ", m_is_powered, "\n");
            //console::log("time_left_alive: ", m_time_left_alive, "\n");

            //console::log("speed: ", m_speed, "\n");

            /*if (m_is_powered) {
                m_time_left_alive = U16_MAX;
            } else {
                m_time_left_alive = 0;
            }*/


            direction_x(0.0F);
            if (velocity().x < 0.0F) {
                direction_x(-1.0F);
            } else if (velocity().x > 0.0F) {
                direction_x(1.0F);
            }
            direction_y(0.0F);
            if (velocity().y < 0.0F) {
                direction_y(-1.0F);
            } else if (velocity().y > 0.0F) {
                direction_y(1.0F);
            }

            //console::log("rotation acc: ", m_config.rotation_acc(), "\n");

            //console::log(class_name(), "::update() direction: ", direction().x, " ", direction().y, "\n");
            
            if (velocity().x < -0.05F or velocity().x > 0.05F) {
                m_prev_velocity.x = velocity().x;
            }
            if (velocity().y < -0.05F or velocity().y > 0.05F) {
                m_prev_velocity.y = velocity().y;
            }

            if (m_time_left_alive > 0) {
                if (m_start_speed > 0.0F and m_speed < m_start_speed) {
                    m_speed += 0.2F;
                } else if (m_start_speed < 0.0F and m_speed > m_start_speed) {
                    m_speed -= 0.2F;
                }
                if (m_start_speed > 0.0F and m_speed > m_start_speed or
                    m_start_speed < 0.0F and m_speed < m_start_speed) {
                    m_speed = m_start_speed;
                }
                if (velocity().x >= -0.05F and velocity().x <= 0.05F and
                    velocity().y >= -0.05F and velocity().y <= 0.05F) {
                    console::log("entity::Train at zero\n");
                    velocity(m_prev_velocity);
                }


                cVec2F sound_pos = { (position().x + sprite_rect().w / 2.0F) - app::config::extent().x / 2.0F,
                                     (position().y + sprite_rect().h / 2.0F) - app::config::extent().y / 2.0F };

                //console::log(class_name(), "::update() sound position: ", sound_pos.x, " ", sound_pos.y, "\n");
                sound_position("run", sound_pos);
                sound_is_looped("run", true);

                if (!sound_is_playing("run")) {                    
                    sound_play("run");
                }

                if (m_config.is_rotation_clockwise()) {
                    if (m_rotation_speed < m_config.rotation_speed_limit()) {
                        m_rotation_speed += m_config.rotation_acc();
                    }
                }
                else {
                    if (m_rotation_speed > -m_config.rotation_speed_limit()) {
                        m_rotation_speed -= m_config.rotation_acc();
                    }
                }
            } else {
                //direction({});
                m_speed *= 0.5F;
                //velocity(velocity() * 0.5F);

                if (m_speed > 0.0F and m_speed < 0.01F) {
                    m_speed = 0.0F;          
                } else if (m_speed < 0.0F and m_speed > -0.01F) {
                    m_speed = 0.0F;
                }
                if (sound_is_playing("run")) {
                    sound_pause("run");
                }
                
                if (m_rotation_speed < 0.0F) {
                    m_rotation_speed += m_config.rotation_dec();
                } else if (m_rotation_speed > 0.0F) {
                    m_rotation_speed -= m_config.rotation_dec();
                }

                if (m_rotation_speed >= -m_config.rotation_acc() and m_rotation_speed <= m_config.rotation_acc()) {
                    m_rotation_speed = 0.0F;
                }
            }

            //console::log(class_name(), "::update() rotation is clockwise: ", m_config.is_rotation_clockwise(), "\n");

            //console::log(class_name(), "::update() ", to_string(m_type), " rotation speed: ", m_rotation_speed, "\n");
            sprite_angle_add(m_rotation_speed);
            //console::log("speed: ", m_speed, "\n");
            
            /*if (!m_is_second_update) {
                m_is_second_update = true;
                for (auto& i : m_aabbs) {
                    console::log("aabb ", i, " name: ", aabb::name_as_string(aabb::get(i)->name), "\n");
                }
            }*/
            
            

            //velocity_x(m_speed.x;
            //velocity_y(m_speed.y;
            m_radians = sprite_angle() * (PI / 180.0F);

            for (auto& i : m_aabbs) {
                if (aabb::name(i) != aabb::Name::body) continue;

                cF32 pos_x = aabb::start_rect(i).x - aabb::start_rect(i).w / 2.0F;
                cF32 pos_y = aabb::start_rect(i).y - aabb::start_rect(i).h / 2.0F;

                cF32 cos = std::cosf(m_radians);
                cF32 sin = std::sinf(m_radians);
                //i->offsetX(cos + i->originX() - i->w() / 2.0f);
                //i->offsetY(sin + i->originY() - i->h() / 2.0f);

                aabb::rect_x(i, pos_x * cos - pos_y * sin + aabb::start_rect(i).w / 2.0F);
                aabb::rect_y(i, pos_y * cos + pos_x * sin + aabb::start_rect(i).h / 2.0F);

                //console::log(aabb::start_rect(i).x, " ", aabb::start_rect(i).y, "\n");
            }
        }
    };
}