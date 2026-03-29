module entity.brick;
import app.config;
import console;
import aabb;
import particle_system;

namespace entity {
    void Brick::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if ((m_parent and other.owner == m_parent) or is_dead() or other.owner->is_dead()) return;

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        //if (velocity().y > 0.0F and our_rect.y > other_rect.h - 4.0F) return;

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_rect.y < other_rect.y ? our_rect.h - other_rect.y : -(other_rect.h - our_rect.y);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (other.owner->parent() and other_type == Type::brick) {
            return;
        }

        if (m_parent) {
            //collide_x(our, other);
            return;
        }

        if (is_arch(other_type)) {
            if (m_parent or other.owner->parent() or is_hurting()) return;
            console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.9F);
            //if (velocity().y <= -m_break_velocity.y or velocity().y >= m_break_velocity.y) {
                hurt(other.owner);
                //return;
            //}
        }
        else if (other_type == Type::brick) {
            if (m_parent or other.owner->parent()) return;

            //if (m_state == state::Type::tossed) {
            if (std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                hurt(other.owner);
                other.owner->hurt(this);
                velocity_y(our_velocity.y * -0.5F);
                other.owner->velocity_y(our_velocity.y * 0.5F);
                return;
            }
            if (our_rect.y < other_rect.y) {
                position_add_y(-overlap_y);
                m_is_on_ground = true;
                velocity_y(0.0F);
                moved_velocity(other_velocity);
            }
        }
        else if (other_type == Type::bug) {            
            //console::log("Brick::collide_y bug\n");
            if (m_state == state::Type::tossed or other.owner->state() == state::Type::tossed
                //std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y
                ) {
                velocity_y(velocity().y * -0.5F);
                hurt(other.owner);
                other.owner->hurt(this);
                return;
            }
            position_add_y(-overlap_y);
            m_is_on_ground = true;
            velocity_y(0.0F);
            moved_velocity(other_velocity);
        }
        else if (other_type == Type::clip or
            other_type == Type::clip_ledge or
            other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {

            /*if (our_rect.y > other_rect.h - 2.0F) {
                return;
            }*/

            if (other_type == Type::clip_D and velocity().y + moved_velocity().y > 0.0F) {
                return;
            }
            if (other_type == Type::clip_LD and velocity().x + moved_velocity().x < 0.0F
                and velocity().y + moved_velocity().y > 0.0F) {
                return;
            }
            if (other_type == Type::clip_RD and velocity().x + moved_velocity().x > 0.0F
                and velocity().y + moved_velocity().y > 0.0F) {
                return;
            }


            if (m_state == state::Type::tossed) {
                //console::log("velocity.y: ", velocity().y, "\n");
                if (
                    (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD and our_velocity.y < -1.0F)
                    or
                    std::abs(velocity().y) >= m_break_velocity.y) {
                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -0.9F);
                    hurt(other.owner);
                    return;
                }
            }

            if (velocity().y < 0.0F and other_rect.h < our_rect.h) {
                position_add_y(-overlap_y);
                //velocity_y(velocity().y * -1.0F);
            }
            else if (velocity().y > 0.0F and other_rect.y > our_rect.y) {
                position_add_y(-overlap_y);
                moved_velocity_y(0.0F);
                m_is_on_ground = true;
            }
            velocity_y(0.0F);
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            if (std::abs(velocity().y) >= m_break_velocity.y) {
                console::log("Brick::collide_y velocity.y: ", velocity().y, "\n");
                velocity_y(velocity().y * -0.9F);
                hurt(other.owner);
                return;
            }
            position_add_y(-overlap_y);
            velocity({});
            moved_velocity({});
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::frog) {
            if (our_rect.h >= other_rect.y + 4.0F) return;

            if (m_state == state::Type::tossed) {
                //if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                hurt(other.owner);
                other.owner->hurt(this);
                return;
            }
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            moved_velocity(other_velocity);
            m_is_on_ground = true;
        }
        else if (other_type == Type::particle_shot) {
            hurt(other.owner);
        }
        else if (other_type == Type::player) {
            if (our_rect.h >= other_rect.y + 4.0F) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.9F);
            if (std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                hurt(other.owner);
                return;
            }
            velocity_y(-std::abs(other_velocity.y));
            return;

            if (other.owner->state() == state::Type::sling) {
                velocity_x(-other.owner->rotation_speed());
                hurt(other.owner);
                return;
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            if (std::abs(velocity().y) >= m_break_velocity.y) {
                velocity_y(velocity().y * -0.9F);
                hurt(other.owner);
                return;
            }
            position_add_y(-overlap_y);
            //velocity_y(-velocity().x;
            if (velocity().y > 0.0F) {
                velocity_x(-velocity().y);
            }
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        } else if (other_type == Type::slope_R_1x1) {
            if (std::abs(velocity().y) >= m_break_velocity.y) {
                velocity_y(velocity().y * -0.9F);
                hurt(other.owner);
                return;
            }
            position_add_y(-overlap_y);
            //velocity_y(velocity().x);
            if (velocity().y > 0.0F) {
                velocity_x(velocity().y);
            }
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            if (std::abs(velocity().y) >= m_break_velocity.y) {
                velocity_y(velocity().y * -0.9F);
                hurt(other.owner);
                return;
            }
            position_add_y(-overlap_y);
            //velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x < 0.0F) {

            //velocity_y(velocity().x / -2.0F;
            //}
            //if (velocity().y > 0.0F) {
                //velocity_x(-velocity().y / 2.0F;
            //}
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            if (std::abs(velocity().y) >= m_break_velocity.y) {
                velocity_y(velocity().y * -0.9F);
                hurt(other.owner);
                return;
            }
            position_add_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            //if (velocity().x > 0.0F) {
                //velocity_y(velocity().x / 2.0F;
            //}
            if (velocity().y > 0.0F) {
                velocity_x(velocity().y / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::spring_U) {
            if (m_parent) return;
            position_add_y(-overlap_y);
            max_velocity_y(10.0F);
            velocity_y(-5.0F - velocity().y / 4.0F);
            m_is_on_ground = false;
            m_is_on_slope = false;
        }
        else if (is_water_line(other_type)) {

            m_next_state = state::Type::swim;

            if (velocity().y > 0.0F) {
                if (!sound_is_playing("water_enter")) {
                    console::log("Brick::collide_y water_line\n");
                    sound_position("water_enter", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_enter");
                    particle::spawn_fan(this, 235.0F, 305.0F, 6,
                                        particle::Type::drop_water, position() + Vec2F{ -4.0F, 0.0F },
                                        Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 2.0F);
                }
            }
        }
    }
}