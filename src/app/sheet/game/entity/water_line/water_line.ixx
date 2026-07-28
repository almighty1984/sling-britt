export module entity.water_line;
import anim;
import console;
import entity;
import aabb;
import line;
import random;
import std;

export namespace entity {
    class WaterLine : public Object {
        F32 m_line_offset_y   =  1.0F;
        I32 m_top_line     = -1,
            m_bottom_line  = -1;
        U8 m_wave_timer       =  0,
           m_top_line_size    =  1,
           m_bottom_line_size =  3;

    public:
        WaterLine() {
            m_input_limit = 2;
        }
        ~WaterLine() {            
            line::erase(m_top_line);
            line::erase(m_bottom_line);
        }

        const char* class_name() override { return "entity::WaterLine"; }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        void update(cF32 dt) override {
            if (m_inputs.empty()) return;

            if (m_is_first_update) {
                m_is_first_update = false;

                velocity_y(random::real_number(-0.1, 0.1));

                deceleration({ 100.0F, 0.0F });

                //console::log("water input: ", m_inputs.size(), "\n");

                start_offset(position_on_level());

                m_bottom_line = line::make(position() + Vec2F{ 0.0F, 4.0F },
                                           m_inputs.back()->position() + Vec2F{ 0.0F,
                                           m_line_offset_y });
                line::size(m_bottom_line, m_bottom_line_size);
                line::color(m_bottom_line, { 52, 206, 206 });

                line::layer(m_bottom_line, m_start_layer - 1);
                //line::is_hidden(m_bottom_line, true);

                m_top_line = line::make(position() + Vec2F{ 0.0F, 0.0F },
                                        m_inputs.back()->position() + Vec2F{ 0.0F,
                                        m_line_offset_y });                
                line::size(m_top_line, m_top_line_size);
                line::color(m_top_line, { 52, 255, 255 });
                line::layer(m_top_line, m_start_layer);
            }

            cF32 diff_y = position_on_level().y - start_position_on_level().y;
            velocity_add({ 0.0F, -diff_y * 0.025F });

            //console::log("entity::WaterLine::update() diff_y: ", diff_y, "\n");

            /*aabb::rect(m_aabbs.back(), RectF{ aabb::rect(m_aabbs.back()).x,
                                                 -diff_y,
                                                 aabb::rect(m_aabbs.back()).w,
                                                 aabb::rect(m_aabbs.back()).h });*/



            if (std::abs(velocity().y) > 0.05F) {
                velocity_y(velocity().y * 0.93f);
            } else {
                /*if (diff_y < 0.0F and diff_y > -0.04f) {
                    diff_y = -0.04f;
                } else if (diff_y >= 0.0F and diff_y < 0.04f) {
                    diff_y = 0.04f;
                }*/
            }

            //if (m_force.x > -0.1F and m_force.x < 0.1F) {
                //m_force.x = 0.0F;
            //}
            /*if (inputs.size() == 2) {
                if (m_inputs.front()->force().x > 0.0F) {
                    m_force.x += m_inputs.front()->force().x * 0.75F;
                }
                if (m_inputs.back()->force().x < 0.0F) {
                    m_force.x += m_inputs.back()->force().x * 0.75F;
                }

                m_force.x *= 0.5F;
            }*/

            // make left aabb red, right blue, for now
            if (m_wave_timer < 2) {
                ++m_wave_timer;
            } else {
                m_wave_timer = 0;
                switch (m_type) {
                case Type::water_line: {
                    if (m_inputs.size() == 2) {
                        //m_force.x *= 0.4F;

                        //if (m_inputs.front()->force().x > 0.0F) {
                        //    m_force.x += m_inputs.front()->force().x * 0.3F;
                        //}
                        //if (m_inputs.back()->force().x < 0.0F) {
                            //m_force.x += m_inputs.back()->force().x * 0.3F;
                        //}
                        //if      (m_force.x < -1.0F) m_force.x = -1.0F;
                        //else if (m_force.x >  1.0F) m_force.x =  1.0F;
                        m_force.x = m_inputs.front()->force().x * 0.4F + m_inputs.back()->force().x * 0.4F + m_force.x * 0.2F;

                    }
                    break;
                }
                case Type::water_line_L: {
                    if (m_inputs.back()->force().x < 0.0F) {
                        //m_inputs.back()->force_x(-m_inputs.back()->force().x * 1.0F);
                        //m_force.x = m_inputs.back()->force().x;
                    }
                    //m_force.x *= 0.5F;

                    m_force.x = m_inputs.back()->force().x * 0.8F + m_force.x * 0.2F;
                    break;
                }
                case Type::water_line_R: {
                    if (m_inputs.back()->force().x > 0.0F) {
                        //m_inputs.back()->force_x(-m_inputs.back()->force().x * 1.0F);
                        //m_force.x = m_inputs.back()->force().x;
                    }
                    //m_force.x *= 0.5F;
                    m_force.x = m_inputs.back()->force().x * 0.8F + m_force.x * 0.2F;
                    if (m_inputs.size() == 1) {
                        //if (m_inputs.back()->force().x < 0.0F and m_inputs.back()->position().x > position().x) {
                        //    m_force.x -= m_inputs.back()->force().x * 10.0F;
                        //}
                        //else if (m_inputs.back()->force().x > 0.0F and m_inputs.back()->position().x < position().x) {
                        //    m_force.x -= m_inputs.back()->force().x * 10.0F;
                        //}
                        //// high value so wave bounces back
                        //if      (m_force.x < -5.0F) m_force.x = -5.0F;
                        //else if (m_force.x >  5.0F) m_force.x =  5.0F;
                    }
                    break;
                }
                }

                //m_force.x *= 0.99F;

                //m_force.x *= 0.9F;
                if (std::abs(velocity().y) > std::abs(m_inputs.front()->velocity().y)) {
                    //m_inputs.front()->velocity_y(velocity().y * 0.95f + m_inputs.front()->velocity().y * 0.1F;
                    m_inputs.front()->velocity_y(velocity().y * 0.99F);
                }
                if (std::abs(velocity().y) > std::abs(m_inputs.back()->velocity().y)) {
                    //m_inputs.back()->velocity_y(velocity().y * 0.95f + m_inputs.back()->velocity().y * 0.1F;
                    m_inputs.back()->velocity_y(velocity().y * 0.99F);
                }
            }


            if (m_force.x < -1.0F) m_force.x = -1.0F;
            if (m_force.x > 1.0F)  m_force.x = 1.0F;

            if (m_force.x > -0.01F and m_force.x < 0.01F) {
                m_force.x = 0.0F;
            }

            if (m_force.x != 0.0F) {
                F32 amount = std::abs(m_force.x);
                if (amount > 0.0F) {
                    if (amount > 1.0F) amount = 1.0F;

                    amount = 1.0F / amount;
                    //console::log(class_name(), "::update()) color amount: ", amount, "\n");
                    cU8 color_amount = (U8)amount;

                    if (color_amount >= 0) {
                        Color aabb_color = aabb::start_color(m_aabbs.back());
                        if (m_force.x < 0.0F) {
                            aabb_color = { 255,   0,   0 };
                        } else if (m_force.x > 0.0F) {
                            aabb_color = { 0,   0, 255 };
                        }

                        aabb::color(m_aabbs.back(), aabb_color / color_amount);
                    }
                }
            }

            if (m_type == Type::water_line_L) {
                aabb::color(m_aabbs.back(), { 0,   0, 255 });
            } else if (m_type == Type::water_line_R) {
                aabb::color(m_aabbs.back(), { 255,   0,   0 });
            }

            /*if (m_type == Type::water_line_L and m_number == 0 or
                m_type == Type::water_line_R and m_number == 3) {
                aabb::color(m_aabbs.back(), { 255, 255, 255 });
            }*/




            cF32 add_to_length_x = 0.1F;
            //cF32 size_offset = m_top_line_size / 4.0F;
            //cF32 size_offset = 0.5F;

            Vec2F start{}, end{};
            if (m_inputs.size() == 1) {
                if (position().x > m_inputs.back()->position().x) {
                    // R
                    start = m_inputs.back()->position() + Vec2F{ -add_to_length_x, m_line_offset_y };
                    end   = position() + Vec2F{ 4.0F + add_to_length_x, m_line_offset_y };
                } else {
                    // L
                    start = m_inputs.back()->position() + Vec2F{ add_to_length_x, m_line_offset_y };
                    end   = position() + Vec2F{ -add_to_length_x, m_line_offset_y };
                }
            } else if (m_inputs.size() == 2) {
                start = m_inputs.front()->position() + Vec2F{ -add_to_length_x, m_line_offset_y };
                end   = m_inputs.back()->position()  + Vec2F{  add_to_length_x, m_line_offset_y };
            }

            line::set(m_top_line, start, end);
            line::set(m_bottom_line, start + Vec2F{ 0.0F, (F32)m_top_line_size + 0.0F },
                                     end   + Vec2F{ 0.0F, (F32)m_top_line_size + 0.0F });

            Vec2F l = { line::end(m_top_line).x - line::start(m_top_line).x,
                        line::end(m_top_line).y > line::start(m_top_line).y ? line::end(m_top_line).y - line::start(m_top_line).y :
                                                                              line::start(m_top_line).y - line::end(m_top_line).y
            };
            m_radians = std::atan(l.x / l.y);
            if (m_radians < 0.0F) {
                m_radians += PI * 2.0F;
            }

            /* m_radians = std::atan(line::delta(m_top_line).x / line::delta(m_top_line).y);
             if (m_radians < 0.0F) {
                 m_radians += 3.1415926535F * 2.0F;
             }*/

        }
    };
}