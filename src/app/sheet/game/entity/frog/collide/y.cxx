module entity.frog;
import app.config;
import aabb;
import console;
import particle_system;

namespace entity {
    void Frog::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (!our.owner or !other.owner or is_dead()) return;

        aabb::cName our_name = aabb::name(our.id);

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F our_center = aabb::center(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);
        cVec2F other_center = aabb::center(other.id);

        if (velocity().y > 0.0F and our_DR.y - 4.0F - velocity().y > other_UL.y) {
            return;
        }

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();
        cVec2F our_velocity = velocity() + move_velocity();

       
        if (m_state == state::Type::dive) {
            if (is_clip(other_type) and other_velocity.y >= 0.0F and our_DR.y < m_water_line_y) {
                m_next_state = state::Type::idle;
            }
        }

        switch (other_type) {
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50: 
            case Type::clip_ledge_R_50: {
                position_add_y(-overlap_y);
                velocity_y(0.0F);
                move_velocity_y(0.0F);
                if (our_UL.y < other_UL.y) {
                    m_is_on_ground = true;
                    m_is_on_slope = false;
                    move_velocity_x(0.0F);
                }
                break;
            }
            case Type::clip_U:
            case Type::clip_U_50:
            case Type::slope_U: {
                if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                move_velocity_x(0.0F);
                m_is_on_ground = true;
                m_is_on_slope = other_type == Type::slope_U;
                break;
            }
            case Type::clip_L:
            case Type::clip_L_50:
            case Type::clip_R:
            case Type::clip_R_50:
            case Type::clip_D:
            case Type::clip_D_50:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (velocity().y > 0.0F) return;
                position_add_y(-overlap_y);
                m_is_on_ground = false;
                m_is_on_slope = false;
                velocity_y(std::abs(velocity().y) * 0.75F);

                particle::spawn(this, particle::Type::hit, { other_UL.x + 8.0F, other_DR.y }, {});
                break;
            }
            case Type::bug: {
                //console::log("Frog::collide_y bug\n");
                if (other.owner->state() == state::Type::tossed) {
                    return;
                }
                if (other_UL.y > our_UL.y) return;

                if (our_UL.y > other_UL.y) {
                    //hurt(other.owner);
                    //m_next_state = state::Type::hurt;
                    /*Vec2F our_center = Vec2F{ our_UL.x + (our_DR.x - our_UL.x) / 2.0F,     our_UL.y + (our_DR.y - our_UL.y) / 2.0F };
                    Vec2F other_center = Vec2F{ other_UL.x + (other_DR.x - other_UL.x) / 2.0F, other_UL.y + (other_DR.y - other_UL.y) / 2.0F };

                    spawn(particle::Type::hit, { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                    hurt(other_type);*/
                    return;
                }
                position_add_y(-overlap_y);
                if (other.owner->state() == state::Type::upended or other.owner->state() == state::Type::bounce) {
                    if (velocity().y < 0.0F) return;
                    position_add_y(-overlap_y);
                    max_velocity_y(10.0F);
                    velocity_y(-5.0F - velocity().y / 4.0F);
                    m_is_on_ground = false;
                    m_is_on_slope = false;
                    //m_time_left_bouncing = m_config.time_to_bounce();
                    return;
                }
                move_velocity(other_velocity);
                //velocity_x(velocity().x * 0.5F);
                velocity_y(0.0F);
                m_is_on_ground = true;
                break;
            }
            case Type::frog: {
                if (our_DR.y <= other_UL.y) {
                    position_add_y(-overlap_y);
                    velocity_y(-4.0F);

                    velocity_x(other.owner->velocity().x + other.owner->move_velocity().x);
                    velocity_add({ 0.0F, other.owner->velocity().y + other.owner->move_velocity().y });

                    Vec2F our_center = Vec2F{ our_UL.x + (our_DR.x - our_UL.x) / 2.0F,     our_UL.y + (our_DR.y - our_UL.y) / 2.0F };
                    Vec2F other_center = Vec2F{ other_UL.x + (other_DR.x - other_UL.x) / 2.0F, other_UL.y + (other_DR.y - other_UL.y) / 2.0F };

                    particle::spawn(this, particle::Type::hit,
                        { (our_center.x + other_center.x) / 2.0F, (our_center.y + other_center.y) / 2.0F }, {});
                    //, sound("melee")
                }
                break;
            }
            case Type::particle_health: {
                collide_x(our, other);
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                //if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                //if (velocity().x < 0.0F) {
                velocity_y(velocity().x / -2.0F);
                //}        
                m_is_on_ground = true;
                m_is_on_slope = true;
                if (velocity().x > max_velocity().x * 0.9F) {
                    velocity_x(max_velocity().x * 0.9F);
                }
                move_velocity_x(0.0F);
                break;
            }
            case Type::slope_R_1x1: {
                //if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                //if (velocity().x > 0.0F) {
                velocity_y(velocity().x);
                //}
                m_is_on_ground = true;
                m_is_on_slope = true;
                if (velocity().x < -max_velocity().x * 0.7F) {
                    velocity_x(-max_velocity().x * 0.7F);
                }
                move_velocity_x(0.0F);
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                //if (velocity().y < 0.0F) return;
                position_add_y(-overlap_y);

                velocity_y(0.0F);
                move_velocity_y(0.0F);
                if (velocity().x > 0.0F) {
                    velocity_y(velocity().x / 2.0F);
                }
                m_is_on_ground = true;
                m_is_on_slope = true;
                if (velocity().x < -max_velocity().x * 0.9F) {
                    velocity_x(-max_velocity().x * 0.9F);
                }
                move_velocity_x(0.0F);
                break;
            }
            case Type::player: {
                if (other.owner->state() == state::Type::sling) {
                    collide_x(our, other);
                    return;
                }
                if (our_UL.y <= other_UL.y) {
                    position_add_y(-overlap_y);
                    m_next_state = state::Type::dive;
                    //other.owner->velocity_y(std::abs(our_velocity.y) * 0.5F + std::abs(other_velocity.y) * 0.5F);                        
                    position_add_y(-8.0F);
                    velocity_y(other_velocity.y * 1.2F);
                    velocity_x(other_velocity.x * 0.5F);
                    other.owner->velocity_y(other_velocity.y * -1.0F);
                    other.owner->next_state(state::Type::swim);      
                        
                    //cVec2F our_center = Vec2F{ our_UL.x + (our_DR.x - our_UL.x) / 2.0F,     our_UL.y + (our_DR.y - our_UL.y) / 2.0F };
                    cVec2F other_center = Vec2F{ other_UL.x + (other_DR.x - other_UL.x) / 2.0F, other_UL.y + (other_DR.y - other_UL.y) / 2.0F };

                    cVec2F hit_pos = { other_center.x - 8.0F, other_UL.y - 12.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});

                    sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                    sound_play("bump_head");
                }
                break;
            }
            case Type::train_saw: {
                position_add_y(-overlap_y);
                collide_x(our, other);                
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R: 
            case Type::water_line: {
                if (m_state == state::Type::dive and our_velocity.y < 0.0F) return;
                bool is_to_splash = false,
                     is_upwards   = false;

                m_radians = other.owner->radians();

                if (our_velocity.y > 0.0F and m_state != state::Type::swim) {
                    m_next_state = state::Type::swim;
                }

                m_water_line_y = other.owner->start_offset().y;

                //if (position_on_level().y + 0.0F > m_water_line_y) return;

                cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                                     position().y - app::config::extent().y / 2.0F };

                if (our_velocity.y <= -2.0F) {
                    is_upwards = true;
                    if (!sound_is_playing("water_exit") and !sound_is_playing("water_enter")) {
                        sound_position("water_exit", sound_pos);
                        sound_play("water_exit");
                        is_to_splash = true;
                    }
                    m_time_in_state = 0;
                    m_is_on_ground = false;
                } else if (our_velocity.y > 2.0F) {
                    is_upwards = false;
                    if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                        sound_position("water_enter", sound_pos);
                        sound_play("water_enter");
                        is_to_splash = true;
                    }
                }

                if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                    other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                } else {
                    velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
                }

                if (is_to_splash) {
                    console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                    cVec2F pos = Vec2F{ our_UL.x, other_UL.y } + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F };
                    cVec2F vel = Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) };

                    particle::splash_water(this, pos, vel, 2.0F);
                }
                break;
            }
        }
    }
}