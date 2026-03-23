module;
#include <cmath>
#include <sstream>

module entity.frog;
import particle_system;

namespace entity {
    bool Frog::hurt(Object* culprit) {
        if (m_time_left_hurt > 0) return false;
        m_time_left_hurt = m_time_to_hurt;

        if (!culprit) return false;

        if (culprit->type() == Type::brick || culprit->type() == Type::bug) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit);

            //velocity() = culprit->velocity() * 0.25F;
            cF32 amount = std::abs(culprit->velocity().x) * 4.0F;
            health::add_amount(m_health_id, -amount);
            //health::get(m_health_id)->amount -= 32.0F;
            m_next_state = State::hurt;
        }
        else if (culprit->type() == Type::particle_brick) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit);

            cF32 amount = std::sqrtf(std::abs(culprit->velocity().x * culprit->velocity().y));
            //console::log("amount: ", amount, "\n");

            health::add_amount(m_health_id, -amount);
            m_next_state = State::hurt;
        }
        else if (culprit->type() == Type::particle_down_thrust) {
            velocity_x(culprit->velocity().x * 0.7F);
            if (m_is_on_ground) {
                velocity_y(-2.0f);
            }
            m_next_state = State::stunned;
        }
        else if (culprit->type() == Type::particle_melee) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit->parent());

            velocity(culprit->velocity() * 0.25F);
            health::add_amount(m_health_id, -8.0f);
            m_next_state = State::hurt;
        }
        else if (culprit->type() == Type::player) {
            if (culprit->state() == State::run) {
                if (culprit->velocity().y >= 6.0F) {
                    velocity(culprit->velocity() * 0.5F);
                    m_next_state = State::stunned;
                    m_time_to_be_in_state = 100;

                    console::log("Frog::hurt() m_time_to_be_in_state: ", m_time_to_be_in_state, "\n");

                    sound_position("bounce", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                    sound_play("bounce");

                    particle::spawn(this, particle::Type::hit, position(), {});
                }
            }
            else if (culprit->state() == State::sling) {
                m_sensed_objects.clear();
                health_add_amount(-8.0f);
                m_next_state = State::stunned;
                m_time_to_be_in_state = 20;

                sound_position("bump_head", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
                sound_play("bump_head");
            }
        }
        return true;
    }
    void Frog::state_blocked() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            reset_anim("hurt");
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_in_state = 0;
            transform::velocity(m_transform_id, { sprite::is_leftward(m_sprite_id) ? 1.0f : -1.0f, -1.5f });

            m_is_on_ground = false;

            sound_position("block", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("block");
        }
        //console::log("Frog::blocked\n");

        set_anim("hurt");
        m_tounge_start = transform::position(m_transform_id) + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };

        line::max_length(m_tounge_line_id, 32.0F - m_time_in_state * 4.0f);
        if (line::max_length(m_tounge_line_id) < 1.0f) {
            line::max_length(m_tounge_line_id, 1.0f);
        }

        line::set(m_tounge_line_id, m_tounge_start, m_tounge_end);

        transform::velocity_add(m_transform_id, { 0.0F, transform::acceleration(m_transform_id).y });

        ++m_time_in_state;
        if (m_is_on_ground && m_time_in_state > 100) {
            m_time_in_state = 0;
            m_next_state = State::idle;
        }
    }
    void Frog::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");
            line::is_hidden(m_tounge_line_id, true);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::drop_blood, position() + Vec2F{ 4.0F, 0.0F },
                                velocity(), 3.0F);
            particle::spawn(this, particle::Type::health, position() + Vec2F{ 4.0F, 0.0F }, {});

            sound_position("dead", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("dead");

            //m_next_state = State::none;
            //m_sensed_state = State::none;
            m_sensed_objects.clear();
        }

        //console::log("Frog::dead() next state: ", to_string(m_next_state), "\n");

        velocity(velocity() * 0.9F);

        //velocity().x *= 0.9F;
        //velocity().y *= 0.9F;
        moved_velocity({});

        if (m_time_left_dead > 0) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log("Frog::dead() done being dead\n");
            }
        }
    }
    void Frog::state_heal() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("stunned");
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_objects.clear();

            line::max_length(m_tounge_line_id, line::length(m_tounge_line_id));
        }
        velocity_add_y(acceleration().y);

        //console::log("entity::Frog::state_heal() health: ", health::amount(m_health_id), " ", m_time_left_in_state, "\n");
        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            health::add_amount(m_health_id, 0.1F);

            cVec2F tounge_vector = m_tounge_end - m_tounge_start;
            cF32 tounge_length = line::length(tounge_vector);

            if (tounge_length > 1.0F) {
                cVec2F tounge_normalized = tounge_vector / tounge_length;
                m_tounge_end -= tounge_normalized;
                line::set(m_tounge_line_id, m_tounge_start, m_tounge_end);
            }
        }
        if (m_time_left_in_state == 0 || health::is_full(m_health_id)) {
            m_next_state = State::idle;
        }
    }
    void Frog::state_hurt() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("hurt");
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                }
                else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_state = State::idle;
        }

        set_anim("hurt");
        line::is_hidden(m_tounge_line_id, true);

        console::log("entity::Frog::hurt\n");

        velocity_add_y(acceleration().y);

        //console::log("time left to react: ", (int)m_time_left_to_react, "\n");
        if (m_time_left_to_react > 0) {
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = m_sensed_state;
            }
        }
        else
            if (/*anim()->is_last_frame() &&*/ m_is_on_ground) {
                //console::log("is last frame: ", anim()->is_last_frame(), "\n");

                for (auto& i : m_sensed_objects) {
                    if (i->is_dead()) continue;
                    if (m_is_on_ground && (i->type() == Type::brick ||
                        i->type() == Type::particle_brick ||
                        i->type() == Type::particle_melee ||
                        i->type() == Type::player ||
                        i->type() == Type::bug)
                        ) {
                        m_time_left_to_react = 10;
                        console::log("entity::Frog::hurt() sensed: ", to_string(i->type()), "\n");
                        m_sensed_state = State::melee;
                        m_sensed_position = i->position();
                        m_sensed_objects.clear();
                        break;
                    }
                }
                if (m_sensed_state == State::idle) {
                    m_time_left_to_react = 1;
                }
            }
    }
    void Frog::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            if (m_prev_state == State::dead) {
                //m_sensed_state = State::none;
                health::reset(m_health_id);
                sprite::is_leftward(m_sprite_id, random::number(0, 1) ? true : false);
            }
            max_velocity(start_max_velocity());
            m_time_left_to_spawn_sense = m_time_to_spawn_sense + 0;
            m_time_left_to_react = 0;
            reset_anim("idle");
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_objects.clear();
            m_sensed_position = {};
            m_sensed_state = State::none;
        }
                
        set_anim("idle");

        line::is_hidden(m_tounge_line_id, true);

        deceleration_x(m_is_on_ground ? 0.2F : 0.0F);

        velocity_add({ 0.0F, acceleration().y });

        if (m_time_left_to_spawn_sense > 0) {
            --m_time_left_to_spawn_sense;
            if (m_time_left_to_spawn_sense == 0) {
                //console::log("Frog::idle() spawn sense\n");

                m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                particle::spawn_fan(this, 0.0F, 360.0F, 32,
                                    particle::Type::sense,
                                    position() + Vec2F{ 4.0F, 0.0F }, {}, 3.0F);
            }
        }

        if (m_time_left_to_react > 0) {
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = m_sensed_state;
            }
        }
        else if (m_is_on_ground) {
            for (auto& i : m_sensed_objects) {
                if (i->is_dead()) continue;

                //if (i->type() == Type::particle_health) {
                //    //if (health::amount(m_health_id) < health::max(m_health_id)) {
                //        console::log("health amount: ", health::amount(m_health_id), " max: ", health::max(m_health_id), "\n");
                //    //}
                //}
                if (i->type() == Type::particle_health && health::amount(m_health_id) < health::max(m_health_id)) {
                    console::log("senseD health\n\n\n");
                    m_sensed_state = State::melee;
                    m_sensed_position = i->position();
                    m_time_left_to_react = 1;

                    m_sensed_objects.clear();
                    break;
                } else if (i->type() == Type::player && !(i->state() == State::swim && i->num_jumps() == 0)) {
                    console::log("senseD player\n\n\n");
                    if (i->is_ducking() && (i->velocity().x < -1.5F || i->velocity().x > 1.5F)) {
                        //console::log("velocity x: ", i->velocity().x, "\n");
                        //console::log("player sliding ground\n");
                        m_time_left_to_react = 1;
                        m_sensed_state = State::jump;

                        m_sensed_position = i->position() + Vec2F{ 0.0F, -32.0F };
                    } else {
                        if (i->state() == State::sling) {
                            m_time_left_to_react += std::abs(i->rotation_speed());
                        } else {
                            //m_time_left_to_react = 5 + random::number(0, 2);
                            m_time_left_to_react = std::abs(i->velocity().x / 2.0F);
                        }
                        if (m_time_left_to_react <= 0) {
                            m_time_left_to_react = 1;
                        }
                        console::log("entity::Frog::idle() time left to react: ", m_time_left_to_react, "\n");
                        m_sensed_state = State::melee;
                        m_sensed_position = i->position();
                    }

                    m_sensed_objects.clear();
                    break;
                }
            }
        }
    }
    void Frog::state_jump() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_sensed_objects.clear();

            cF32 diff_x = m_sensed_position.x - position().x;
            cF32 diff_y = m_sensed_position.y - position().y;

            //console::log("diff_y: ", diff_y, "\n");
            max_velocity(start_max_velocity());

            velocity_x(diff_x / 64.0F);
            velocity_y(diff_y / 10.0F);

            position_add_y(-2.0F);

            if (velocity().y < -4.0F) {
                velocity_y(-4.0F);
            } else if (velocity().y > -2.0F) {
                velocity_y(-2.0F);
            }
            m_is_on_ground = false;

            reset_anim("jump");
        }

        line::is_hidden(m_tounge_line_id, true);

        set_anim("jump");

        velocity_add_y(acceleration().y);

        if (m_is_on_ground && velocity().y > 0.0F) {
            m_next_state = State::idle;
            reset_anim("idle");
        }
    }
    void Frog::state_melee() {
        if (!m_is_on_ground) return;

        set_anim("melee");

        sprite::is_leftward(m_sprite_id, m_sensed_position.x < position().x);

        m_tounge_start = position() + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };
        //console::log("tounge start: ", m_tounge_start.x, " ", m_tounge_start.y, "\n");
        //console::log("tounge end: ", m_tounge_end.x, " ", m_tounge_end.y, "\n");

        line::size(m_tounge_line_id, 2);
        line::max_length(m_tounge_line_id, 48.0F - anim::source(m_current_anim_id).x / 16.0F);

        if (line::max_length(m_tounge_line_id) < 16.0F) {
            line::max_length(m_tounge_line_id, 16.0F);
        }

        //line::max_length(m_tounge_line_id, 64.0F);

        line::is_hidden(m_tounge_line_id, false);
        line::layer(m_tounge_line_id, sprite::layer(m_sprite_id) + 1);
        line::color(m_tounge_line_id, { 191, 0, 0 });
        line::set(m_tounge_line_id, m_tounge_start, m_tounge_end);
        //line::get(m_tounge_line_id)->update();

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("melee");

            sound_position("melee", { position().x / (WINDOW_W / 2.0F), position().y / (WINDOW_H / 2.0F) });
            sound_play("melee");

            cVec2F tounge_vector = (m_tounge_end - m_tounge_start);
            cF32 tounge_length = line::length(tounge_vector);

            //console::log("length: ", tounge_length, "\n");

            Vec2F melee_velocity = (tounge_vector / tounge_length) * 8.0f;

            console::log("entity::Frog::melee() sensed position: ", m_sensed_position.x, " ", m_sensed_position.y, "\n");
            //console::log("entity::Frog::melee melee velocity: ", melee_velocity.x, " ", melee_velocity.y, "\n");

            particle::spawn({ .parent = this,
                              .type = particle::Type::melee,
                              .position = { position().x + 4.0f, position().y + 4.0f },
                              .velocity = melee_velocity });
        }


        if (anim::is_last_frame(m_current_anim_id)) {
            //console::log("is last frame: ", anim()->is_last_frame(), "\n");                
            //line::is_hidden(m_tounge_line_id, true);
            m_next_state = State::jump;
        }

    }
    void Frog::state_stunned() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            reset_anim("stunned");
            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_left_in_state = m_time_to_be_in_state;
        }
        set_anim("stunned");

        line::is_hidden(m_tounge_line_id, true);

        velocity_add_y(acceleration().y);

        console::log("Frog::stunned time left: ", m_time_left_in_state, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
        }
        if (m_is_on_ground && m_time_left_in_state == 0) {
            m_time_in_state = 0;
            m_next_state = State::idle;
        }
    }
    void Frog::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
            //reset_anim(m_anim_ids["swim"]);

            for (auto& i : m_aabb_ids) {
                if (aabb::name(i) == aabb::Name::body_swim) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
        }
        line::is_hidden(m_tounge_line_id, true);
        set_anim("hurt");

        deceleration({ 0.0F, 0.0F });

        if (position_on_level().y + 16 < m_water_line_y) {
            console::log("frog swim water line y: ", m_water_line_y, "\n");
            //transform()->max_velocity = transform()->start_max_velocity;
            max_velocity({ 0.0F, 4.0F });
        } else {
            max_velocity({ 0.0F, 2.0F });
            velocity_x(sprite::is_leftward(m_sprite_id) ? -0.8F : 0.8F);
        }

        velocity_y(velocity().y * 0.9F);
        //moved_velocity({});

        if (position_on_level().y > m_water_line_y) {
            velocity_add({ 0.0F, -0.3F });
        } else {
            velocity_add({ 0.0F, 0.3F });
        }
    }
}