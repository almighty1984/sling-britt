module;
#include <cmath>
#include <sstream>

module entity.particle.drop;
import console;

namespace entity {
    void ParticleDrop::state_idle() {
        m_parent = nullptr;

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
        }

        if (m_time_in_state <= 5) {
            ++m_time_in_state;
        }
        if (m_time_in_state == 5) {
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
        }

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

        cF32 degrees = radians * 180.0F / 3.1415926535F;

        sprite::angle(m_sprite_id, degrees);
    }
    void ParticleDrop::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
        }
        velocity({});
        if (m_parent && is_water_line(m_parent->type())) {
            position_y(m_parent->position().y - 0.0F);
        }


        //console::log("angle: ", sprite::angle(m_sprite_id), "\n");
    }

    void ParticleDrop::collide_x(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);

        cVec2F other_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };

        if (other_type == Type::clip_ledge
            &&
            (our_rect.h < other_rect.y + other_size.y / 4.0F || our_rect.y > other_rect.h - other_size.y / 4.0F)) {

            collide_y(our, other);
            return;
        }

        if (is_water_line(other.owner->type())) {
            collide_y(our, other);
        }
        else if (other_type == Type::clip ||
            other_type == Type::clip_ledge ||
            (other_type == Type::clip_L && velocity().x > 0.0F) ||
            (other_type == Type::clip_LD && velocity().x > 0.0F) ||
            (other_type == Type::clip_R && velocity().x < 0.0F) ||
            (other_type == Type::clip_RD && velocity().x < 0.0F)
            ) {
            position_add_x(-overlap_x);
            velocity({});

            if (other_rect.x < our_rect.x) {
                sprite::angle(m_sprite_id, 180.0f);
            } else {
                sprite::angle(m_sprite_id, 0.0f);
            }
            m_next_state = State::dead;
        }
    }
    void ParticleDrop::collide_y(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        if (other_type == Type::clip_L || other_type == Type::clip_R) {
            collide_x(our, other);
            return;
        }

        if (other_rect.y > our_rect.y && velocity().y > 0.0f) {
            if (is_water_line(other_type)) {
                position_add_y(-overlap_y);
                velocity({});
                //moved_velocity() = other.owner->velocity();
                m_next_state = State::dead;
                m_time_left_dead = m_time_to_be_dead;

                m_parent = other.owner;

                sprite::angle(m_sprite_id, 270.0f);
            } else if (other_type == Type::clip       ||
                       other_type == Type::clip_U     ||
                       other_type == Type::clip_ledge ||
                       other_type == Type::slope_U    ||
                       is_slope(other_type)
                ) {
                position_add_y(-overlap_y);
                position_add({ 0.0F, 2.0f });

                velocity_y(0.0F);
                acceleration_y(0.0F);

                velocity_x(0.0F);

                sprite::angle(m_sprite_id, 90.0f);

                if (other_type == Type::slope_L_1x1) {
                    sprite::angle(m_sprite_id, 45.0f);
                } else if (other_type == Type::slope_R_1x1) {
                    sprite::angle(m_sprite_id, 135.0f);
                } else if (other_type == Type::slope_L_2x1_0 || other_type == Type::slope_L_2x1_1) {
                    sprite::angle(m_sprite_id, 67.5f);
                } else if (other_type == Type::slope_R_2x1_0 || other_type == Type::slope_R_2x1_1) {
                    sprite::angle(m_sprite_id, 112.5f);
                }

                m_next_state = State::dead;
                m_time_left_dead = m_time_to_be_dead;

                m_parent = nullptr;
            }
        } else if (other_rect.h < our_rect.h && velocity().y < 0.0f) {
            if (is_arch(other_type)) {
                if (other_type == Type::arch_L_1x1) {
                    sprite::angle(m_sprite_id, 45.0f);
                } else if (other_type == Type::arch_R_1x1) {
                    sprite::angle(m_sprite_id, 135.0f);
                } else if (other_type == Type::arch_L_2x1_0 || other_type == Type::arch_L_2x1_1) {
                    sprite::angle(m_sprite_id, 112.5f);
                } else if (other_type == Type::arch_R_2x1_0 || other_type == Type::arch_R_2x1_1) {
                    sprite::angle(m_sprite_id, 67.5f);
                }
                position_add({ 0.0F, -overlap_y });
                position_add({ 0.0F, -4.0F });
                velocity_y(0.0F);
                acceleration_y(0.0F);
                m_next_state = State::dead;
            } else if (other_type == Type::clip ||
                other_type == Type::clip_D ||
                other_type == Type::clip_LD ||
                other_type == Type::clip_RD ||
                other_type == Type::clip_ledge) {
                position_add({ 0.0F, -overlap_y });

                velocity_y(0.0F);
                acceleration_y(0.0F);
                velocity_x(0.0F);
                sprite::angle(m_sprite_id, 270.0f);

                m_next_state = State::dead;

                m_parent = nullptr;
            }
        }
    }
}