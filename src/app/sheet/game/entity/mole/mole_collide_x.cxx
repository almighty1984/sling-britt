module entity.mole;
import console;
import aabb;

namespace entity {
    void Mole::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or !other.owner or other.owner->is_dead()) return;

        entity::cType other_type = other.owner->type();

        if (is_slope(other_type)) {
            collide_y(our, other);
            return;
        }

        aabb::cName other_name = aabb::name(other.id);

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        Vec2F our_dimensions = { aabb::point(our.id, 1).x - aabb::point(our.id, 0).x,
                                 aabb::point(our.id, 3).y - aabb::point(our.id, 1).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_points.x < other_points.x ? our_points.w - other_points.x : -(other_points.w - our_points.x);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (m_state == state::Type::swim) {
            if (other_type == Type::bug) {
                if (m_state == state::Type::swim and other.owner->state() == state::Type::swim) {                    

                    position_add_x(-overlap_x);
                    velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F);
                    other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F);

                    force_x(force().x - overlap_x);

                    /*if (force().x < 0.0F and other.owner->is_near_wall_L()) {
                        force_x(4.0F);
                    }
                    else if (force().x > 0.0F and other.owner->is_near_wall_R()) {
                        force_x(-4.0F);
                    }
                    else {
                        force_x(-other_velocity.x * 1.0F);
                        other.owner->force_x(our_velocity.x * 1.0F);
                    }*/
                }
            }
            else if ((other_type == Type::clip_L or other_type == Type::clip_ledge) and velocity().x > 0.0F) {
                //if (m_time_left_colliding_with[Type::water_line] > 0) return;
                //m_time_left_colliding_with[Type::water_line] = 5;

                //console::log("yo\n");
                position_add_x(-overlap_x);
                //position_add_x(-2.0F);
                                
                m_force.x = 0.0F;
                velocity_x(velocity().x * -1.0F);
                if (our_points.x < other_points.x) {
                    if (velocity().x > -1.0F) {
                        velocity_x(-1.0F);
                    }
                }

                //force_x(0.0F);
                //force_x(-1.0F);

                //moved_velocity_x(moved_velocity().x * -1.0F);
                //moved_velocity_x(0.0F);
                //force_x(force().x * -1.0F);
                //force_x(0.0F);

                //m_is_near_wall_R = true;
            }
            else if ((other_type == Type::clip_R or other_type == Type::clip_ledge) and velocity().x < 0.0F) {
                //if (m_time_left_colliding_with[Type::water_line] > 0) return;
                //m_time_left_colliding_with[Type::water_line] = 5;

                position_add_x(-overlap_x);
                //position_add_x(2.0F);

                m_force.x = 0.0F;
                velocity_x(velocity().x * -1.0F);
                if (our_points.w > other_points.w) {
                    if (velocity().x < 1.0F) {
                        velocity_x(1.0F);
                    }
                }
                

                //force_x(0.0F);
                //force_x(1.0F);

                //moved_velocity_x(moved_velocity().x * -1.0F);
                //moved_velocity_x(0.0F);
                //force_x(force().x * -1.0F);
                //force_x(0.0F);

                //m_is_near_wall_L = true;
            }
            sprite_is_leftward(velocity().x < 0.0F);
            return;            
        }
        else if (m_state == state::Type::jump) {
            if (velocity().x < 0.0F and (other_type == Type::clip_R or other_type == Type::clip_RD or other_type == Type::clip_ledge)) {
                position_add_x(-overlap_x);
                position_add_x(1.0F);
                velocity_x(velocity().x * -1.0F);
            }
            if (velocity().x > 0.0F and (other_type == Type::clip_L or other_type == Type::clip_LD or other_type == Type::clip_ledge)) {
                position_add_x(-overlap_x);
                position_add_x(-1.0F);
                velocity_x(velocity().x * -1.0F);
            }
            return;
        }

        if (other_type == Type::brick) {
            if (m_state != state::Type::idle and other.owner->state() == state::Type::tossed) {
                hurt(other.owner);
            }
        }        
        else if (other_type == Type::particle_brick) {
            if (m_state != state::Type::idle and !(other_velocity.x >= -1.0F and other_velocity.x <= 1.0F)) {
                hurt(other.owner);
            }
        }
        else if (other_type == Type::particle_health) {
            //if (health_is_max()) return;
            //health_amount(health_max());
        }
        else if (other_type == Type::player) {
            collide_y(our, other);
        }
        else if (other_type == Type::level_L_0) {
            sprite::is_leftward(m_sprite, false);
        }
        else if (other_type == Type::level_R_0) {
            sprite::is_leftward(m_sprite, true);
        }        
        else if (other_type == Type::water_line) {
            //collide_y(our, other);
        }
    }
}