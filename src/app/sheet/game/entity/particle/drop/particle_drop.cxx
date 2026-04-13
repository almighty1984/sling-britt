module entity.particle.drop;
import console;

namespace entity {
    void ParticleDrop::state_idle() {
        //m_parent = nullptr;

        /*if (m_parent) {
            if (m_parent->type() == Type::water_line) {
                console::log("parent pos y: ", m_parent->position().y, "\n");
            }
        }*/

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
        }

        if (m_time_in_state <= 5) {
            ++m_time_in_state;
        }
        if (m_time_in_state == 5) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
        }

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

        cF32 degrees = radians * 180.0F / 3.1415926535F;

        sprite::angle(m_sprite, degrees);
    }
    void ParticleDrop::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
        }
        velocity({});
        if (m_parent and is_water_line(m_parent->type())) {
            position_y(m_parent->position().y - 0.0F);
        }


        //console::log("angle: ", sprite::angle(m_sprite), "\n");
    }

    void ParticleDrop::collide_x(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_points.x < other_points.x ? our_points.w - other_points.x : -(other_points.w - our_points.x);

        cVec2F other_size = { other_points.w - other_points.x, other_points.h - other_points.y };

        if (other_type == Type::clip_ledge
            and
            (our_points.h < other_points.y + other_size.y / 4.0F or our_points.y > other_points.h - other_size.y / 4.0F)) {

            collide_y(our, other);
            return;
        }

        if (is_water_line(other.owner->type())) {
            collide_y(our, other);
        }
        else if (other_type == Type::clip or
            other_type == Type::clip_ledge or
            (other_type == Type::clip_L and velocity().x > 0.0F) or
            (other_type == Type::clip_LD and velocity().x > 0.0F) or
            (other_type == Type::clip_R and velocity().x < 0.0F) or
            (other_type == Type::clip_RD and velocity().x < 0.0F)
            ) {
            position_add_x(-overlap_x);
            velocity({});

            if (other_points.x < our_points.x) {
                sprite::angle(m_sprite, 180.0f);
            } else {
                sprite::angle(m_sprite, 0.0f);
            }
            m_next_state = state::Type::dead;
        }
    }
    void ParticleDrop::collide_y(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);

        if (other_type == Type::clip_L or other_type == Type::clip_R) {
            collide_x(our, other);
            return;
        }

        if (other_points.y > our_points.y and velocity().y > 0.0F) {
            if (is_water_line(other_type)) {
                position_add_y(-overlap_y);
                velocity({});
                //moved_velocity() = other.owner->velocity();
                m_next_state = state::Type::dead;
                m_time_left_dead = m_time_to_be_dead;

                m_parent = other.owner;

                sprite::angle(m_sprite, 270.0f);
            } else if (other_type == Type::clip       or
                       other_type == Type::clip_U     or
                       other_type == Type::clip_ledge or
                       other_type == Type::slope_U    or
                       is_slope(other_type)
                ) {
                position_add_y(-overlap_y);
                position_add_y(2.0F);

                velocity_y(0.0F);
                acceleration_y(0.0F);

                velocity_x(0.0F);

                sprite_angle(90.0F);

                if (other_type == Type::slope_L_1x1) {
                    sprite::angle(m_sprite, 45.0F);
                } else if (other_type == Type::slope_R_1x1) {
                    sprite::angle(m_sprite, 135.0F);
                } else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
                    sprite::angle(m_sprite, 67.5F);
                } else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
                    sprite::angle(m_sprite, 112.5F);
                }

                m_next_state = state::Type::dead;
                m_time_left_dead = m_time_to_be_dead;

                m_parent = nullptr;
            }
        } else if (other_points.h < our_points.h and velocity().y < 0.0F) {
            if (is_arch(other_type)) {
                if (other_type == Type::arch_L_1x1) {
                    sprite_angle(45.0F);
                } else if (other_type == Type::arch_R_1x1) {
                    sprite_angle(135.0F);
                } else if (other_type == Type::arch_L_2x1_0 or other_type == Type::arch_L_2x1_1) {
                    sprite_angle(112.5F);
                } else if (other_type == Type::arch_R_2x1_0 or other_type == Type::arch_R_2x1_1) {
                    sprite_angle(67.5F);
                }
                position_add_y(-overlap_y);
                position_add_y(-4.0F);
                velocity_y(0.0F);
                acceleration_y(0.0F);
                m_next_state = state::Type::dead;
            } else if (other_type == Type::clip or
                other_type == Type::clip_D or
                other_type == Type::clip_LD or
                other_type == Type::clip_RD or
                other_type == Type::clip_ledge) {
                position_add({ 0.0F, -overlap_y });

                velocity_y(0.0F);
                acceleration_y(0.0F);
                velocity_x(0.0F);
                sprite_angle(270.0f);

                m_next_state = state::Type::dead;

                m_parent = nullptr;
            }
        }
    }
}