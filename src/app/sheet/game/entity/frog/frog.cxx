module entity.frog;
import app.config;
import particle_system;

namespace entity {
    bool Frog::hurt(Object* culprit) {
        if (m_time_left_hurt > 0) return false;
        m_time_left_hurt = m_time_to_hurt;

        if (!culprit) return false;

        Vec2F add_to_position = { 0.0F, 0.0F };

        if (culprit->type() == Type::brick or culprit->type() == Type::bug) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit);

            //velocity() = culprit->velocity() * 0.25F;
            cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y) * 3.0F;

            console::warning(class_name(), "::hurt() amount: ", amount, "\n");
            health_amount_add(-amount);
            //health::get(m_health_id)->amount -= 32.0F;

            m_next_state = health_amount() > 0.0F ? state::Type::hurt : state::Type::dead;

            if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
            else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
            if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
            else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
        }
        else if (culprit->type() == Type::particle_brick) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit);

            cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y);
            //console::log("amount: ", amount, "\n");

            health_amount_add(-amount);
            m_next_state = state::Type::hurt;
        }
        else if (culprit->type() == Type::particle_down_thrust) {
            velocity_x(culprit->velocity().x * 0.7F);
            if (m_is_on_ground) {
                velocity_y(-2.0f);
            }
            m_next_state = state::Type::stunned;
        }        
        else if (culprit->type() == Type::particle_melee) {
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit->parent());

            velocity(culprit->velocity() * 0.25F);
            health_amount_add(-8.0F);
            m_next_state = state::Type::hurt;

            if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  2.0F;
            else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -2.0F;
            if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  2.0F;
            else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -2.0F;
            
        }
        else if (culprit->type() == Type::particle_shot) {
            if (culprit->parent()) {
                console::log(class_name(), "::hurt() particle_shot parent: ", to_string(culprit->parent()->type()), "\n");
            }
            else {
                console::log(class_name(), "::hurt() parent is nullptr\n");
            }
            m_sensed_objects.clear();
            m_sensed_objects.emplace_back(culprit->parent());            
            m_next_state = state::Type::hurt;
            health_amount_add(-16.0F);
        }
        else if (culprit->type() == Type::player) {
            if (culprit->state() == state::Type::run) {
                if (culprit->velocity().y >= 6.0F) {
                    velocity(culprit->velocity() * 0.5F);
                    m_next_state = state::Type::stunned;
                    m_time_to_be_in_state = 100;

                    console::log("Frog::hurt() m_time_to_be_in_state: ", m_time_to_be_in_state, "\n");

                    sound_position("bounce", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                    sound_play("bounce");

                    particle::spawn(this, particle::Type::hit, position(), {});
                }
            }
            else if (culprit->state() == state::Type::sling) {
                m_sensed_objects.clear();
                health_amount_add(-8.0f);
                m_next_state = state::Type::stunned;
                m_time_to_be_in_state = 20;

                sound_position("bump_head", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
                sound_play("bump_head");
            }
        }

        position_add(add_to_position);


        return true;
    }
    void Frog::state_blocked() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            reset_anim("hurt");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_time_in_state = 0;
            transform::velocity(m_transform, { sprite::is_leftward(m_sprite) ? 1.0f : -1.0f, -1.5f });

            m_is_on_ground = false;

            sound_position("block", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("block");
        }
        //console::log("Frog::blocked\n");

        set_anim("hurt");
        m_tounge_start = transform::position(m_transform) + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };

        line::max_length(m_tounge_line, 32.0F - m_time_in_state * 4.0f);
        if (line::max_length(m_tounge_line) < 1.0f) {
            line::max_length(m_tounge_line, 1.0f);
        }

        line::set(m_tounge_line, m_tounge_start, m_tounge_end);

        transform::velocity_add(m_transform, { 0.0F, transform::acceleration(m_transform).y });

        ++m_time_in_state;
        if (m_is_on_ground and m_time_in_state > 100) {
            m_time_in_state = 0;
            m_next_state = state::Type::idle;
        }
    }
    void Frog::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");
            line::is_hidden(m_tounge_line, true);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this, 0.0F, 360.0F, 8,
                                particle::Type::drop_blood, position() + Vec2F{ 4.0F, 0.0F },
                                velocity(), 3.0F);
            particle::spawn(this, particle::Type::health, position() + Vec2F{ 4.0F, 0.0F }, {});

            sound_position("dead", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("dead");

            //m_next_state = state::Type::none;
            //m_sensed_state = state::Type::none;
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
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_objects.clear();
            m_time_left_in_state = 100;

            line::max_length(m_tounge_line, line::length(m_tounge_line));
        }
        velocity_add_y(acceleration().y);

        //console::log(class_name(), "::state_heal() health: ", health::amount(m_health_id), " ", m_time_left_in_state, "\n");
        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            health::amount_add(m_health_id, 0.1F);

            cVec2F tounge_vector = m_tounge_end - m_tounge_start;
            cF32 tounge_length = line::length(tounge_vector);

            if (tounge_length > 1.0F) {
                cVec2F tounge_normalized = tounge_vector / tounge_length;
                m_tounge_end -= tounge_normalized;
                line::set(m_tounge_line, m_tounge_start, m_tounge_end);
            }
        }
        if (m_time_left_in_state == 0 or health::is_max(m_health_id)) {
            m_next_state = state::Type::idle;
        }
    }
    void Frog::state_hurt() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("hurt");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                }
                else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_state = state::Type::idle;
        }

        set_anim("hurt");
        line::is_hidden(m_tounge_line, true);

        console::log(class_name(), "::hurt\n");

        velocity_add_y(acceleration().y);

        //console::log("time left to react: ", (int)m_time_left_to_react, "\n");
        if (m_time_left_to_react > 0) {
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = m_sensed_state;
            }
        }
        else
            if (/*anim()->is_last_frame() and*/ m_is_on_ground) {
                //console::log("is last frame: ", anim()->is_last_frame(), "\n");

                for (auto& i : m_sensed_objects) {
                    if (!i or i->is_dead()) continue;
                    if (m_is_on_ground and (i->type() == Type::brick or
                        i->type() == Type::particle_brick or
                        i->type() == Type::particle_melee or
                        i->type() == Type::player or
                        i->type() == Type::bug)
                        ) {
                        m_time_left_to_react = 10;
                        console::log(class_name(), "::hurt() sensed: ", to_string(i->type()), "\n");
                        m_sensed_state = state::Type::melee;
                        m_sensed_position = i->position();
                        m_sensed_objects.clear();
                        break;
                    }
                }
                if (m_sensed_state == state::Type::idle) {
                    m_time_left_to_react = 1;
                }
            }
    }
    void Frog::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            if (m_prev_state == state::Type::dead) {
                //m_sensed_state = state::Type::none;
                health::reset(m_health_id);
                sprite::is_leftward(m_sprite, random::number(0, 1) ? true : false);
            }
            max_velocity(start_max_velocity());
            m_time_left_to_spawn_sense = m_time_to_spawn_sense + 0;
            m_time_left_to_react = 0;
            reset_anim("idle");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                } else {
                    aabb::is_active(i, false);
                }
            }
            m_sensed_objects.clear();
            m_sensed_position = {};
            m_sensed_state = state::Type::none;
        }
                
        set_anim("idle");

        line::is_hidden(m_tounge_line, true);

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
                if (!i or i->is_dead()) continue;

                //if (i->type() == Type::particle_health) {
                //    //if (health::amount(m_health_id) < health::max(m_health_id)) {
                //        console::log("health amount: ", health::amount(m_health_id), " max: ", health::max(m_health_id), "\n");
                //    //}
                //}

                if (i->type() == Type::particle_shot) {
                    console::log(class_name(), "::state_idle() sensed shot\n");
                    m_sensed_state = state::Type::melee;
                    m_sensed_position = i->position();
                    m_time_left_to_react = 1;

                    m_sensed_objects.clear();
                    break;
                }
                else if (i->type() == Type::particle_health and health::amount(m_health_id) < health::max(m_health_id)) {
                    console::log(class_name(), "::state_idle() sensed health\n");
                    m_sensed_state = state::Type::melee;
                    m_sensed_position = i->position();
                    m_time_left_to_react = 1;

                    m_sensed_objects.clear();
                    break;
                }
                else if (i->type() == Type::player and !(i->state() == state::Type::swim and i->num_jumps() == 0)) {
                    console::log(class_name(), "::state_idle() sensed player\n");
                    if (i->is_ducking() and (i->velocity().x < -1.5F or i->velocity().x > 1.5F)) {
                        //console::log("velocity x: ", i->velocity().x, "\n");
                        //console::log("player sliding ground\n");
                        m_time_left_to_react = 1;
                        m_sensed_state = state::Type::jump;

                        m_sensed_position = i->position() + Vec2F{ 0.0F, -32.0F };
                    }
                    else {
                        if (i->state() == state::Type::sling) {
                            m_time_left_to_react += std::abs(i->rotation_speed());
                        } else {
                            //m_time_left_to_react = 5 + random::number(0, 2);
                            m_time_left_to_react = std::abs(i->velocity().x / 2.0F);
                        }
                        if (m_time_left_to_react <= 0) {
                            m_time_left_to_react = 1;
                        }
                        console::log(class_name(), "::idle() time left to react: ", m_time_left_to_react, "\n");
                        m_sensed_state = state::Type::melee;
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

        line::is_hidden(m_tounge_line, true);

        set_anim("jump");

        velocity_add_y(acceleration().y);

        if (m_is_on_ground and velocity().y > 0.0F) {
            m_next_state = state::Type::idle;
            reset_anim("idle");
        }
    }
    void Frog::state_melee() {
        if (!m_is_on_ground) return;

        if (m_sensed_position == Vec2F{}) {
            console::log(class_name(), "::state_melee() empty sensed position, back to idle\n");
            m_next_state = state::Type::idle;
            return;
        }

        set_anim("melee");

        sprite::is_leftward(m_sprite, m_sensed_position.x < position().x);

        m_tounge_start = position() + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 8.0F, 8.0F };
        //console::log("tounge start: ", m_tounge_start.x, " ", m_tounge_start.y, "\n");
        //console::log("tounge end: ", m_tounge_end.x, " ", m_tounge_end.y, "\n");

        line::size(m_tounge_line, 2);
        line::max_length(m_tounge_line, 48.0F - anim::source(m_current_anim).x / 16.0F);

        if (line::max_length(m_tounge_line) < 16.0F) {
            line::max_length(m_tounge_line, 16.0F);
        }

        //line::max_length(m_tounge_line, 64.0F);

        line::is_hidden(m_tounge_line, false);
        line::layer(m_tounge_line, sprite::layer(m_sprite) + 1);
        line::color(m_tounge_line, { 191, 0, 0 });
        line::set(m_tounge_line, m_tounge_start, m_tounge_end);
        //line::get(m_tounge_line)->update();

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("melee");

            sound_position("melee", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("melee");

            cVec2F tounge_vector = (m_tounge_end - m_tounge_start);
            cF32 tounge_length = line::length(tounge_vector);

            //console::log("length: ", tounge_length, "\n");

            Vec2F melee_velocity = (tounge_vector / tounge_length) * 8.0f;

            console::log(class_name(), "::melee() sensed position: ", m_sensed_position.x, " ", m_sensed_position.y, "\n");
            //console::log(class_name(), "::melee melee velocity: ", melee_velocity.x, " ", melee_velocity.y, "\n");

            particle::spawn({ .parent = this,
                              .type = particle::Type::melee,
                              .position = { position().x + 4.0f, position().y + 4.0f },
                              .velocity = melee_velocity });
        }


        if (anim::is_last_frame(m_current_anim)) {
            //console::log("is last frame: ", anim()->is_last_frame(), "\n");                
            //line::is_hidden(m_tounge_line, true);
            m_next_state = state::Type::jump;
        }

    }
    void Frog::state_stunned() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            reset_anim("stunned");
            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body) {
                    aabb::is_active(i, true);
                }
                else {
                    aabb::is_active(i, false);
                }
            }
            m_time_left_in_state = m_time_to_be_in_state;
        }
        set_anim("stunned");

        line::is_hidden(m_tounge_line, true);

        velocity_add_y(acceleration().y);

        console::log("Frog::stunned time left: ", m_time_left_in_state, "\n");

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
        }
        if (m_is_on_ground and m_time_left_in_state == 0) {
            m_time_in_state = 0;
            m_next_state = state::Type::idle;
        }
    }
    void Frog::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
            //reset_anim(m_anims["swim"]);

            for (auto& i : m_aabbs) {
                if (aabb::name(i) == aabb::Name::body_swim) {
                    aabb::is_active(i, true);
                }
                else {
                    aabb::is_active(i, false);
                }
            }
        }
        line::is_hidden(m_tounge_line, true);
        set_anim("hurt");

        deceleration({ 0.0F, 0.0F });

        if (position_on_level().y + 16 < m_water_line_y) {
            console::log("frog swim water line y: ", m_water_line_y, "\n");
            //transform()->max_velocity = transform()->start_max_velocity;
            max_velocity({ 0.0F, 4.0F });
        }
        else {
            max_velocity({ 0.0F, 2.0F });
            velocity_x(sprite::is_leftward(m_sprite) ? -0.8F : 0.8F);
        }

        velocity_y(velocity().y * 0.9F);
        //moved_velocity({});

        if (position_on_level().y > m_water_line_y) {
            velocity_add({ 0.0F, -0.3F });
        }
        else {
            velocity_add({ 0.0F, 0.3F });
        }
    }
}