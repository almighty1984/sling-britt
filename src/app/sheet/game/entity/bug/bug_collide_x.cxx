module entity.bug;
import aabb;
import console;
import sound;
import particle_system;

namespace entity {
    void Bug::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent or is_dead() or !other.owner or other.owner->is_dead()) return;

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };


        if (is_clip(other_type) or
            (other_type == Type::brick and !other.owner->is_tossed())) {
            //console::log(to_string(other_type), " has parent\n");
            if (our_points.w > other_points.w) {
                m_is_near_wall_L = true;
            }
            if (our_points.x < other_points.x) {
                m_is_near_wall_R = true;
            }
        }
        if (m_parent) {
            return;
        }

        //console::log(class_name(), "::collide_x() ", to_string(other_type), "\n");

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        cF32 overlap_x = our_points.x < other_points.x ? our_points.w - other_points.x : -(other_points.w - our_points.x);
        //cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);




        if (m_state == state::Type::tossed) {
            
            if (is_arch(other_type) or
                other_type == Type::brick or
                other_type == Type::clip or                
                (other_type == Type::clip_LD and our_velocity.x > 0.0F) or
                (other_type == Type::clip_RD and our_velocity.x < 0.0F) or
                other_type == Type::clip_ledge) {
                console::log(class_name(), " collide_x() tossed ", to_string(other_type), "\n");
                if (!is_hurting()) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    m_next_state = state::Type::hurt;
                    
                    position_add_x(-overlap_x);
                    //velocity_x(other_velocity.x * 0.9F);
                    //other.owner->velocity_x(our_velocity.x * 0.9F);
                    velocity_x(0.0F);
                    
                    cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                       (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    
                }
                return;
            }
        }


        if (is_arch(other_type)) {
            if (m_parent or other.owner->parent() or is_hurting()) return;
            console::log(class_name(), "::collide_x ", to_string(other_type), "\n");
            position_add_x(-overlap_x);
            if (std::abs(our_velocity.x - other_velocity.x) >= 2.0F) {

                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                return;
            }
        }
        else if (other_type == Type::bug) {
            if (other.owner->parent()) return;

            if (m_state == state::Type::upended or m_state == state::Type::bounce) {
                if (other_velocity.y > 1.0F) {
                    m_state = state::Type::upended;
                    m_next_state = state::Type::bounce;
                    return;
                }
            }
            position_add_x(-overlap_x);
            velocity_x(velocity().x * -1.0F);
            moved_velocity_x(0.0F);
            sprite::is_leftward(m_sprite, !sprite::is_leftward(m_sprite));
        }
        else if (other_type == Type::brick) {
            if (m_parent or other.owner->parent()) return;
            
            position_add_x(-overlap_x);

            if (m_state == state::Type::walk) {
                console::log(class_name(), "::collide_x() brick turn\n");
                
                
                velocity_x(our_velocity.x * -0.9F);
                moved_velocity_x(0.0F);
                sprite_is_leftward(!sprite_is_leftward());
                //time_left_colliding_with(other_type, 4);
            }


            //if (m_parent or other.owner->parent()) return;
            console::log(class_name(), "::collide_x ", to_string(other_type), "\n");

            
            
            //aabb::time_left_colliding_x(our.id, 100);

            if (std::abs(other_velocity.x) >= 1.0F and other.owner->state() == state::Type::carried) {
                velocity_x(other_velocity.x);
                velocity_y(-2.0F);
            }
        }
        else if (other_type == Type::clip or other_type == Type::clip_ledge) {
            if (our_points.h > other_points.y and our_points.h - other_points.y < 2.0F) return;
            //if ((velocity().x < 0.0F and !m_is_near_wall_L) or
            //    (velocity().x > 0.0F and !m_is_near_wall_R)) {
            position_add_x(-overlap_x);
            //}

            moved_velocity_x(0.0F);
            //if (m_time_left_turning > 0 or !m_is_on_ground) return;

            if (m_state == state::Type::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                m_force.x = 0.0F;
                velocity_x(velocity().x * -1.0F);
                if (our_points.x < other_points.x) {
                    if (velocity().x > -1.0F) {
                        velocity_x(-1.0F);
                    }
                }
                else if (our_points.w > other_points.w) {
                    if (velocity().x < 1.0F) {
                        velocity_x(1.0F);
                    }
                }
                return;
            }
            
            if (m_state == state::Type::upended) {
                console::log(class_name(), "::collide_x velocity ", velocity().x, "\n");
                if (velocity().x < 0.0F) {
                    m_is_near_wall_L = true;
                }
                else if (velocity().x > 0.0F) {
                    m_is_near_wall_R = true;
                }
                velocity_x(0.0F);
            }
            if (m_state == state::Type::walk) {
                if (velocity().x < 0.0F) {
                    if (our_points.x > other_points.x) {
                        velocity_x(1.0F);
                        //position().x += 2.0F;
                        //m_time_left_turning = 30;
                    }
                }
                else if (velocity().x > 0.0F) {

                    if (our_points.x < other_points.x) {
                        velocity_x(-1.0F);
                        //position().x -= 2.0F;
                        //m_time_left_turning = 30;
                    }
                }
                sprite::is_leftward(m_sprite, velocity().x < 0.0F);
            }
        }
        else if (other_type == Type::clip_L or other_type == Type::clip_LD) {
            if (velocity().x + moved_velocity().x < 0.0F) return;
            position_add_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == state::Type::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                                                
                velocity_x(velocity().x * -1.0F);

                m_force.x = 0.0F;
                if (velocity().x > -1.0F) {
                    velocity_x(-1.0F);
                }

                return;
            }

            if (m_state == state::Type::tossed or
                m_state == state::Type::upended and (velocity().x > 1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                        (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = state::Type::hurt;
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            velocity_x(std::abs(velocity().x) * -1.0F);
            sprite::is_leftward(m_sprite, !sprite::is_leftward(m_sprite));

        }
        else if (other_type == Type::clip_R or other_type == Type::clip_RD) {
            if (velocity().x + moved_velocity().x > 0.0F) return;

            position_add_x(-overlap_x);
            moved_velocity_x(0.0F);

            if (m_state == state::Type::swim) {
                console::log(class_name(), "::collide_x swim ", to_string(other_type), "\n");
                
                m_force.x = 0.0F;

                velocity_x(velocity().x * -1.0F);
                m_force.x = 0.0F;
                if (velocity().x < 1.0F) {
                    velocity_x(1.0F);
                }
                return;
            }

            if (m_state == state::Type::tossed or
                m_state == state::Type::upended and (velocity().x < -1.0F)) {
                velocity_x(velocity().x * -0.9F);
                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                hurt(other.owner);
                m_next_state = state::Type::hurt;

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
            velocity_x(std::abs(velocity().x) * 1.0F);
            sprite::is_leftward(m_sprite, !sprite::is_leftward(m_sprite));

        }
        else if (other_type == Type::frog) {
            position_add_x(-overlap_x);
            if (m_state == state::Type::tossed) {
                cVec2F hit_pos = { (our_points.x < other_points.x ? other_points.x : other_points.w) - 8.0F,
                                   (our_points.y < other_points.y ? other_points.y : other_points.h) - 8.0F };

                velocity_x(velocity().x * -0.9F);
                particle::spawn(this, particle::Type::hit, hit_pos, velocity());
                hurt(other.owner);
                other.owner->hurt(this);
                m_next_state = state::Type::hurt;

                //velocity().y *= -1.0f;

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                return;
            }
        }
        else if (other_type == Type::mole) {
            if (m_state == state::Type::swim and other.owner->state() == state::Type::swim) {                

                position_add_x(-overlap_x);

                velocity_x(-other_velocity.x * 0.5F);
                //other.owner->velocity_x(our_velocity.x * 0.5F);

                force_x(force().x - overlap_x);

                /*if (force().x < 0.0F and other.owner->is_near_wall_L()) {
                    force_x(10.0F);
                    console::log("near L\n");
                }
                else if (force().x > 0.0F and other.owner->is_near_wall_R()) {
                    force_x(-10.0F);
                    console::log("near R\n");
                }
                else {
                    force_x(-other_velocity.x * 1.0F);
                    other.owner->force_x(our_velocity.x * 1.0F);
                }*/
            }
        }
        else if (other_type == Type::particle_interact) { }
        else if (other_type == Type::player) {
            //console::log("entity::Bug::collide_x player overlap_x: ", overlap_x, "\n");
            /*if (other_velocity.y > 3.0F) {
                collide_y(our, other);
                return;
            }*/
            
            if (m_state == state::Type::tossed) return;
            //console::log("aabb name: ", aabb::to_string(other_name), "\n");

            if (other_points.h < our_points.y + 4.0F) return;

            /*if (!m_is_near_wall_L and !m_is_near_wall_R) {
                position_add_x(-overlap_x);
                velocity_x(other_velocity.x);
            } else {
                if (m_is_near_wall_L) {
                    other.owner->position_add({ 2.0F, 0.0F });
                    other.owner->velocity_x(2.0F);
                } else if (m_is_near_wall_R) {
                    other.owner->position_add({ -2.0F, 0.0F });
                    other.owner->velocity_x(-2.0F);
                }
            }*/

            if (other.owner->state() == state::Type::sling) {
                velocity_x(-other.owner->rotation_speed() * 0.5F);
                hurt(other.owner);
                return;
            }

            position_add_x(-overlap_x);
            
            if (m_state == state::Type::upended or m_state == state::Type::bounce) {
                velocity_x(other_velocity.x);
                other.owner->position_add_x(overlap_x);                
            }
            else if (m_state == state::Type::walk) {
                bool prev_is_left = sprite::is_leftward(m_sprite);

                cVec2F vel_sum = our_velocity + other_velocity;

                bool is_left = -vel_sum.x < 0.0F;

                console::log(class_name(), "::collide_x() player velocity sum: ", vel_sum.x, "\n");

                if (vel_sum.x <= 0.1F and !sprite::is_leftward(m_sprite)) {
                    sprite::is_leftward(m_sprite, true);
                    velocity_x(-1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                }
                else if (vel_sum.x >= -0.1F and sprite::is_leftward(m_sprite)) {
                    sprite::is_leftward(m_sprite, false);
                    velocity_x(1.0F);
                    velocity_y(-1.0F);
                    other.owner->velocity_x(velocity().x * -0.9F);
                }
                else {
                    if (prev_is_left and velocity().x > 0.0F or !prev_is_left and velocity().x < 0.0F) {
                        sprite::is_leftward(m_sprite, !sprite::is_leftward(m_sprite));
                    }                    
                    other.owner->velocity_x(other.owner->velocity().x * -0.9F + our_velocity.x * 2.0F);
                    other.owner->velocity_y(-1.0F);
                }
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            //if (our_points.h > other_points.h) return;

            //velocity_y(-velocity().x;
            position_add({ 0.0F, -velocity().x });
            /*if (velocity().x > max_velocity().x * 0.7F) {
                velocity_x(max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_1x1) {
            //if (our_points.h > other_points.h) return;

            //velocity_y(velocity().x);
            position_add({ 0.0F, velocity().x });
            /*if (velocity().x < -max_velocity().x * 0.7F) {
                velocity_x(-max_velocity().x * 0.7F;
            }*/
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            moved_velocity({});
            if (velocity().x > 0.0F) {
                velocity_y(-velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            moved_velocity({});
            if (velocity().x < 0.0F) {
                velocity_y(velocity().x / 2.0F);
            }
            m_is_on_ground = true;
            m_is_on_slope = true;
        }
        else if (other_type == Type::water_line) {
            collide_y(our, other);
        }
    }
}