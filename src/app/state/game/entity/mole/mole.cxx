module entity.mole;
import app.config;
import particle_system;

namespace entity {
    bool Mole::hurt(entity::Object* culprit) {
        if (!culprit or m_state == state::Type::swim) return false;

        if (m_time_left_hurt > 0) {
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        Vec2F add_to_position = { 0.0F, 0.0F };

        if (culprit->type() == Type::brick) {
            health_amount_add(-health_max());
            m_next_state = state::Type::enter;
        }
        else if (culprit->type() == Type::particle_brick) {
            health_amount_add(-1.0f);
            m_next_state = state::Type::enter;
        }
        else if (culprit->type() == Type::particle_down_thrust) {
            if (culprit->parent()) {
                F32 vel_x = (position().x - culprit->parent()->position().x) / 16.0F;
                if      (vel_x < -1.5F) vel_x = -1.5F;
                else if (vel_x > 1.5F) vel_x  =  1.5F;
                console::log(class_name(), "::hurt() vel_x: ", vel_x, "\n");

                velocity_x(vel_x);
            }
            health_amount_add(-1.0f);
            m_next_state = state::Type::jump;
        }
        else if (culprit->type() == Type::particle_melee) {
            if (m_state == state::Type::jump) {
                if (culprit->parent()) {
                    if (sprite::is_leftward(culprit->parent()->sprite_id())) {
                        velocity_x(-5.0F);
                    } else {
                        velocity_x(5.0F);
                    }
                }
                //velocity_y(-1.0F);
            }
            health_amount_add(-16.0f);

            if (culprit->position().x < position().x + 8.0F) {
                add_to_position.x = 4.0F;
            }
            else if (culprit->position().x + 8.0F > position().x) {
                add_to_position.x = -4.0F;
            }            
        }
        else {
            health_amount_add(-8.0f);
            m_next_state = state::Type::enter;
        }
        sprite_is_leftward(!sprite_is_leftward());

        sound_position("melee", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
        sound_play("melee");


        position_add(add_to_position);

        return true;
    }
    void Mole::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");

            sprite_is_hidden(true);

            health_is_hidden(true);

            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this, 0.0F, 360.0F, 8, particle::Type::drop_blood, position() - Vec2F{ 16.0F, 8.0F }, velocity(), 3.0F);

            particle::spawn(this, particle::Type::health, position() + Vec2F{ 0.0F, -4.0F }, {});

            sound_position("dead", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("dead");

            if (m_parent) {
                m_parent->is_carrying(false);
                m_parent = nullptr;
            }
        }
        if (m_time_left_dead > 0 and m_time_to_be_dead != U16_MAX) {
            --m_time_left_dead;
            if (m_time_left_dead == 0) {
                console::log(class_name(), "::state_dead() done being dead\n");
            }
        }
        velocity({});
        moved_velocity({});
    }
    void Mole::state_enter() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("enter");
            m_time_in_state = 0;

            sprite_offset_y(-8.0F);

