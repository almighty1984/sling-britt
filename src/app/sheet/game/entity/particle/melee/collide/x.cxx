module entity.particle.melee;
import particle_system;
import anim;

namespace entity {
    void ParticleMelee::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or is_hurting() or m_is_to_erase or
            !other.owner or other.owner->is_dead() or
            !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F our_center = aabb::center(our.id);
        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
        cVec2F our_velocity = velocity();

        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);
        cVec2F other_velocity = other.owner->velocity();
        state::Type other_state = other.owner->state();

        bool is_to_spawn_hit = false;
        Vec2F hit_pos = { our_UL.x + our_extent.x / 2.0F - 8.0F,
                          our_UL.y + our_extent.y / 2.0F - 8.0F };

        switch (other_type) {
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_ledge_L_50: 
            case Type::clip_ledge_R_50: {
                aabb::is_active(our.id, false);
                break;
            }
            case Type::clip_L:
            case Type::clip_L_50:
            case Type::clip_LD: {
                if (our_velocity.x > 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::clip_R:
            case Type::clip_R_50:
            case Type::clip_RD: {
                if (our_velocity.x < 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::arch_L_1x1:
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_1x1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1:
            case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (other.owner->parent() and other.owner->parent()->type() == Type::frog) {
                    //m_is_to_erase = true;
                }
                break;
            }
            case Type::brick: {
                if (!aabb::is_active(our.id)) return;
                /*for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }*/
                if (!sound_is_playing("block")) {
                    is_to_spawn_hit = true;                    
                }
                
                aabb::is_active(our.id, false);
                hurt(other.owner);
                break;
            }
            case Type::bee:
            case Type::bee_hive:
            case Type::bug:
            case Type::frog:
            case Type::mole:
            case Type::player: {                
                if (other_type == Type::mole and other_state == state::Type::idle) return;
                //other.owner->velocity(other_velocity * 0.5F + our_velocity * 0.5F);
                //other.owner->velocity_x(our_velocity.x * 0.0F);
                aabb::is_active(our.id, false);
                other.owner->hurt(this);
                hurt(other.owner);
                is_to_spawn_hit = true;
                hit_pos = other.owner->position();

                velocity({});
                break;
            }
            case Type::particle_health: {
                if (m_parent->type() == Type::frog and !m_parent->health_is_max()) {
                    console::log(class_name(), "::collide_x() particle_health\n");
                    m_parent->next_state(state::Type::heal);
                    m_parent->time_to_be_in_state(200);
                    m_is_to_erase = true;
                }
                break;
            }
            case Type::particle_melee: {
                if (other.owner->parent()->type() == Type::frog) {
                    other.owner->parent()->next_state(state::Type::blocked);
                    //other.owner->time_left_alive(0);
                    //other.owner->time_left_dead(0);
                } else {
                    if (!other.owner->sound_is_playing("idle") and !other.owner->sound_is_playing("hit")) {
                        other.owner->time_left_alive(0);
                        other.owner->time_left_dead(1);
                    }
                }
                //m_is_to_erase = true;
                break;
            }
            case Type::sling: {
                if (m_parent->type() == Type::player) {
                    if (m_parent->next_state() == state::Type::sling) return;

                    aabb::is_active(our.id, false);
                    
                    // add time until next state based on distance from sling and how far from last anim frame
                    U16 added_time_until_sling = 0;

                    cI32 anim_id = m_parent->anim("melee");
                    cI32 frame_diff = anim::last_frame(anim_id) - anim::current_frame(anim_id);
                    //console::log(class_name(), "::collide_x() frame diff: ", frame_diff, "\n");

                    added_time_until_sling += frame_diff;
                    
                    cF32 y_diff = m_parent->position().y - our_UL.y;
                    //console::log(class_name(), "::collide_x() y diff: ", y_diff, "\n");
                    if (y_diff < 0.0F) {
                        added_time_until_sling -= (int)(y_diff / 2.0F);
                    }

                    cF32 x_diff = aabb::center(m_parent->aabb(aabb::Name::body)).x - our_center.x;
                    //console::log(class_name(), "::collide_x() x diff: ", x_diff, "\n");

                    added_time_until_sling += std::abs((int)(x_diff / 8.0F));

                    cVec2F move_towards_sling = Vec2F{ -x_diff, -y_diff } / 40.0F;
                    console::log(class_name(), "::collide_x() move_towards_sling: ", move_towards_sling.x, " ", move_towards_sling.y, "\n");
                    m_parent->velocity_add(move_towards_sling);

                    m_parent->time_left_until_next_state(added_time_until_sling);
                    m_parent->next_state(state::Type::sling);
                    m_parent->parent(other.owner);

                    //cVec2F rect_size = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };
                    //m_parent->sling_level_offset = Vec2F{ other_UL.x, other_UL.y } + camera::position - rect_size / 2;

                    //velocity({});

                    hurt(other.owner);
                }
                break;
            }
        }

        if (is_to_spawn_hit) {
            console::log(class_name(), " to spawn hit\n");
            particle::spawn(this,
                            particle::Type::hit,
                            hit_pos,
                            {});
        }
    }
}