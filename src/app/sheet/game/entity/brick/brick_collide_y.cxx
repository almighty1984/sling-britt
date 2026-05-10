module entity.brick;
import app.config;
import console;
import aabb;
import particle_system;

namespace entity {
    void Brick::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if ((m_parent and other.owner == m_parent) or is_dead() or other.owner->is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        //if (velocity().y > 0.0F and our_UL.y > other_DR.y - 4.0F) return;

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (other.owner->parent() and other_type == Type::brick) {
            return;
        }

        if (m_parent) {
            //collide_x(our, other);
            return;
        }


        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                if (m_parent or other.owner->parent() or is_hurting()) return;
                console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.9F);
                //if (velocity().y <= -m_break_velocity.y or velocity().y >= m_break_velocity.y) {
                hurt(other.owner);
                //return;
            //}
                break;
            }

            case Type::brick: {
                if (m_parent or other.owner->parent()) return;

                //if (m_state == state::Type::tossed) {
                if (std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    velocity_y(our_velocity.y * -0.2F);
                    other.owner->velocity_y(our_velocity.y * 0.2F);
                    return;
                }
                if (our_UL.y < other_UL.y) {
                    position_add_y(-overlap_y);
                    m_is_on_ground = true;
                    velocity_y(0.0F);
                    moved_velocity(other_velocity);
                }
                break;
            }
            case Type::bee: {
                if (m_state == state::Type::tossed) {
                    collide_x(our, other);
                    return;
                }                
                break;
            }
            case Type::bug: {
                console::log("Brick::collide_y bug\n");
                if (m_state == state::Type::tossed
                    //std::abs(our_velocity.y - other_velocity.y) >= m_break_velocity.y
                    ) {
                    //velocity_y(velocity().y * -0.5F);
                    //hurt(other.owner);
                    //other.owner->hurt(this);
                    collide_x(our, other);
                    return;
                }
                /*if (our_UL.y < other_DR.y - 2.0F) {
                    position_add_y(-overlap_y);
                    m_is_on_ground = true;
                    velocity_y(0.0F);
                    moved_velocity(other_velocity);
                }*/
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD: {
                /*if (our_UL.y > other_DR.y - 2.0F) {
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

                if (velocity().y < 0.0F and other_DR.y < our_DR.y) {
                    position_add_y(-overlap_y);
                    //velocity_y(velocity().y * -1.0F);
                }
                else if (velocity().y > 0.0F and other_UL.y > our_UL.y) {
                    position_add_y(-overlap_y);
                    moved_velocity_y(0.0F);
                    m_is_on_ground = true;
                }
                velocity_y(0.0F);
                break;
            }
            case Type::clip_U:
            case Type::slope_U: {
                if (our_velocity.y < 0.0F) return;
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    console::log("Brick::collide_y velocity.y: ", velocity().y, "\n");
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    return;
                }
                position_add_y(-overlap_y);
                velocity_y(0.0F);
                moved_velocity({});
                m_is_on_ground = true;
                m_is_on_slope = other_type == Type::slope_U;
                break;
            }
            case Type::frog: {
                if (m_state == state::Type::tossed) {
                    //if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    position_add_y(-overlap_y);
                    velocity_y(our_velocity.y * -0.2F);
                    hurt(other.owner);
                    other.owner->hurt(this);
                    return;
                }
                if (our_DR.y >= other_UL.y + 4.0F) return;

                position_add_y(-overlap_y);
                velocity_y(0.0F);
                moved_velocity(other_velocity);
                m_is_on_ground = true;
                break;
            }        
            case Type::player: {
                if (our_DR.y >= other_UL.y + 4.0F) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.2F);
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
                break;
            }
            case Type::slope_L_1x1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(velocity().y * -0.2F);
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
                break;
            }
            case Type::slope_R_1x1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(velocity().y * -0.2F);
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
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(our_velocity.y * -0.2F);
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
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (std::abs(our_velocity.y) >= m_break_velocity.y) {
                    velocity_y(our_velocity.y * -0.2F);
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
                break;
            }
            case Type::spring_U: {
                if (m_parent) return;
                position_add_y(-overlap_y);
                max_velocity_y(10.0F);
                velocity_y(-5.0F - our_velocity.y / 4.0F);
                m_is_on_ground = false;
                m_is_on_slope = false;
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                m_next_state = state::Type::swim;

                if (our_velocity.y > 0.0F) {
                    if (!sound_is_playing("water_enter")) {
                        console::log("Brick::collide_y water_line\n");
                        sound_position("water_enter", { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
                        sound_play("water_enter");
                        particle::spawn_fan(this, 235.0F, 305.0F, 6,
                                            particle::Type::drop_water, position() + Vec2F{ -4.0F, 0.0F },
                                            Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 2.0F);
                    }
                }
                break;
            }
        }
    }
}