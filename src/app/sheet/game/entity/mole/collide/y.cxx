module entity.mole;
import app.config;
import console;
import aabb;
import particle_system;

namespace entity {
    void Mole::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or !other.owner or other.owner->is_dead()) return;
        
        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        entity::cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        if (time_left_colliding_with(other_type) > 0) return;

        cVec2F our_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);


        /*if (is_slope(other_type) and other_type != Type::slope_U) {
            if (our_DR.y - 4.0F < other_DR.y) return;
        }*/

        if (our_velocity.y < 0.0F) {
            if (m_state == state::Type::dive and our_UL.y > other_DR.y - 4.0F) {
                if (is_arch(other_type)           or
                    other_type == Type::clip      or
                    other_type == Type::clip_D    or
                    other_type == Type::clip_D_50 or
                    other_type == Type::clip_LD   or
                    other_type == Type::clip_RD   or
                    other_type == Type::clip_ledge) {
                    sound_position("bump_head", { position().x - app::config::extent().x / 2.0F,
                                                  position().y - app::config::extent().y / 2.0F });
                    sound_pitch("bump_head", 1.0F - our_velocity.y * 0.1F);
                    sound_play("bump_head");
                    position_add_y(-overlap_y);
                    velocity_y(our_velocity.y * -0.9F);
                    console::log(class_name(), "::collide_y() ", to_string(other_type), "\n");
                }
            }
        }

        if (other_type == Type::brick) {
            if (m_state != state::Type::idle and other.owner->state() == state::Type::tossed) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::clip_U or
                 other_type == Type::clip_U_50 or 
                 other_type == Type::clip_ledge or
                 other_type == Type::clip_ledge_L_50 or
                 other_type == Type::clip_ledge_R_50 or
                 other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            move_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;

            if (other_type == Type::clip_ledge) {
                if (other_UL.x < our_UL.x) {
                    sprite_is_leftward(false);
                } else if (other_DR.x > our_DR.x) {
                    sprite_is_leftward(true);
                }
            }
            else if (other_type == Type::clip_ledge_L_50) {
                sprite_is_leftward(false);
            }
            else if (other_type == Type::clip_ledge_R_50) {
                sprite_is_leftward(true);
            }
            sprite_angle(0.0F);
        }
        else if (other_type == Type::particle_brick) {
            if (m_state != state::Type::idle and !(other_velocity.y > -2.0F and other_velocity.y < 2.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::particle_health) {
            collide_x(our, other);
        }
        else if (other_type == Type::player) {
            if (m_state == state::Type::exit and !sound_is_playing("bounce")) {
                sound_position("bounce", { (position().x + sprite_rect().w / 2.0F) / (app::config::extent().x / 2.0F),
                                           (position().y + sprite_rect().h / 2.0F) / (app::config::extent().y / 2.0F) });
                sound_play("bounce");
                return;
            }

            if (our_UL.y > other_DR.y - 2.0F/* - other_velocity.y*/) {

            } else if (our_DR.y <= other_UL.y + other_extent.y / 2.0F) {
                position_add_y(-overlap_y);
                m_next_state = state::Type::dive;
                //other.owner->velocity_y(std::abs(our_velocity.y) * 0.5F + std::abs(other_velocity.y) * 0.5F);                        
                position_add_y(-8.0F);
                velocity_y(other_velocity.y * 1.2F);
                velocity_x(other_velocity.x * 0.5F);
                other.owner->velocity_y(other_velocity.y * -1.0F);
                //other.owner->next_state(state::Type::swim);

                cVec2F our_center = Vec2F{ our_UL.x + (our_DR.x - our_UL.x) / 2.0F,     our_UL.y + (our_DR.y - our_UL.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_UL.x + (other_DR.x - other_UL.x) / 2.0F, other_UL.y + (other_DR.y - other_UL.y) / 2.0F };

                cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});

                sound_position("melee", { position().x - app::config::extent().x / 2.0F,
                                          position().y - app::config::extent().y / 2.0F });
                sound_play("melee");
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            position_add_y(-overlap_y);
            velocity_y(-velocity().x);
            move_velocity_x(0.0F);
            move_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(45.0F - 90.0F);
        }
        else if (other_type == Type::slope_R_1x1) {
            position_add_y(-overlap_y);
            velocity_y(velocity().x);
            move_velocity_x(0.0F);
            move_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(135.0F - 90.0F);
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            move_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(67.5F - 90.0F);
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            position_add_y(-overlap_y);
            velocity_y(0.0F);
            move_velocity_y(0.0F);
            m_is_on_ground = m_is_on_slope = true;
            sprite_angle(112.5F - 90.0F);
        }
        else if (is_water_line(other_type)) {
            m_radians = other.owner->radians();
            m_water_line_y = other.owner->start_offset().y;
            
            bool is_to_splash = false,
                 is_upwards   = false;

            cVec2F sound_pos = { (position().x + sprite_rect().w / 2.0F) / (app::config::extent().x / 2.0F),
                                 (position().y + sprite_rect().h / 2.0F) / (app::config::extent().y / 2.0F) };
            if (velocity().y < 0.0F and m_next_state == state::Type::dive and m_state != state::Type::dive) {
                time_left_colliding_with(Type::water_line, 10);

                m_next_state = state::Type::dive;
                is_upwards = true;
                if (velocity().y < 0.0F and !sound_is_playing("water_exit")) {
                    sound_position("water_exit", sound_pos);
                    sound_play("water_exit");
                    is_to_splash = true;
                }
            }
            else if (velocity().y > 0.0F and m_next_state != state::Type::swim) {
                m_next_state = state::Type::swim;
                if (velocity().y > 0.0F and !sound_is_playing("water_enter")) {
                    //sound::position(sound("water_enter"), { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
                    //sound::play(sound("water_enter"));
                    sound_position("water_enter", sound_pos);
                    sound_play("water_enter");
                    is_to_splash = true;
                }
            }
            if (is_to_splash) {
                console::log(class_name(), "::collide_y() velocity y: ", velocity().y, "\n");
                cVec2F pos = Vec2F{ our_UL.x, other_UL.y } + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F };
                cVec2F vel = Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) };

                particle::splash_water(this, pos, vel, 2.0F);
            }

            if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
            else {
                velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
        }
    }
}