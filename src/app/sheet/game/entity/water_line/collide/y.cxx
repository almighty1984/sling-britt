module entity.water_line;

namespace entity {
    void WaterLine::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner) return;

        cType other_type = other.owner->type();

        if (time_left_colliding_with(other_type) > 0) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F our_center = aabb::center(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);
        cVec2F other_center = aabb::center(other.id);

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        switch (other_type) {
            case Type::bug: {
                if (m_type == Type::water_line_L or m_type == Type::water_line_R) return;

                m_force.x = m_force.x * 0.5F + other_velocity.x * 0.5F;
                other.owner->velocity_x(m_force.x * 0.1F + other_velocity.x * 0.9F);
            

                if (std::abs(other_velocity.y) > 0.2F) {
                    velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                }
                break;
            }
            case Type::frog:
            case Type::mole: {
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
                break;
            }
            case Type::player: {
                if (other_center.y > our_DR.y) return;

                cF32 start_diff_y = start_offset().y - position_on_level().y;
                if (std::abs(start_diff_y) > 8.0F or std::abs(other_velocity.y) < 1.5F) {
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
                //move_velocity_y(other.owner->velocity().y * 1.0F;
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
                break;
            }
        }
    }
}