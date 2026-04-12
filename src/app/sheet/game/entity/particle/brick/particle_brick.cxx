module entity.particle.brick;
import app.config;
import particle_system;

namespace entity {
    void ParticleBrick::collide_x(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_points.x < other_points.x ? our_points.w - other_points.x : -(other_points.w - our_points.x);

        cVec2F other_size = { other_points.w - other_points.x, other_points.h - other_points.y };

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        if (other_type == Type::arch_L_1x1 or other_type == Type::arch_R_1x1) {
            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::arch_L_2x1_0 or other_type == Type::arch_L_2x1_1 or
            other_type == Type::arch_R_2x1_0 or other_type == Type::arch_R_2x1_1) {
            velocity_x(velocity().x * -0.75F);
        }
        else if (other_type == Type::brick or other_type == Type::bug) {
            //console::log(class_name(), "::collide_x ", to_string(other_type), "\n");
            if (is_hurting() or (std::abs(other_velocity.x) < 0.5F and std::abs(our_velocity.x) < 0.5F)) return;

            m_time_left_hurt = m_time_to_hurt;
            m_time_left_dead = m_time_to_be_dead;

            velocity_y(other_velocity.y - 2.0F);
            velocity_x(other_velocity.x * 1.2F);
        }
        else if (other_type == Type::clip or
            (other_type == Type::clip_L and velocity().x > 0.0F) or
            (other_type == Type::clip_R and velocity().x < 0.0F) or
            other_type == Type::clip_ledge
            ) {

            if (other_points.x < our_points.x and velocity().x > 0.0F or
                other_points.w > our_points.w and velocity().x < 0.0F) return;

            position_add_x(-overlap_x);
            velocity_x(velocity().x * -0.75F);
        }
        else if (other_type == Type::frog) {
            if (std::abs(velocity().x) >= 2.0F) {
                other.owner->hurt(this);
                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                sound_position("melee", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("melee");

                particle::spawn(this, particle::Type::hit, hit_pos, {});
            }
            position_add_x(-overlap_x);
            velocity_x(velocity().x * -1.0F);
        }
        else if (other_type == Type::mole) {
            if (is_hurting() or other.owner->is_hurting() or other.owner->state() == state::Type::swim or (velocity().x >= -1.0F and velocity().x <= 1.0F)) return;

            if (other.owner->state() == state::Type::idle) {
                m_time_left_hurt = m_time_to_hurt;
                velocity_y(other_velocity.y - 1.0F);
                velocity_x(other_velocity.x * 0.5F);
                return;
            }

            position_add_x(-overlap_x);
            velocity_x(velocity().x * -1.0F);

            cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
            sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
            sound_play("melee");
            particle::spawn(this, particle::Type::hit, hit_pos, {});

        }
        else if (other_type == Type::slope_L_1x1) {
            //position_add_x( -overlap_x );
            collide_y(our, other);
        }
        else if (other_type == Type::slope_R_1x1) {
            //position_add_x( -overlap_x );
            collide_y(our, other);
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            //position_add_x( -overlap_x );
            collide_y(our, other);
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            //position_add_x( -overlap_x );
            collide_y(our, other);
        }
        else if (other_type == Type::particle_brick) {
            //if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;

            //position_add_x( -overlap_x );
            //velocity_x(our_velocity.x * 0.4F + other_velocity.x * 0.4F;
            //other.owner->velocity_x(our_velocity.x * 0.5F + other_velocity.x * 0.5F;

            //velocity_x(other_velocity.x * 0.8F;
            //other.owner->velocity_x(our_velocity.x * 0.8F;

            /*if (std::abs(velocity().x) > std::abs(other.owner->velocity().x)) {
                other.owner->velocity_x(velocity().x;
            } else {
                velocity_x(other.owner->velocity().x;
            }*/
        }
        else if (other_type == Type::player) {
            if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;

            m_time_left_hurt = m_time_to_hurt;
            m_time_left_dead = m_time_to_be_dead;

            //position_add_x( -overlap_x );
            //position().y -= 2.0F;
            velocity_y(other_velocity.y - 2.0F);

            velocity_x(other_velocity.x * 1.2F);
            //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

            //other.owner->velocity().x *= 0.95F;
        }
        else if (is_water_line(other.owner->type())) {
            collide_y(our, other);
        }
    }
    void ParticleBrick::collide_y(aabb::cInfo our, aabb::cInfo other) {
        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        if (is_arch(other_type)) {
            if (velocity().y < 0.0F) {
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -1.0F);
            }
        }
        else if (other_type == Type::brick) {
            if (our_points.y > other_points.y) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.5F);
            if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                velocity_y(0.0F);
            }
            moved_velocity({});

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::clip or other_type == Type::clip_ledge) {
            if (velocity().y < 0.0F and our_points.h < other_points.h) return;
            if (velocity().y > 0.0F and our_points.y > other_points.y) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.5F);
            if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                velocity_y(0.0F);
            }
            moved_velocity({});

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::clip_U or other_type == Type::slope_U) {
            if (velocity().y < 0.0F) return;
            //if (velocity().y > 0.0F and our_points.y > other_points.h) return;

            position_add_y(-overlap_y);

            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }

            moved_velocity({});

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::clip_D or other_type == Type::clip_LD or other_type == Type::clip_RD) {
            if (velocity().y > 0.0F) return;
            //if (velocity().y < 0.0F and our_points.h < other_points.h) return;

            position_add_y(-overlap_y);
            velocity_y(velocity().y * -0.5F);
            moved_velocity({});

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::frog) {
            if (std::abs(velocity().y) >= 2.0F) {
                other.owner->hurt(this);
                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };

                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");

                particle::spawn(this, particle::Type::hit, hit_pos, {});
            }
            if (our_points.y < other_points.y) {
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -1.0F);
            }
        }
        else if (other_type == Type::mole) {
            if (other.owner->state() == state::Type::idle or (velocity().y >= -2.0F and velocity().y <= 2.0F)) return;
            if (our_points.y < other_points.y) {
                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                sound_position("melee", { hit_pos.x / 160.0F, hit_pos.y / 90.0F });
                sound_play("melee");
                particle::spawn(this, particle::Type::hit, hit_pos, {});

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -1.0F);
            }
        }
        else if (other_type == Type::slope_L_1x1) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }
            sprite::angle(m_sprite, 45.0f);

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::slope_R_1x1) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }
            sprite::angle(m_sprite, 135.0f);

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::slope_L_2x1_0 or other_type == Type::slope_L_2x1_1) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }
            sprite::angle(m_sprite, 67.5f);

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::slope_R_2x1_0 or other_type == Type::slope_R_2x1_1) {
            if (velocity().y < 0.0F) return;
            position_add_y(-overlap_y);
            if (velocity().y > 1.0F) {
                velocity_y(velocity().y * -0.5F);
            } else {
                velocity_y(0.0F);
            }
            sprite::angle(m_sprite, 112.5f);

            velocity_x(velocity().x * 0.5F);
        }
        else if (other_type == Type::player) {
            if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;
            m_time_left_hurt = m_time_to_hurt;

            position_add({ 0.0F, -2.0F });
            velocity_x(other_velocity.x * 1.2F);
            velocity_y(other_velocity.y - 1.0F);
        }
        else if (other_type == Type::particle_brick) {
            //if (is_hurting() or std::abs(other_velocity.x) < 1.0F) return;
            ////if (other_points.y > our_points.y) return;

            ////position_add_y( -overlap_y );
            ////velocity_x(other_velocity.y * 0.5F;
            ////other.owner->velocity_x(our_velocity.y * 0.5F;

            ////velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
            ////other.owner->velocity_y(our_velocity.y * 0.4F + other_velocity.y * 0.4F;
            //velocity_y(0.0F);


            //velocity_x(velocity().x * -0.9F);
        }
        else if (is_water_line(other_type)) {
            if (acceleration() == Vec2F{ 0.05F, 0.05F }) return;

            acceleration({ 0.05F, 0.05F });
            max_velocity({ 0.4F, 0.7F });

            if (velocity().y > 0.0F) {
                if (!sound_is_playing("water_enter")) {
                    sound_position("water_enter", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("water_enter");
                    particle::spawn_fan(this,
                        235.0F, 305.0F, 3,
                        particle::Type::drop_water,
                        position() + Vec2F{ -4.0F, 0.0F },
                        Vec2F{ velocity().x * 0.1F, -velocity().y * 0.1F }, 1.5F);
                }
            }

        }
    }
}