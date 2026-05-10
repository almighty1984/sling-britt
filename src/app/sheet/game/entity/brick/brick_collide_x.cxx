module entity.brick;
import console;
import aabb;

namespace entity {
    void Brick::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (other.owner == m_parent or is_dead() or other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        Vec2F our_extent = { aabb::UR(our.id).x - aabb::UL(our.id).x,
                             aabb::DL(our.id).y - aabb::UL(our.id).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (other.owner->parent() and other_type == Type::brick) {
            return;
        }

        if (m_parent) {
            if (is_clip(other_type) or
                other_type == Type::brick) {
                //console::log(class_name(), "::collide_x near: ", to_string(other_type), "\n");
                if (our_DR.x > other_DR.x) {
                    m_is_near_wall_L = true;
                }
                if (our_UL.x < other_UL.x) {
                    m_is_near_wall_R = true;
                }
            }
            return;
        }

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                collide_y(our, other);
                break;
            }
            case Type::bee: {
                if (m_state == state::Type::tossed) {                    
                    hurt(other.owner);
                    other.owner->hurt(this);
                    velocity_x(our_velocity.x * 0.5F);
                    other.owner->velocity_x(our_velocity.x * 0.2F);
                }
                break;
            }
            case Type::brick: {
                position_add_x(-overlap_x);
                moved_velocity_x(0.0F);

                if (m_state == state::Type::tossed) {
                    //if (!m_is_on_ground and std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    other.owner->hurt(this);
                    hurt(other.owner);
                    velocity_x(our_velocity.x * -0.2F);
                    other.owner->velocity_x(our_velocity.x * 0.2F);

                    return;
                }
                if (std::abs(other_velocity.x) > std::abs(our_velocity.x)) {
                    velocity_x(other_velocity.x);
                }
                break;
            }
            case Type::bug: {
                if (m_parent or other.owner->parent()) return;
                //console::log(class_name(), "::collide_x bug\n");
                //position_add_x( -overlap_x );
                //moved_velocity_x(0.0F);
                //position_add_x(-overlap_x);
                if (m_state == state::Type::tossed) {
                    //if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    hurt(other.owner);
                    other.owner->hurt(this);

                    velocity_x(other_velocity.x * 0.2F);
                    other.owner->velocity_x(our_velocity.x * 0.2F);
                    return;
                }
                /*if (other.owner->state() == state::Type::walk) {
                    other.owner->velocity_x(velocity().x * -0.9F);
                    other.owner->moved_velocity_x(0.0F);
                    sprite::is_leftward(other.owner->sprite(), !sprite::is_leftward(other.owner->sprite()));
                }*/

                //velocity_x(velocity().x * -0.9F);
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_L:
            case Type::clip_LD:
            case Type::clip_R:
            case Type::clip_RD: {
                if ((other_type == Type::clip_L or other_type == Type::clip_LD) and our_velocity.x < 0.0F or
                    (other_type == Type::clip_R or other_type == Type::clip_RD) and our_velocity.x > 0.0F) {
                    return;
                }
                position_add_x(-overlap_x);
                moved_velocity_x(0.0F);
                velocity_x(our_velocity.x * -0.5F);
                //console::log(class_name(), "velocity.x ", velocity().x, "\n");
                if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    hurt(other.owner);
                }
                break;
            }
            case Type::frog: {
                if (our_DR.y < other_UL.y + 4.0F) return;

                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -0.9F);
                moved_velocity_x(0.0F);
                if (m_state == state::Type::tossed) {
                    //if (std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    return;
                }
                break;
            }
            case Type::mole: {
                if (m_time_in_state < 10) {
                    position_add_x(-overlap_x);
                    return;
                }
                if (m_state == state::Type::tossed and other.owner->state() != state::Type::idle) {
                    hurt(other.owner);
                }
                break;
            }
            case Type::player: {
                if (other_velocity.y >= 6.0F) {
                    console::log("yo\n");
                    hurt(other.owner);
                    return;
                }
                velocity_x(other_velocity.x);
                position_add_x(-overlap_x);
                if (other.owner->state() == state::Type::sling) {
                    velocity_x(-other.owner->rotation_speed());
                    hurt(other.owner);
                }
                break;
            }
            case Type::slope_L_1x1:
            case Type::slope_R_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                collide_y(our, other);
                break;
            }
            case Type::spring_U:
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
    }
}