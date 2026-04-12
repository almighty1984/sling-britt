module entity.bug;
import app.config;
import aabb;
import console;
import sound;
import particle_system;

namespace entity {
    void Bug::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent or is_dead() or other.owner->is_dead()) return;

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        /*if (velocity().y > 0.0F and our_points.h - 4.0F - velocity().y > other_points.y) {
            return;
        }*/

        //if (velocity().y > 0.0F and our_points.y > other_points.h - 4.0F) return;


        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        //console::log("type: ", to_string(other_type), "\n");

        if (m_state == state::Type::tossed) {
            if (our_velocity.y < 0.0F and our_points.y > other_points.h - 4.0F) {
                if (is_arch(other_type) or
                    other_type == Type::clip or
                    other_type == Type::clip_D or
                    other_type == Type::clip_LD or
                    other_type == Type::clip_RD or
                    other_type == Type::clip_ledge) {
                    position_add_y(-overlap_y);
                    if (!is_hurting()) {
                        if (other_type == Type::slope_L_1x1 or other_type == Type::slope_R_1x1) {
                            velocity_x(velocity().x * 0.5F);
                        }
                        else if (other_type == Type::slope_L_1x1 or other_type == Type::slope_R_1x1) {
                            velocity_x(velocity().x * 0.75F);
                        }
                        velocity_y(velocity().y * -0.9F);
                        cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                       (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                        particle::spawn(this, particle::Type::hit, hit_pos, {});
                        hurt(other.owner);
                        m_next_state = state::Type::hurt;
                        return;
                    }
                }
            }
        }


        if (m_state == state::Type::swim and m_num_jumps == 1) {
            if (is_clip(other_type) and other_velocity.y >= 0.0F and our_points.h < m_water_line_y) {
                m_next_state = state::Type::walk;
                m_num_jumps = 0;
            }
        }

        if (is_arch(other_type)) {
            if (m_parent or other.owner->parent() or is_hurting()) return;
            console::log(class_name(), "::collide_y ", to_string(other_type), "\n");
            position_add_y(-overlap_y);
            if (velocity().y <= -2.0F or velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        }
        else if (other_type == Type::bug) {
        }
        else if (other_type == Type::brick) {
            if (m_parent or other.owner->parent()) return;
            return;
            //if (our_points.y > other_points.y) return;
            //if (our_points.y > other_points.h - 4.0F) return;

            
            if (our_points.h < other_points.y + 2.0F) {
                position_add_y(-overlap_y);
                console::log(class_name(), "::collide_y brick above\n");
            }
            if (velocity().y <= -2.0F or velocity().y >= 2.0F) {

                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                velocity_y(velocity().y * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                //other.owner->hurt(this);
                return;
            }
        }
        else if (other_type == Type::clip or other_type == Type::clip_ledge) {
            if (our_points.y < other_points.h) {
                if (velocity().y > 0.0F) {
                    position_add_y(-overlap_y);
                    //console::log(class_name(), "::collide_y hmmmmm\n");                
                }
            }
            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            if (our_points.y < other_points.y) {
                m_is_on_ground = true;
                m_is_on_slope = false;
            }

            //if (!m_is_on_ground) return;
            if (other_type == Type::clip_ledge) {
                if (velocity().y > 0.0F and our_points.y > other_points.y) return;

                if (velocity().y <= -2.0F or velocity().y >= 2.0F) {
                    velocity_y(velocity().y * -0.9F);
                    cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                       (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    hurt(other.owner);
                    m_next_state = state::Type::hurt;
                    sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                    sound_play("melee");
                    return;
                }
                if (m_state == state::Type::walk) {
                    if (velocity().x > 0.0F and other_points.w > our_points.w) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite, true);
                    }
                    else if (velocity().x < 0.0F and other_points.x < our_points.x) {
                        velocity_x(velocity().x * -1.0F);
                        sprite::is_leftward(m_sprite, false);
                    }
                }
            }
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) {
            //if (our_velocity.y < 0.0F) return;
            if (our_points.y > other_points.y) return;

            position_add_y(-overlap_y);

            velocity_y(0.0F);
            moved_velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = other_type == Type::slope_U;
        }
        else if (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            position_add_y(-overlap_y);
            if (velocity().y <= -2.0F or velocity().y >= 2.0F) {
                velocity_y(velocity().y * -0.9F);
                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = state::Type::hurt;
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            m_is_on_ground = false;
            m_is_on_slope = false;
            velocity_y(std::abs(velocity().y) * 0.75F);

            m_next_state = state::Type::hurt;
        } else if (other_type == Type::frog) {
            console::log("entity::Bug::collide_y frog\n");
            if (m_state == state::Type::tossed or (m_state == state::Type::upended and our_points.y < other_points.y)) {
                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = state::Type::hurt;
                other.owner->hurt(this);
                //velocity().x *= -1.0f;
                velocity_y(velocity().y * -1.0f);
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }

            if (our_points.h < other_points.y + 4.0F) {
                position_add_y(-overlap_y);
                if (m_state == state::Type::upended) {
                    m_next_state = state::Type::bounce;
                }
            }
            //if (other_points.y > our_points.y) {            
                //if (!is_hurting()) {
                //    velocity_y(other.owner->velocity().y + velocity().y - 10.0f;
                //}
            //} else {
                //position_add_y( -overlap_y );            
            //}
        }
        else if (other_type == Type::particle_interact) {
            collide_x(our, other);
        }
        else if (other_type == Type::player) {
            if (m_state == state::Type::upended or m_state == state::Type::bounce) {                                
                if (other_points.h < our_points.y + 4.0F and other_velocity.y > 2.0F) {
                    console::log("entity::bug::collide_y() player\n");
                    m_state = state::Type::upended;
                    m_next_state = state::Type::bounce;              
                }
                return;
            }
            //console::log(class_name(), "::collide_y player overlap_y: ", overlap_y, "\n");
            if (m_state == state::Type::swim) {
                //velocity().y += other_velocity.y;
                //moved_velocity_y(other_velocity.y;
                return;
            }
            if (other.owner->state() == state::Type::sling) return;

            if (our_points.y > other_points.h - 2.0F/* - other_velocity.y*/) {
                
            }
            else if (our_points.h <= other_points.y + 4.0F) {
                return;
                cVec2F our_center = Vec2F{ our_points.x + (our_points.w - our_points.x) / 2.0F,     our_points.y + (our_points.h - our_points.y) / 2.0F };
                cVec2F other_center = Vec2F{ other_points.x + (other_points.w - other_points.x) / 2.0F, other_points.y + (other_points.h - other_points.y) / 2.0F };

                cVec2F hit_pos = Vec2F{ our_center.x + other_center.x, our_center.y + other_center.y } / 2.0F - Vec2F{ 8.0F, 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            velocity_y(-velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);

            velocity_y(velocity().x);
            moved_velocity_x(0.0F);
            moved_velocity_y(0.0F);

            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            position_add_y(-overlap_y);
            moved_velocity_y(0.0F);
            velocity_y(0.0F);
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
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
        else if (is_water_line(other_type)) {
            if (m_state == state::Type::swim and m_num_jumps == 1 and velocity().y < 0.0F) return;

            bool is_to_splash = false,
                 is_upwards   = false;

            m_num_jumps = 0;

            m_radians = other.owner->radians();
            
            if (velocity().y > 0.0F and m_state != state::Type::swim) {
                m_next_state = state::Type::swim;
            }

            m_water_line_y = other.owner->start_offset().y - 6.0F;

            if (position_on_level().y + 0.0F > m_water_line_y) return;

            if (velocity().y <= -2.0F) {
                is_upwards = true;
                if (!sound_is_playing("water_exit") and !sound_is_playing("water_enter")) {
                    sound_position("water_exit", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_exit");
                    is_to_splash = true;
                }
                m_time_in_state = 0;
                m_is_on_ground = false;
            }
            else if (velocity().y > 2.0F) {
                is_upwards = false;
                if (!sound_is_playing("water_enter") and !sound_is_playing("water_exit")) {
                    sound_position("water_enter", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_enter");
                    is_to_splash = true;
                }
            }

            if (std::abs(our_velocity.y) > std::abs(other_velocity.y)) {
                other.owner->velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }
            else {
                velocity_y(other_velocity.y * 0.5F + our_velocity.y * 0.5F);
            }


            if (is_to_splash) {
                Vec2F pos = Vec2F{ our_points.x, other_points.y };
                particle::spawn_fan(this, 225.0F, 315.0F, 8,
                                    particle::Type::drop_water, pos + Vec2F{ 0.0F, is_upwards ? -12.0F : -6.0F },
                                    Vec2F{ velocity().x * 0.9F, -std::abs(velocity().y * 0.1F) },
                                    is_upwards ? 2.0F : 1.5F);
            }
        }
    }
}