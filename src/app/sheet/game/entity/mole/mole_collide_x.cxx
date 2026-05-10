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

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        Vec2F our_extent = { aabb::UR(our.id).x - aabb::UL(our.id).x,
                             aabb::DR(our.id).y - aabb::DL(our.id).y };

        aabb::cName our_name = aabb::name(our.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F our_velocity = velocity() + moved_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->moved_velocity();

        if (m_state == state::Type::swim) {
            switch (other_type) {
                case Type::bug: {
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
                    break;
                }
                case Type::clip_L:
                case Type::clip_R:
                case Type::clip_ledge: {
                    position_add_x(-overlap_x);

                    m_force.x = 0.0F;
                    velocity_x(velocity().x * -1.0F);

                    if (other_type == Type::clip_L or (other_type == Type::clip_ledge and our_velocity.x > 0.0F)) {
                        if (our_UL.x < other_UL.x) {
                            if (velocity().x > -1.0F) {
                                velocity_x(-1.0F);
                            }
                        }
                    } else if (other_type == Type::clip_R or(other_type == Type::clip_ledge and our_velocity.x < 0.0F)) {
                        if (our_DR.x > other_DR.x) {
                            if (velocity().x < 1.0F) {
                                velocity_x(1.0F);
                            }
                        }
                    }

                    //if (m_time_left_colliding_with[Type::water_line] > 0) return;
                    //m_time_left_colliding_with[Type::water_line] = 5;

                    //console::log("yo\n");
                    //position_add_x(-2.0F);
                                

                    

                    //force_x(0.0F);
                    //force_x(-1.0F);

                    //moved_velocity_x(moved_velocity().x * -1.0F);
                    //moved_velocity_x(0.0F);
                    //force_x(force().x * -1.0F);
                    //force_x(0.0F);

                    //m_is_near_wall_R = true;
                    break;
                }
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

        switch (other_type) {
            case Type::brick: {
                if (m_state != state::Type::idle and other.owner->state() == state::Type::tossed) {
                    hurt(other.owner);
                }
                break;
            }        
            case Type::particle_brick: {
                if (m_state != state::Type::idle and !(other_velocity.x >= -1.0F and other_velocity.x <= 1.0F)) {
                    hurt(other.owner);
                }
                break;
            }
            case Type::particle_health: {
                //if (health_is_max()) return;
                //health_amount(health_max());
                break;
            }
            case Type::player: {
                collide_y(our, other);
                break;
            }
            case Type::level_L_0: {
                sprite_is_leftward(false);
                break;
            }
            case Type::level_R_0: {
                sprite_is_leftward(true);
                break;
            }        
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                //collide_y(our, other);
                break;
            }
        }
    }
}