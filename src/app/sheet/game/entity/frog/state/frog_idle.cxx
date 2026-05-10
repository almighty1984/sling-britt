module entity.frog;

namespace entity {
    void Frog::state_idle(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_sensed_position = {};

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

        if (m_is_on_ground and m_sensed_state == state::Type::none) {
            for (auto& i : m_sensed_objects) {
                if (!i or i->is_dead()) continue;

                //if (i->type() == Type::particle_health) {
                //    //if (health::amount(m_health_id) < health::max(m_health_id)) {
                //        console::log("health amount: ", health::amount(m_health_id), " max: ", health::max(m_health_id), "\n");
                //    //}
                //}

                if (i->type() == Type::particle_rock) {
                    console::log(class_name(), "::state_idle() sensed rock\n");
                    m_sensed_state = state::Type::melee;
                    m_sensed_position = i->position();
                    m_time_left_to_react = 1;

                    m_sensed_objects.clear();
                    break;
                } else if (i->type() == Type::particle_health and health::amount(m_health_id) < health::max(m_health_id)) {
                    console::log(class_name(), "::state_idle() sensed health\n");
                    m_sensed_state = state::Type::melee;
                    m_sensed_position = i->position();
                    m_time_left_to_react = 1;

                    m_sensed_objects.clear();
                    break;
                } else if (i->type() == Type::player and !(i->state() == state::Type::swim and i->num_jumps() == 0)) {
                    console::log(class_name(), "::state_idle() sensed player\n");
                    if (i->state() == state::Type::duck and (i->velocity().x < -1.5F or i->velocity().x > 1.5F)) {
                        //console::log("velocity x: ", i->velocity().x, "\n");
                        //console::log("player sliding ground\n");
                        m_time_left_to_react = 1;
                        m_sensed_state = state::Type::jump;

                        m_sensed_position = i->position() + Vec2F{ 0.0F, -32.0F };
                    } else {
                        if (i->state() == state::Type::sling) {
                            m_time_left_to_react += std::abs(i->rotation_speed());
                        } else {
                            //m_time_left_to_react = 5 + random::number(0, 2);
                            //m_time_left_to_react = std::abs(i->velocity().x / 2.0F);
                        }
                        //if (m_time_left_to_react <= 0) {
                            m_time_left_to_react = 10;
                        //}
                        console::log(class_name(), "::idle() time left to react: ", m_time_left_to_react, "\n");
                        m_sensed_state = state::Type::melee;
                        m_sensed_position = i->position() + Vec2F{ 8.0F, 8.0F };
                    }

                    m_sensed_objects.clear();
                    break;
                }
            }
        }

        if (m_time_left_to_react > 0) {
            console::log(class_name(), "::state_idle()  time left to react: ", (int)m_time_left_to_react, "\n");
            --m_time_left_to_react;
            if (m_time_left_to_react == 0) {
                m_next_state = m_sensed_state;
            }
        }
    }
}