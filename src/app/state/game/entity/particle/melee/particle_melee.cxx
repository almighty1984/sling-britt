module;
#include <sstream>

module entity.particle.melee;
import app.config;
import console;

namespace entity {
    bool ParticleMelee::hurt(Object* culprit) {
        if (!culprit or is_dead()) return false;

        console::log("entity::ParticleMelee::hurt() culprit: ", to_string(culprit->type()), "\n");

        m_time_left_alive = 0;
        m_time_left_dead = m_time_to_be_dead;

        if (culprit->type() == Type::sling) {
            sound_position("bounce", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("bounce");
        }
        else {
            sound_position("hurt", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("hurt");
        }
        return true;
    }

    void ParticleMelee::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cVec2F our_velocity = velocity();

        if (other_type == Type::clip or other_type == Type::clip_ledge) {
            aabb::is_active(our.id, false);
        }
        else if (other_type == Type::clip_L or other_type == Type::clip_LD) {
            if (our_velocity.x > 0.0F) {
                aabb::is_active(our.id, false);
            }
        }
        else if (other_type == Type::clip_R or other_type == Type::clip_RD) {
            if (our_velocity.x < 0.0F) {
                aabb::is_active(our.id, false);
            }
        }
        else if (is_arch(other_type) or is_slope(other_type)) {
            if (other.owner->parent() and other.owner->parent()->type() == Type::frog) {
                //m_is_to_erase = true;
            }
        }
        else if (other.owner->type() == Type::brick) {
            //m_is_to_erase = true;
            //hurt(other.owner);
            other.owner->hurt(this);
            aabb::is_active(our.id, false);
        }
        else if (other_type == Type::bug or
            other_type == Type::frog or
            other_type == Type::player) {
            aabb::is_active(our.id, false);
            hurt(other.owner);
            other.owner->hurt(this);
        }
        else if (other_type == Type::mole) {
            if (other.owner->state() == state::Type::idle) return;
            aabb::is_active(our.id, false);
            hurt(other.owner);
            other.owner->hurt(this);
        }
        else if (other_type == Type::particle_health) {
            if (m_parent->type() == Type::frog) {
                other.owner->velocity(-our_velocity / 4.0F);
                m_is_to_erase = true;
            }
        }
        else if (other_type == Type::particle_melee) {
            if (other.owner->parent()->type() == Type::frog) {
                other.owner->parent()->next_state(state::Type::blocked);
                m_is_to_erase = true;
                //other.owner->time_left_alive(0);
                //other.owner->time_left_dead(0);
            }
        }
        else if (other_type == Type::sling) {
            if (m_parent->type() == Type::player) {

                //if (m_parent->is_on_ground()) return;
                m_parent->next_state(state::Type::sling);
                m_parent->parent(other.owner);

                //cVec2F rect_size = { other_rect.w - other_rect.x, other_rect.h - other_rect.y };
                //m_parent->sling_level_offset = Vec2F{ other_rect.x, other_rect.y } + camera::position - rect_size / 2;

                //velocity({});

                hurt(other.owner);
            }
        }
    }
    void ParticleMelee::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (is_dead() or m_is_to_erase or !m_parent or m_parent == other.owner or m_parent->is_blocked()) return;

        cType other_type = other.owner->type();

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cVec2F our_velocity = velocity();

        if (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {
            if (our_velocity.y < 0.0F) {
                aabb::is_active(our.id, false);
            }
        }
        else if (other_type == Type::clip_U or is_slope(other_type)) {
            if (our_velocity.y > 0.0F) {
                aabb::is_active(our.id, false);
            }
        }
        else if (other_type == Type::particle_health) {
            collide_x(our, other);
        }
        else if (other_type == Type::particle_melee) {
            collide_x(our, other);
        }
    }
}