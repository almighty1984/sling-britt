module entity.particle.sense;

namespace entity {
    void ParticleSense::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (!m_parent or other.owner->is_dead() or m_parent == other.owner or m_parent->is_blocked()) return;
        cVec2F other_velocity = other.owner->velocity();
        
        if (other_velocity.y >= 5.0F) {
            //console::log("too fast!\n");
            return;
        }
        cType other_type = other.owner->type();

        /* if (m_parent->type() == Type::mole) {
             console::log("type: ", to_string(m_parent->type()), "\n");
         }*/

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1:
            case Type::clip:
            case Type::clip_U:
            case Type::clip_D:
            case Type::clip_L:
            case Type::clip_R:
            case Type::clip_LD:
            case Type::clip_RD:
            case Type::clip_ledge:
            case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1:
            case Type::slope_U:
            case Type::particle_down_thrust: {
                m_is_to_erase = true;
                break;
            }
            case Type::brick:
            case Type::bug:
            case Type::frog:
            case Type::water_line:
            case Type::particle_health:
            case Type::particle_rock:
            case Type::player: {
                if (m_parent->is_reacting()) return;

                m_parent->add_sensed(other.owner);
                //m_parent->sensed_position(position());
                m_is_to_erase = true;
                break;
            }
            case Type::particle_brick: {
                if (!(other_velocity.x > -0.2F and other_velocity.x < 0.2F and other_velocity.y > -2.0F and other_velocity.y < 2.0F)) {
                    m_parent->add_sensed(other.owner);
                    m_is_to_erase = true;
                }
                break;
            }            
        }
    }
    void ParticleSense::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}