            health_is_hidden(false);
        }
        //console::log(class_name(), "::enter()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim_id("enter"))) {
            m_next_state = state::Type::idle;
        }
    }
    void Mole::state_exit() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("exit");
            sprite_offset_y(-8.0F);

            health_is_hidden(false);
        }
        //console::log(class_name(), "::exit()\n");
        velocity_x(0.0F);
        if (anim::is_last_frame(anim_id("exit"))) {
            if (m_sensed_position.x == 0.0F and m_sensed_position.y == 0.0F) {
                m_next_state = state::Type::enter;
            }
            else {
                m_next_state = state::Type::shoot;
            }
        }
    }
    void Mole::state_idle() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            //sprite_angle(0.0F);
            max_velocity(start_max_velocity());
            m_time_left_alive = U16_MAX;
            
            m_sensed_objects.clear();
            //m_time_left_to_spawn_sense = m_time_to_spawn_sense;
            m_time_left_to_spawn_sense = m_time_to_spawn_sense * 1;
            m_time_left_to_react = 0;
            reset_anim("idle");
            sprite_is_hidden(false);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
            if (m_prev_state == state::Type::dead) {
                health_reset();
                sprite_is_leftward(random::number(0, 1) ? true : false);
            }

            sprite_offset_y(8.0F);

            health_is_hidden(true);
        }

        ++m_time_in_state;
        if (m_time_in_state > m_time_to_turn) {
            m_time_in_state = 0;
            m_time_to_turn = 40 + random::number(0, 20);
            sprite_is_leftward(!sprite_is_leftward());
        }

        if (sprite::is_leftward(m_sprite_id)) {
            velocity_add_x(-acceleration().x);
        }
        else {
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
                m_time_left_to_spawn_sense = m_time_to_spawn_sense;

                particle::spawn_fan(this, 170.0F, 370.0F, 16, particle::Type::sense, position() + Vec2F{ 4.0F, 0.0F }, velocity(), 6.0F);

                if (std::abs(velocity().x) > 1.0F) {

                    particle::spawn({ .parent = this,
                                        .type = particle::Type::dust,
                                        .position = position() + Vec2F{4.0F, 18.0F},
                                        .velocity = Vec2F{ 0.0F, 0.0F } });
                }

            }
        }

        for (auto& i : m_sensed_objects) {            
            if (!(i->velocity().x > -0.2F and i->velocity().x < 0.2F and i->velocity().y > -1.0F and i->velocity().y < 1.0F) and
                (i->type() == Type::player or i->type() == Type::brick and i->is_tossed()) //or
                //i->type() == Type::particle_brick
                ) {
                
                //m_sensed_position = i->position() + Vec2F{ sprite::source_rect(i->sprite_id()).w, sprite::source_rect(i->sprite_id()).h };
                m_sensed_position = i->position();
                if (m_time_left_to_react == 0) {
                    if (i->type() == Type::brick) {
                        m_time_left_to_react = 1;
                    }
                    else {
                        m_time_left_to_react = random::number(10, 40);
                    }
                    //console::log(class_name(), "::idle() sensed: ", to_string(i->type()), " time left to react : ", m_time_left_to_react, "\n");
                }
            }
            else {
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
                                m_sensed_position = {};
                            }
                        }
                    }                    
                }
            }
        }

        if (m_time_left_to_react > 0) {
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
        }
    }
    void Mole::state_jump() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("jump");
            sprite_is_hidden(false);
            for (auto& i : m_aabb_ids) {
                aabb::is_active(i, true);
            }
            console::log(class_name(), "::state_jump() ", start_acceleration().x, " ", start_acceleration().y, "\n");
            //velocity_x(0.0F);
            if (!m_is_first_update) {
                velocity_y(-4.0F);
            }

            m_is_on_ground = false;
            acceleration(start_acceleration());
            deceleration(start_deceleration());
            max_velocity({ 10.0F, 10.0F });

            sprite_offset({ -8.0F, -8.0F });

            health_is_hidden(false);
        }

        velocity_add_y(acceleration().y);

        F32 radians = std::atan2(velocity().y, velocity().x);
        if (radians < 0.0F) radians += 3.1415926535f * 2.0F;

        cF32 degrees = radians * 180.0F / 3.1415926535F;

        sprite_angle(degrees + 90.0F);
        if (m_is_on_ground) {
            m_next_state = state::Type::idle;
        }
    }
    void Mole::state_shoot() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_in_state = 30;
            m_next_state = state::Type::enter;
            m_sensed_objects.clear();

            sprite_is_leftward(m_sensed_position.x < position().x + 4.0F);

            reset_anim("shoot");

            sound_position("shoot", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound_play("shoot");

            cVec2F start_position = position() + Vec2F{ 4.0F, -2.0F };
            cVec2F end_position = m_sensed_position + Vec2F{ 8.0F, 8.0F };
            cVec2F v = (end_position - start_position);

            Vec2F shot_velocity = v / line::length(v) * 8.0F;
            if (shot_velocity.y > -3.0F) {
                shot_velocity.y = -3.0F;
            }
            if (shot_velocity.x < 0.0F and shot_velocity.x > -2.0F) {
                shot_velocity.x = -2.0F;
            } else if (shot_velocity.x > 0.0F and shot_velocity.x < 2.0F) {
                shot_velocity.x = 2.0F;
            }
            particle::spawn({ .parent = this,
                              .type = particle::Type::shot,
                              .position = start_position,
                              .velocity = shot_velocity });


            sprite_offset_y(-4.0F);

            health_is_hidden(false);
        }
        velocity_x(0.0F);

        //console::log(class_name(), "::shoot() time left in state: ", m_time_left_in_state, "\n");

    }
    void Mole::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
            reset_anim("jump");

            if (velocity().x < 0.0F) {
                added_degrees = 180.0F;
            }
            else {
                added_degrees = 0.0F;
            }

            sprite_offset({ -8.0F, -8.0F });

            health_is_hidden(true);
        }

        acceleration({ });
        deceleration({ 0.0F, 0.1F });
        max_velocity({ 4.0F, 4.0F });

        cF32 diff_y = m_water_line_y - position_on_level().y;
        
        velocity_add_y(diff_y / 30.0F);

        //console::log(class_name(), "::state_swim() degrees: ", degrees(), "\n");

        //sprite_angle(degrees() + added_degrees);

        sprite_angle_add(1.0F);

        sprite_is_leftward(false);
        sprite_is_upended(false);
    }
}