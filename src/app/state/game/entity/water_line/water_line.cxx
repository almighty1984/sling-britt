module entity.water_line;

namespace entity {
    void WaterLine::collide_x(aabb::cInfo our, aabb::cInfo other) {
        //if (other.owner and other.owner->type() == Type::player) {
        collide_y(our, other);
        //}
    }
    void WaterLine::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner) return;

        cType other_type = other.owner->type();

        if (time_left_colliding_with(other_type) > 0) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        if (other_type != Type::player and
            other_type != Type::bug and
            other_type != Type::mole) {
            return;
        }

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();


        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);
        if (other_type == Type::bug) {
            if (m_type == Type::water_line_L or m_type == Type::water_line_R) return;

            other.owner->velocity_x(m_force.x * 0.1F + other_velocity.x * 0.9F);
            m_force.x = m_force.x * 0.5F + other_velocity.x * 0.5F;


            if (std::abs(other_velocity.y) > 0.2F) {
                velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
        }
        else if (other_type == Type::mole) {
            if (other.owner->state() == state::Type::swim) {
                if (m_type == Type::water_line_L or m_type == Type::water_line_R) return;

                //if (other.owner->time_left_colliding_with(Type::water_line) > 0) {
                //    m_force.x = other_velocity.x * 0.5F;
                //    //if (m_force.x < -1.0F) m_force.x = -1.0F;
                //    //if (m_force.x >  1.0F) m_force.x =  1.0F;
                //    return;
                //}

                other.owner->velocity_x(m_force.x * 0.1F + other_velocity.x * 0.9F);
                m_force.x = m_force.x * 0.5F + other_velocity.x * 0.5F;

                if (std::abs(other_velocity.y) > 0.2F) {
                    velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                }
            }
        }
        else if (other_type == Type::player) {
            cF32 start_diff_y = start_offset().y - position_on_level().y;
            if (std::abs(start_diff_y) > 8.0F or std::abs(other_velocity.y) < 1.0F) {
                return;
            }
            //console::log("start_diff_y: ", start_diff_y, "\n");

            //if (velocity().y < 0.0F)
                //velocity_y(other.owner->velocity().y * 1.0F;
            //else
            velocity_y(other.owner->velocity().y * 0.3F);

            //console::log("WaterLine::collide_y velocity y: ", velocity().y, "\n");

        //if (std::abs(start_diff_y) < 8.0F/* and std::abs(other.owner->velocity().y) > 0.5F*/) {
            //velocity_y(other.owner->velocity().y * 0.9F;
            //moved_velocity_y(other.owner->velocity().y * 1.0F;
        //}
            //velocity_y(other.owner->velocity().y / std::abs(start_diff_y);
            //velocity_y(other.owner->velocity().y * 1.0F;



            /*if (inputs().front()) {
                inputs().front()->velocity_y(velocity().y * 0.3F + inputs().front()->velocity().y * 0.5F;
                if (inputs().front()->inputs().front()) {
                    inputs().front()->inputs().front()->velocity_y(inputs().front()->velocity().y;
                }
            }
            if (inputs().back()) {
                inputs().back()->velocity_y(velocity().y * 0.3F + inputs().back()->velocity().y * 0.5F;
                if (inputs().back()->inputs().back()) {
                    inputs().back()->inputs().back()->velocity_y(inputs().back()->velocity().y;
                }
            }*/


            //}
        }
    }
    void WaterLine::update() {
        if (m_input_objects.empty()) return;

        if (m_is_first_update) {
            m_is_first_update = false;

            velocity_y(random::real_number(-0.1, 0.1));

            deceleration({ 100.0F, 0.0F });

            //console::log("water input: ", m_input_objects.size(), "\n");

            start_offset(position_on_level());

            m_bottom_line_id = line::make(position() + Vec2F{ 0.0F, 4.0F },
                                          m_input_objects.back()->position() + Vec2F{ 0.0F,
                                          m_line_offset_y });
            line::size(m_bottom_line_id, m_bottom_line_size);
            line::color(m_bottom_line_id, { 52, 206, 206 });

            line::layer(m_bottom_line_id, m_start_layer);

            m_top_line_id = line::make(position() + Vec2F{ 0.0F, 2.0F },
                                       m_input_objects.back()->position() + Vec2F{ 0.0F,
                                       m_line_offset_y });
            line::size(m_top_line_id, m_top_line_size);
            line::color(m_top_line_id, { 52, 255, 255 });
            line::layer(m_top_line_id, m_start_layer);
        }

        cF32 diff_y = position_on_level().y - start_position_on_level().y;
        velocity_add({ 0.0F, -diff_y * 0.025f });

        //console::log("entity::WaterLine::update() diff_y: ", diff_y, "\n");

        /*aabb::rect(m_aabb_ids.back(), RectF{ aabb::rect(m_aabb_ids.back()).x,
                                             -diff_y,
                                             aabb::rect(m_aabb_ids.back()).w,
                                             aabb::rect(m_aabb_ids.back()).h });*/



        if (std::abs(velocity().y) > 0.05F) {
            velocity_y(velocity().y * 0.93f);
        }
        else {
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
            if (m_input_objects.front()->force().x > 0.0F) {
                m_force.x += m_input_objects.front()->force().x * 0.75F;
            }
            if (m_input_objects.back()->force().x < 0.0F) {
                m_force.x += m_input_objects.back()->force().x * 0.75F;
            }

            m_force.x *= 0.5F;
        }*/
        
        // make left aabb red, right blue, for now
        if (m_wave_timer < 2) {
            ++m_wave_timer;
        }
        else {
            m_wave_timer = 0;
            if (m_type == Type::water_line) {
                if (m_input_objects.size() == 2) {
                    //m_force.x *= 0.4F;

                    //if (m_input_objects.front()->force().x > 0.0F) {
                    //    m_force.x += m_input_objects.front()->force().x * 0.3F;
                    //}
                    //if (m_input_objects.back()->force().x < 0.0F) {
                        //m_force.x += m_input_objects.back()->force().x * 0.3F;
                    //}
                    //if      (m_force.x < -1.0F) m_force.x = -1.0F;
                    //else if (m_force.x >  1.0F) m_force.x =  1.0F;
                    m_force.x = m_input_objects.front()->force().x * 0.4F + m_input_objects.back()->force().x * 0.4F + m_force.x * 0.2F;

                }
            }            
            else if (m_type == Type::water_line_L) {
                if (m_input_objects.back()->force().x < 0.0F) {
                    //m_input_objects.back()->force_x(-m_input_objects.back()->force().x * 1.0F);
                    //m_force.x = m_input_objects.back()->force().x;
                }
                //m_force.x *= 0.5F;

                m_force.x = m_input_objects.back()->force().x * 0.8F + m_force.x * 0.2F;
            } 
            else if (m_type == Type::water_line_R) {
                if (m_input_objects.back()->force().x > 0.0F) {
                    //m_input_objects.back()->force_x(-m_input_objects.back()->force().x * 1.0F);
                    //m_force.x = m_input_objects.back()->force().x;
                }
                //m_force.x *= 0.5F;
                m_force.x = m_input_objects.back()->force().x * 0.8F + m_force.x * 0.2F;
                if (m_input_objects.size() == 1) {
                    //if (m_input_objects.back()->force().x < 0.0F and m_input_objects.back()->position().x > position().x) {
                    //    m_force.x -= m_input_objects.back()->force().x * 10.0F;
                    //}
                    //else if (m_input_objects.back()->force().x > 0.0F and m_input_objects.back()->position().x < position().x) {
                    //    m_force.x -= m_input_objects.back()->force().x * 10.0F;
                    //}
                    //// high value so wave bounces back
                    //if      (m_force.x < -5.0F) m_force.x = -5.0F;
                    //else if (m_force.x >  5.0F) m_force.x =  5.0F;
                }
            }
            //m_force.x *= 0.99F;

            //m_force.x *= 0.9F;
            if (std::abs(velocity().y) > std::abs(m_input_objects.front()->velocity().y)) {
                //m_input_objects.front()->velocity_y(velocity().y * 0.95f + m_input_objects.front()->velocity().y * 0.1F;
                m_input_objects.front()->velocity_y(velocity().y * 0.99F);
            }
            if (std::abs(velocity().y) > std::abs(m_input_objects.back()->velocity().y)) {
                //m_input_objects.back()->velocity_y(velocity().y * 0.95f + m_input_objects.back()->velocity().y * 0.1F;
                m_input_objects.back()->velocity_y(velocity().y * 0.99F);
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
                    Color aabb_color = aabb::start_color(m_aabb_ids.back());
                    if (m_force.x < 0.0F) {
                        aabb_color = { 255,   0,   0 };
                    }
                    else if (m_force.x > 0.0F) {
                        aabb_color = {   0,   0, 255 };
                    }

                    aabb::color(m_aabb_ids.back(), aabb_color / color_amount);
                }
            }
        }

        if (m_type == Type::water_line_L) {
            aabb::color(m_aabb_ids.back(), {   0,   0, 255 });
        }
        else if (m_type == Type::water_line_R) {
            aabb::color(m_aabb_ids.back(), { 255,   0,   0 });
        }

        /*if (m_type == Type::water_line_L and m_number == 0 or 
            m_type == Type::water_line_R and m_number == 3) {
            aabb::color(m_aabb_ids.back(), { 255, 255, 255 });
        }*/




        cF32 half_w = 1.0F;
        cF32 size_offset = m_top_line_size / 4.0F;

        Vec2F start = position(),
            end = position() + Vec2F{ 0.0F, -8.0F };
        if (m_input_objects.size() == 1) {
            if (position().x > m_input_objects.back()->position().x) {
                // R
                start = m_input_objects.back()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
                end   = position() + Vec2F{ half_w * 2 + size_offset + 1.5F, m_line_offset_y };
            }
            else {
                // L
                start = position() + Vec2F{ -0.1F, m_line_offset_y };
                end   = m_input_objects.back()->position() + Vec2F{ half_w + size_offset, m_line_offset_y };
            }
        }
        else if (m_input_objects.size() == 2) {

            start = m_input_objects.front()->position() + Vec2F{ half_w - size_offset, m_line_offset_y };
            end   = m_input_objects.back()->position() + Vec2F{ half_w + size_offset, m_line_offset_y };
        }

        line::set(m_top_line_id, start, end);
        line::set(m_bottom_line_id, start + Vec2F{ 0.0F, size_offset * m_top_line_size },
                                    end + Vec2F{ 0.0F, size_offset * m_top_line_size });


        Vec2F l;
        l.x = line::end(m_top_line_id).x - line::start(m_top_line_id).x;
        if (line::end(m_top_line_id).y > line::start(m_top_line_id).y) {
            l.y = line::end(m_top_line_id).y - line::start(m_top_line_id).y;
        }
        else {
            l.y = line::start(m_top_line_id).y - line::end(m_top_line_id).y;
        }
        m_radians = std::atan(l.x / l.y);
        if (m_radians < 0.0F) {
            m_radians += 3.1415926535F * 2.0F;
        }
        
       /* m_radians = std::atan(line::delta(m_top_line_id).x / line::delta(m_top_line_id).y);
        if (m_radians < 0.0F) {
            m_radians += 3.1415926535F * 2.0F;
        }*/

    }
}