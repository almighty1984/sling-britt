module entity.particle.melee;
import app.config;
import console;
import particle_system;

namespace entity {
    bool ParticleMelee::hurt(Object* culprit) {
        if (!culprit or is_dead()) return false;

        console::log(class_name(), "::hurt() culprit: ", to_string(culprit->type()), "\n");
        console::log(class_name(), "::hurt() time to be dead: ", m_time_to_be_dead, "\n");

        m_time_left_alive = 0;
        m_time_left_dead = m_time_to_be_dead;

        switch (culprit->type()) {
            case Type::bee:
            case Type::bug:
            case Type::frog: {
                sound_position("hit", { position().x - app::config::extent().x / 2.0F,
                                        position().y - app::config::extent().y / 2.0F });
                sound_play("hit");
                break;
            }
            case Type::sling: {
                sound_position("sling", { position().x - app::config::extent().x / 2.0F,
                                          position().y - app::config::extent().y / 2.0F });
                sound_play("sling");
                break;
            }
        }        
        return true;
    }
    void ParticleMelee::state_idle(cF32 dt) {
        sound_position("idle", { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F });
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //console::log("run this\n");
            sound_play("idle");
        }
    }
    void ParticleMelee::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;            
        }
        //console::log(class_name(), "::state_dead() time left dead: ", m_time_left_dead, "\n");
        velocity({});
    }

    void ParticleMelee::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        state::Type other_state = other.owner->state();

        bool is_to_spawn_hit = false;
        Vec2F hit_pos = { our_UL.x + our_extent.x / 2.0F - 8.0F,
                          our_UL.y + our_extent.y / 2.0F - 8.0F };

        switch (other_type) {            
            case Type::clip:
            case Type::clip_ledge: {
                aabb::is_active(our.id, false);
                break;
            }
            case Type::clip_L:
            case Type::clip_LD: {
                if (our_velocity.x > 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::clip_R:
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
                aabb::is_active(our.id, false);
                other.owner->velocity_x(our_velocity.x * 1.0F);
                hurt(other.owner);
                other.owner->hurt(this);
                is_to_spawn_hit = true;
                break;
            }
            case Type::bee:
            case Type::bug:
            case Type::frog:
            case Type::mole:
            case Type::player: {
                other.owner->velocity_x(other_velocity.x * 0.5F + our_velocity.x * 0.5F);
                //aabb::is_active(our.id, false);
                hurt(other.owner);
                other.owner->hurt(this);
                is_to_spawn_hit = true;
                hit_pos = other.owner->position();

                velocity({});
                break;
            }
            case Type::particle_health: {
                if (m_parent->type() == Type::frog and !m_parent->health_is_max()) {
                    console::log(class_name(), "::collide_x() particle_health\n");                    
                    m_parent->next_state(state::Type::heal);
                    m_parent->time_left_in_next_state(200);                    
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
                    other.owner->time_left_alive(0);
                    other.owner->time_left_dead(1);
                }
                //m_is_to_erase = true;
                break;
            }        
            case Type::sling: {
                if (m_parent->type() == Type::player) {

                    //if (m_parent->is_on_ground()) return;
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
            console::log(class_name(), "extent: ", our_extent.x, " ", our_extent.y, "\n");
            particle::spawn(this,
                            particle::Type::hit,
                            hit_pos,
                            {});
        }
    }
    void ParticleMelee::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_velocity = velocity();

        switch (other_type) {
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (our_velocity.y < 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::clip_U:
            case Type::slope_L_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_1x1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                if (our_velocity.y > 0.0F) {
                    aabb::is_active(our.id, false);
                }
                break;
            }
            case Type::particle_health: {
                collide_x(our, other);
                break;
            }
            case Type::particle_melee: {
                collide_x(our, other);
                break;
            }
        }
    }
}