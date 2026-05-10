module entity.particle.down_thrust;
import app.config;
import particle_system;

namespace entity {
    void ParticleDownThrust::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_time_left_alive == 0 or is_dead() or !m_parent or m_parent == other.owner or other.owner->is_dead()) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();

        cVec2F our_velocity = velocity();
        cVec2F other_velocity = other.owner->velocity();

        if (other_type == Type::particle_down_thrust) return;

        //console::log("type: ", to_string(other_type), "\n");

        switch (other_type) {
            case Type::bug:
            case Type::frog:
            case Type::mole: {
                if (other.owner->is_hurting()) return;

                cF32 our_w = our_DR.x - our_UL.x;
                cF32 our_h = our_DR.y - our_UL.y;

                cF32 other_w = other_DR.x - other_UL.x;
                cF32 other_h = other_DR.y - other_UL.y;

                cVec2F our_mid_point = position() + Vec2F{ our_w / 2.0F, our_h / 2.0F };
                cVec2F other_mid_point = other.owner->position() + Vec2F{ other_w / 2.0F, other_h / 2.0F };

                cVec2F hit_pos = Vec2F(our_mid_point + other_mid_point) / 2.0F - Vec2F{ our_w / 2.0F, our_h / 2.0F };

                other.owner->hurt(this);
                velocity({});
                m_time_left_alive = 0;
                m_time_left_dead = m_time_to_be_dead;

                sound_position("melee", { position().x - app::config::extent().x / 2.0F,
                                          position().y - app::config::extent().y / 2.0F });
                sound_play("melee");

                particle::spawn(this, particle::Type::hit, hit_pos, {});
                //particle::spawn({ this, particle::Type::hit, position(), {} });
                break;
            }
            case Type::particle_brick: {
                console::log(class_name(), "::collide_x() vel: ", our_velocity.x, " ", our_velocity.y, "\n");
                break;
            }
        }
    }
    void ParticleDownThrust::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}