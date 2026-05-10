module entity.particle.rock;
import app.config;
import particle_system;

namespace entity {
    bool ParticleRock::hurt(Object* culprit) {
        if (!culprit or m_state == state::Type::dead or m_next_state == state::Type::dead) return false;
        m_next_state = state::Type::dead;

        particle::spawn(this,
                        particle::Type::hit,
                        position() - Vec2F{ 6.0F, 6.0F } + velocity(),
                        {});

        cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                             position().y - app::config::extent().y / 2.0F };

        console::log(class_name(), "::hurt() sound position: ", sound_pos.x, " ", sound_pos.y, "\n");

        sound_position("dead", sound_pos);
        sound_play("dead");

        switch (culprit->type()) {
            case Type::bee:
            case Type::bug:
            //case Type::brick:
            case Type::frog:
            case Type::player: {
                sound_position("hit", sound_pos);
                sound_play("hit");
                break;
            }
        }
        return true;
    }
    void ParticleRock::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sound_play("toss");
        }

        sound_position("toss", { position().x - app::config::extent().x / 2.0F,
                                 position().y - app::config::extent().y / 2.0F });



        m_radians = std::atan2f(velocity().y, velocity().x);
        if (m_radians < 0.0F) m_radians += (2 * PI);

        cF32 angle = m_radians * (180.0F / PI);

        //console::log("angle: ", angle, "\n");

        //if (m_time_in_state <= 4) {
        ++m_time_in_state;
        //}
        if (m_time_in_state == 2) {
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
        }


        if (m_time_in_state > 2) {
            m_time_in_state = 2;
            if (velocity().x < -2.0F or velocity().x > 2.0F or
                velocity().y < -2.0F or velocity().y > 2.0F) {
                particle::spawn(this, particle::Type::trail_smoke, position() - Vec2F{ 4.0F, 4.0F } - velocity(), {});
            }
        }

        m_radians = std::atan2f(velocity().y, velocity().x);
        if (m_radians < 0.0F) m_radians += (2 * PI);

        cF32 degrees = m_radians * (180 / PI);

        sprite::angle(m_sprite, degrees);
    }
    void ParticleRock::state_dead(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_alive = 0;
            m_time_left_dead = m_time_to_be_dead;

            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            sprite_is_hidden(true);

            //m_radians += (2 * PI);

            m_radians = std::atan2f(velocity().y, velocity().x);
            if (m_radians < 0.0F) m_radians += (2 * PI);

            cF32 angle = m_radians * (180 / PI);

            console::log(class_name(), "::state_dead() angle: ", angle, "\n");

            cF32 start_angle = angle - 45.0F;
            cF32 end_angle = angle + 45.0F;
            cF32 speed = 1.0F;
            particle::spawn_fan(this,
                start_angle, end_angle, 3,
                particle::Type::pebble,
                position(), velocity() * 0.1F, speed,
                state::Type::idle);

            //int num_to_spawn = 8;
            //F32 radian_offset = -(PI / num_to_spawn);




            /*for (int i = 0; i < num_to_spawn; i++) {
                particle::spawn(this, particle::Type::pebble, position(), Vec2F{ std::cosf(m_radians + radian_offset) * speed, std::sinf(m_radians + radian_offset) * speed });
                radian_offset += (PI / num_to_spawn);
            }*/

            //state::Type::idle

        }
        velocity({});
        if (m_parent and is_water_line(m_parent->type())) {
            position_y(m_parent->position().y - 8.0F);
        }
    }
    void ParticleRock::collide_x(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent == other.owner or m_state == state::Type::dead) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_x = our_UL.x < other_UL.x ? our_DR.x - other_UL.x : -(other_DR.x - our_UL.x);

        cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_R_1x1: {
                velocity_x(velocity().x * 0.5F);
                hurt(other.owner);
                break;
            }
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                velocity_x(velocity().x * -0.75F);
                hurt(other.owner);
                break;
            }
            case Type::bee:{
                other.owner->velocity_x(our_velocity.x * 0.3F);
                other.owner->hurt(this);
                hurt(other.owner);
                break;
            }
            case Type::brick: {
                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -1.0F);
                other.owner->velocity_x(our_velocity.x * 0.3F);
                other.owner->hurt(this);
                hurt(other.owner);
                //console::log(class_name(), "::collide_x() brick ", other.owner->velocity().x, "\n");
                break;
            }
            case Type::bug: {
                //console::log(class_name(), "::collide_x() bug\n");
                position_add_x(-overlap_x);
                velocity_x(our_velocity.x * -1.0F);
                other.owner->velocity_x(our_velocity.x * 0.1F);
                other.owner->hurt(this);
                hurt(other.owner);
                break;
            }
            case Type::clip:
            case Type::clip_L:
            case Type::clip_LD:
            case Type::clip_R:
            case Type::clip_RD:
            case Type::clip_ledge:
            case Type::clip_LR: {
                if (other_UL.x < our_UL.x and velocity().x > 0.0F or
                    other_DR.x > our_DR.x and velocity().x < 0.0F) return;

                position_add_x(-overlap_x);
                velocity_x(velocity().x * -0.75F);
                hurt(other.owner);
                break;
            }
            case Type::frog: {
                position_add_x(-overlap_x);
                velocity_x(velocity().x * -1.0F);
                other.owner->hurt(this);
                hurt(other.owner);
                break;
            }
            case Type::mole: {
                if (std::abs(velocity().x) < 2.0F) {
                    return;
                }
                if (other.owner->state() == state::Type::idle) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    velocity_y(other_velocity.y - 1.0F);
                    velocity_x(other_velocity.x * 0.5F);
                    return;
                }

                position_add_x(-overlap_x);
                velocity_x(velocity().x * -1.0F);

                cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                particle::spawn(this, particle::Type::hit, hit_pos, {});
                break;
            }
            case Type::slope_L_1x1:
            case Type::slope_R_1x1:
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1:
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                position_add_x(-overlap_x);
                collide_y(our, other);
                break;
            }
            case Type::particle_brick: {
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
            case Type::player: {
                if (std::abs(velocity().x) < 2.0F or m_state == state::Type::dead) {
                    return;
                }

                //position_add_x( -overlap_x );
                //position().y -= 2.0F;
                velocity_y(other_velocity.y - 2.0F);

                velocity_x(other_velocity.x * 1.2F);
                //other.owner->velocity_x(other_velocity.x * 0.8F + our_velocity.x * 0.2F;

                //other.owner->velocity().x *= 0.95F;
                other.owner->hurt(this);
                hurt(other.owner);
                break;
            }
            case Type::train: {
                return;
                if (other_name != aabb::Name::body) return;
                if (other_UL.x < our_UL.x and velocity().x > 0.0F or
                    other_DR.x > our_DR.x and velocity().x < 0.0F) return;

                position_add_x(-overlap_x);
                velocity_x(velocity().x * -0.75F);

                hurt(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                collide_y(our, other);
                break;
            }
        }
    }
    void ParticleRock::collide_y(aabb::cInfo our, aabb::cInfo other) {
        if (m_parent == other.owner or m_state == state::Type::dead or m_next_state == state::Type::dead) return;

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };

        cType other_type = other.owner->type();
        aabb::cName   other_name = aabb::name(other.id);

        cF32 overlap_y = our_UL.y < other_UL.y ? our_DR.y - other_UL.y : -(other_DR.y - our_UL.y);

        cVec2F other_velocity = other.owner->velocity();
        cVec2F our_velocity = velocity();

        switch (other_type) {
            case Type::arch_L_1x1:
            case Type::arch_R_1x1: {
                velocity_x(velocity().x * 0.5F);
                hurt(other.owner);
                break;
            }
            case Type::arch_L_2x1_0:
            case Type::arch_L_2x1_1:
            case Type::arch_R_2x1_0:
            case Type::arch_R_2x1_1: {
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -1.0F);
                hurt(other.owner);
                break;
            }
            case Type::bee: {
                other.owner->velocity_y(our_velocity.y * 0.3F);
                other.owner->hurt(this);
                hurt(other.owner);
                break;
            }
            case Type::brick: {
                console::log(class_name(), "::collide_y() brick\n");
                position_add_y(-overlap_y);
                velocity_y(our_velocity.y * -0.5F);
                other.owner->velocity_y(our_velocity.y * 0.1F);
                hurt(other.owner);
                other.owner->hurt(this);
                break;
            }
            case Type::bug: {
                console::log(class_name(), "::collide_y() bug\n");
                position_add_y(-overlap_y);
                velocity_y(our_velocity.y * -0.5F);
                other.owner->velocity_y(our_velocity.y * 0.1F);
                hurt(other.owner);
                other.owner->hurt(this);
                break;
            }
            case Type::clip:
            case Type::clip_ledge:
            case Type::clip_UD: {
                if (our_velocity.y < 0.0F and our_DR.y < other_DR.y) return;
                if (our_velocity.y > 0.0F and our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::clip_U:
            case Type::slope_U: {
                if (our_velocity.y < 0.0F) return;
                //if (velocity().y > 0.0F and our_UL.y > other_DR.y) return;

                position_add_y(-overlap_y);
                if (position().y > other_UL.y) {
                    position_add_y(other_UL.y - position().y - our_extent.y);
                }

                if (velocity().y > 1.0F) {
                    velocity_y(velocity().y * -0.5F);
                } else {
                    velocity_y(0.0F);
                }

                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::clip_D:
            case Type::clip_LD:
            case Type::clip_RD: {
                if (velocity().y > 0.0F) return;
                //if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::frog: {
                console::log(class_name(), "::collide_y() frog\n");
                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);

                hurt(other.owner);
                other.owner->hurt(this);
                break;
            }
            case Type::mole: {
                if (other.owner->state() == state::Type::idle or (velocity().y >= -2.0F and velocity().y <= 2.0F)) return;
                if (our_UL.y < other_UL.y) {
                    hurt(other.owner);
                    other.owner->hurt(this);
                    cVec2F hit_pos = position() + Vec2F{ -8.0F, -8.0F };
                    particle::spawn(this, particle::Type::hit, hit_pos, {});
                    position_add_y(-overlap_y);
                    velocity_y(velocity().y * -1.0F);
                }
                hurt(other.owner);
                break;
            }
            case Type::slope_L_1x1: {
                position_add_y(-overlap_y);
                if (position().y > other_UL.y) {
                    position_add_y(other_UL.y - position().y - our_extent.y);
                }
                velocity_y(std::abs(velocity().y) * -1.0F);
                if (velocity().y > -4.0F) {
                    velocity_y(-4.0F);
                }
                sprite::angle(m_sprite, 45.0f);

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::slope_R_1x1: {
                position_add_y(-overlap_y);
                if (position().y > other_UL.y) {
                    position_add_y(other_UL.y - position().y - our_extent.y);
                }
                velocity_y(std::abs(velocity().y) * -1.0F);
                if (velocity().y > -4.0F) {
                    velocity_y(-4.0F);
                }
                sprite::angle(m_sprite, 135.0f);

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::slope_L_2x1_0:
            case Type::slope_L_2x1_1: {
                position_add_y(-overlap_y);

                if (position().y > other_UL.y) {
                    position_add_y(other_UL.y - position().y - our_extent.y);
                }

                velocity_y(std::abs(velocity().y) * -1.0F);
                if (velocity().y > -4.0F) {
                    velocity_y(-4.0F);
                }

                sprite::angle(m_sprite, 67.5f);

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::slope_R_2x1_0:
            case Type::slope_R_2x1_1: {
                position_add_y(-overlap_y);
                if (position().y > other_UL.y) {
                    position_add_y(other_UL.y - position().y - our_extent.y);
                }

                velocity_y(std::abs(velocity().y) * -1.0F);
                if (velocity().y > -4.0F) {
                    velocity_y(-4.0F);
                }

                sprite::angle(m_sprite, 112.5f);

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::player: {
                if (m_state == state::Type::dead or std::abs(other_velocity.x) < 2.0F) return;
                hurt(other.owner);
                other.owner->hurt(this);

                position_add({ 0.0F, -2.0F });
                velocity_x(other_velocity.x * 1.2F);
                velocity_y(other_velocity.y - 1.0F);
                break;
            }
            case Type::train: {
                return;
                if (other_name != aabb::Name::body) return;
                if (velocity().y < 0.0F and our_DR.y < other_DR.y) return;
                if (velocity().y > 0.0F and our_UL.y > other_UL.y) return;

                position_add_y(-overlap_y);
                velocity_y(velocity().y * -0.5F);
                if (velocity().y >= -acceleration().y and velocity().y <= acceleration().y) {
                    velocity_y(0.0F);
                }
                moved_velocity({});

                velocity_x(velocity().x * 0.5F);

                hurt(other.owner);
                break;
            }
            case Type::water_line_L:
            case Type::water_line_R:
            case Type::water_line: {
                if (acceleration() == Vec2F{ 0.05F, 0.05F }) return;

                acceleration({ 0.05F, 0.05F });
                max_velocity({ 0.4F, 0.7F });

                if (our_velocity.y <= 0.0F) return;
                if (!sound_is_playing("water_enter")) {
                    sound_position("water_enter", { position().x - app::config::extent().x / 2.0F,
                                                    position().y - app::config::extent().y / 2.0F });
                    sound_play("water_enter");
                    particle::spawn_fan(this,
                        235.0F, 305.0F, 3,
                        particle::Type::drop_water,
                        position() + Vec2F{ -4.0F, 0.0F },
                        Vec2F{ our_velocity.x * 0.1F, -our_velocity.y * 0.1F }, 1.5F);
                }
                break;
            }
        }
    }
}