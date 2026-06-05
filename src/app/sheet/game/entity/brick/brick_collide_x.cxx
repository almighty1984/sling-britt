module entity.brick;
import console;
import aabb;

namespace entity {
    void Brick::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!other.owner or is_dead() or other.owner->state() == state::Type::carried) return;

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

        cVec2F our_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();

        state::cType other_state = other.owner->state();

        if (m_parent and m_state == state::Type::carried) {
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
                    other.owner->velocity_x(our_velocity.x * 0.2F);
                    other.owner->hurt(this);
                    velocity_x(-our_velocity.x * 0.2F);
                    hurt(other.owner);
                }
                break;
            }
            case Type::brick: {
                if (m_state == state::Type::dead or other.owner->state() == state::Type::dead) return;
                //move_velocity_x(0.0F);

                if (m_state == state::Type::tossed) {
                    //if (!m_is_on_ground and std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    other.owner->velocity_x(our_velocity.x * 0.2F);
                    other.owner->hurt(this);
                    velocity_x(our_velocity.x * -0.2F);
                    hurt(other.owner);

                    return;
                }
                //velocity_x(other_velocity.x * 0.9F);
                //other.owner->velocity_x(our_velocity.x * 0.5F);

                //velocity(other_velocity * 0.9F);
                

                position_add_x(-overlap_x * 0.5F);
                other.owner->position_add_x(overlap_x * 0.5F);

                //Vec2F relative_vel = our_velocity - other_velocity;
                //velocity_x(-relative_vel.x / 2.0F);
                //other.owner->velocity_x(relative_vel.x / 2.0F);

                //console::log("relative vel: ", relative_vel.x, "\n");
                
                //console::log("num inputs: ", other.owner->num_inputs(), " force_x: ", force_x, "\n");
                //velocity_x(our_velocity.x * force_x);

                //velocity_x(other_velocity.x * 0.5F);
                //other.owner->velocity_x(our_velocity.x * 0.5F);
                velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F);
                other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F);
                if (std::abs(other_velocity.x) > std::abs(our_velocity.x)) {                
                ////    m_parent = nullptr;
                //      //position_add_x(-overlap_x);
                    //velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F);

                      //F32 force_x = 1.0F / (num_inputs() + 1.0F);
                      //velocity_x(other_velocity.x * force_x);

                //      //add_input(other.owner);
                //      //add_inputs_from(other.owner);
                ////}
                ////if (std::abs(other_velocity.x) > std::abs(our_velocity.x)) {
                    //velocity_x(other_velocity.x * 0.7F + our_velocity.x * 0.1F);
                }
                else if (std::abs(our_velocity.x) > std::abs(other_velocity.x)) {
                    //other.owner->velocity_x(our_velocity.x * 0.7F + other_velocity.x * 0.1F);
                ////    //    other.owner->parent(nullptr);
                ////    other.owner->position_add_x(-overlap_x);
                ////    //other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.1F);

                ////    other.owner->add_input(this);
                ////    other.owner->add_inputs_from(this);
                    F32 force_x = 1.0F / (other.owner->num_inputs() + 1.0F);
                    other.owner->velocity_x(our_velocity.x * force_x);
                }
                break;
            }
            case Type::bug: {
                if (m_parent or other.owner->parent()) return;
                //console::log(class_name(), "::collide_x bug\n");
                //position_add_x( -overlap_x );
                //move_velocity_x(0.0F);
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
                    other.owner->move_velocity_x(0.0F);
                    sprite::is_leftward(other.owner->sprite(), !sprite::is_leftward(other.owner->sprite()));
                }*/

                //velocity_x(velocity().x * -0.9F);
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_L:
            case Type::clip_R:
            case Type::clip_LR:
            case Type::clip_LD:
            case Type::clip_RD: {
                if ((other_type == Type::clip_L or other_type == Type::clip_LD) and our_velocity.x < 0.0F or
                    (other_type == Type::clip_R or other_type == Type::clip_RD) and our_velocity.x > 0.0F) {
                    return;
                }
                position_add_x(-overlap_x);
                move_velocity_x(0.0F);
                velocity_x(our_velocity.x * -0.5F);
                //console::log(class_name(), "velocity.x ", velocity().x, "\n");
                if (m_state == state::Type::tossed or std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    hurt(other.owner);
                }
                break;
            }
            case Type::frog: {
                if (m_state == state::Type::tossed or std::abs(our_velocity.x - other_velocity.x) >= m_break_velocity.x) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    velocity_x(our_velocity.x * -0.2F);
                    return;
                }
                if (our_DR.y < other_UL.y + 4.0F) return;

                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -0.9F);
                move_velocity_x(0.0F);
                
                break;
            }
            case Type::mole: {
                if (other_state == state::Type::idle) return;

                if (m_time_in_state < 10) {
                    position_add_x(-overlap_x);
                    return;
                }
                if (m_state == state::Type::tossed and other.owner->state() != state::Type::idle) {
                    hurt(other.owner);
                }
                break;
            }
            case Type::particle_drop: {
                m_child = other.owner;
                break;
            }
            case Type::particle_melee: {
                velocity_x(other_velocity.x * 1.5F);
                hurt(this);
                velocity_x(0.0F);
                other.owner->velocity_x(0.0F);
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
            case Type::train_saw: {
                position_add_x(-overlap_x);
                cVec2F aabb_vel = aabb::velocity(other.id);
                velocity(other_velocity + aabb_vel - our_velocity);
                hurt(other.owner);                
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
    }
}