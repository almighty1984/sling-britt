module entity.particle.down_thrust;
import app.config;
import particle_system;

namespace entity {
    void ParticleDownThrust::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_time_left_alive == 0 or is_dead() or !m_parent or m_parent == other.owner or other.owner->is_dead()) return;

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();

        if (other_type == Type::particle_down_thrust) return;

        //console::log("type: ", to_string(other_type), "\n");

        if (is_arch(other_type) or is_clip(other_type) or is_slope(other_type)) {
            //m_is_to_erase = true;
        }
        else if (other.owner->type() == Type::particle_sense) {

        }
        else if (other_type == Type::bug  or
                 other_type == Type::frog or
                 other_type == Type::mole) {

            if (other.owner->is_hurting()) return;

            cF32 our_w = our_points.w - our_points.x;
            cF32 our_h = our_points.h - our_points.y;

            cF32 other_w = other_points.w - other_points.x;
            cF32 other_h = other_points.h - other_points.y;

            cVec2F our_mid_point = position() + Vec2F{ our_w / 2.0F, our_h / 2.0F };
            cVec2F other_mid_point = other.owner->position() + Vec2F{ other_w / 2.0F, other_h / 2.0F };

            cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

            other.owner->hurt(this);
            velocity({});
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            sound_position("melee", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("melee");

            particle::spawn(this, particle::Type::hit, hit_pos, {});
            //particle::spawn({ this, particle::Type::hit, position(), {} });
        } else if (other_type == Type::sling) {
        }
    }
    void ParticleDownThrust::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}