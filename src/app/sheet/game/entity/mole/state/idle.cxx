module entity.mole;

namespace entity {
    void Mole::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //sprite_angle(0.0F);
            max_velocity(start_max_velocity());
            m_time_left_alive = U16_MAX;

            m_sensed.clear();
            //m_time_left_to_spawn_sense = m_config.time_to_spawn_sense;
            m_time_left_to_spawn_sense = m_config.time_to_spawn_sense() * 1;
            m_time_left_to_react = 0;
            reset_anim("idle");
            sprite_is_hidden(false);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
            if (m_prev_state == state::Type::dead) {
                health_reset();
                sprite_is_leftward(random::number(0, 1) ? true : false);
            }

            sprite_offset_y(8.0F);

            health_is_hidden(true);
        }

        //console::log(class_name(), "::idle()\n");

        ++m_time_in_state;
        if (m_time_in_state > m_time_to_turn) {
            m_time_in_state = 0;
            m_time_to_turn = 40 + random::number(0, 20);
            sprite_is_leftward(!sprite_is_leftward());
        }

        if (sprite::is_leftward(m_sprite)) {
            velocity_add_x(-acceleration().x);
        } else {
            velocity_add_x(acceleration().x);
        }

        velocity_add_y(acceleration().y);

        if (!m_is_on_ground) {
            return;

        }
        //console::log(class_name(), "::state_idle() ", m_time_left_to_spawn_sense, "\n");
        if (health_amount() > 0.0F and m_time_left_to_spawn_sense > 0) {
            --m_time_left_to_spawn_sense;
            if (m_time_left_to_spawn_sense == 0) {
                m_time_left_to_spawn_sense = m_config.time_to_spawn_sense();

                if (!is_reacting()) {
                    particle::spawn_fan(this, 150.0F, 390.0F, 16, particle::Type::sense, position() + Vec2F{ 4.0F, 0.0F }, velocity(), 6.0F);
                }

                if (std::abs(velocity().x) > 1.0F) {

                    particle::spawn({ .parent = this,
                                        .type = particle::Type::dust,
                                        .position = position() + Vec2F{4.0F, 18.0F},
                                        .velocity = Vec2F{ 0.0F, 0.0F } });
                }

            }
        }

        
        if (m_time_left_to_react > 0) {
            console::log(class_name(), "::state_idle() sensed offset: ", m_sensed_offset.x, " ", m_sensed_offset.y, "\n");
            //console::log(class_name(), "::idle() time left to react: ", m_time_left_to_react, "\n");
            --m_time_left_to_react;
            F32 slowdown = 1.0F - (1.0F / m_time_left_to_react);
            if (slowdown < 0.0F) {
                slowdown = 0.0F;
            }
            //console::log(class_name(), "::idle() slowdown: ", slowdown, "\n");                

            velocity_x(velocity().x * slowdown);
            if (m_time_left_to_react == 0) {
                m_next_state = state::Type::exit;
            }
            return;
        }

        if (m_sensed.empty()) return;

        for (auto& i : m_sensed) {
            if (!(i->velocity().x > -0.2F and i->velocity().x < 0.2F and i->velocity().y > -1.0F and i->velocity().y < 1.0F) and
                (i->type() == Type::player or i->type() == Type::brick and i->is_tossed()) or
                (i->type() == Type::frog and is_enemy(i))
                ) {

                //m_sensed_offset = i->position() + Vec2F{ sprite::rect(i->sprite()).w, sprite::rect(i->sprite()).h };
                m_sensed_offset = i->position_on_level() + Vec2F{ aabb::rect(i->aabb(aabb::Name::body)).w / 2.0F - 4.0F,
                                                                  aabb::rect(i->aabb(aabb::Name::body)).h / 2.0F - 4.0F };

                if (m_time_left_to_react == 0) {
                    if (i->type() == Type::brick) {
                        m_time_left_to_react = 1;
                    } else {
                        m_time_left_to_react = random::number(1, 10);
                    }
                    //console::log(class_name(), "::idle() sensed: ", to_string(i->type()), " time left to react : ", m_time_left_to_react, "\n");
                }
            } else {
                if (m_time_left_to_react == 0) {
                    if (i->type() == Type::particle_health) {
                        cVec2F diff = { i->position().x > position().x ? i->position().x - position().x : position().x - i->position().x,
                                        i->position().y > position().y ? i->position().y - position().y : position().y - i->position().y,
                        };

                        //console::log(class_name(), "::idle() sensed health ", diff.x, " ", diff.y, "\n");
                        //console::log(class_name(), "::idle() health amount: ", health_amount(), "\n");
                        if (!health_is_max()) {
                            sprite_is_leftward(i->position().x < position().x);
                            if (diff.x < 4.0F and diff.y < 16.0F) {
                                m_time_left_to_react = 1;
                                m_sensed_offset = {};
                            }
                        }
                    }
                }
            }
        }

    }
}