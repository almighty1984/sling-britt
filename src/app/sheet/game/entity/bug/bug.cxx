module entity.bug;
import app.config;
import particle_system;

namespace entity {
    bool Bug::hurt(entity::Object* culprit) {
        if (!culprit or m_time_left_hurt > 0) {
            return false;
        }
        m_time_left_hurt = m_time_to_hurt;

        Vec2F add_to_position = { 0.0F, 0.0F };

        if (culprit->type() == entity::Type::player) {
            console::log("entity::Bug::hurt player vel y: ", culprit->velocity().y, "\n");
            sound_position("bounce", { position().x / (app::config::extent().x / 2.0F),
                                       position().y / (app::config::extent().y / 2.0F) });
            sound_play("bounce");
            return true;
        }

        //m_state = state::Type::upended;
        m_next_state = state::Type::hurt;

        if (culprit->type() == Type::brick or culprit->type() == Type::bug) {            
            cF32 amount = std::abs(culprit->velocity().x * culprit->velocity().y) * 2.5F;
            health_amount_add(-amount);

            m_next_state = health_amount() > 0.0F ? state::Type::hurt : state::Type::dead;

            if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  4.0F;
            else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -4.0F;
            if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  4.0F;
            else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -4.0F;
        }
        else if (culprit->type() == entity::Type::particle_melee) {
            velocity_x(culprit->velocity().x * 0.1F + velocity().x * 0.5F);
            health_amount_add(-16.0f);
            
            if      (culprit->position().x < position().x + 8.0F) add_to_position.x =  2.0F;
            else if (culprit->position().x + 8.0F > position().x) add_to_position.x = -2.0F;
            if      (culprit->position().y < position().y + 8.0F) add_to_position.y =  2.0F;
            else if (culprit->position().y + 8.0F > position().y) add_to_position.y = -2.0F;
        }
        else {
            health_amount_add(-2.0f);
        }
        sprite_is_leftward(!sprite_is_leftward());

        sound_position("melee", { position().x / (app::config::extent().x / 2.0F),
                                  position().y / (app::config::extent().y / 2.0F) });
        sound_play("melee");

        position_add(add_to_position);

        return true;
    }

    void Bug::state_bounce() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            /*sound::position(sound("bounce"), { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
            sound::play(sound("bounce"));*/

            reset_anim("bounce");
        }
        //int num_frames = anim()->texture_size.x / anim()->source.w;
        //console::log("num_frames: ", anim()->current_frame(), "\n");
        if (anim::current_frame(m_current_anim) + 1 == anim::num_frames(m_current_anim)) {
            m_next_state = state::Type::upended;
        }
    }
    void Bug::state_carried() {
        if (!m_parent) {
            m_next_state = state::Type::walk;            
            return;
        }
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_saved_state = m_prev_state;
        }
        m_is_on_ground = false;

        console::log("entity::Bug::carried ", velocity().x, " ", velocity().y, "\n");


        velocity(m_parent->velocity());
        moved_velocity({});
        if (sprite::is_leftward(m_parent->sprite())) {
            if (position().x < m_parent->position().x - 12.0F) {
                position_x(m_parent->position().x - 12.0F);
            }
            if (position().x > m_parent->position().x - 12.0F) {
                cF32 diff_x = (m_parent->position().x - 12.0F) - position().x;
                velocity_add_x(diff_x / 4.0F);
            }
            else {
                position_x(m_parent->position().x - 12.0F);
            }
        }
        else {
            if (position().x > m_parent->position().x + 12.0F) {
                position_x(m_parent->position().x + 12.0F);
            }

            if (position().x < m_parent->position().x + 12.0F) {
                cF32 diff_x = (m_parent->position().x + 12.0F) - position().x;
                velocity_add_x(diff_x / 4.0F);
            }
            else {
                position_x(m_parent->position().x + 12.0F);
            }
        }

        sprite::is_leftward(m_sprite, position().x + 16.0F < m_parent->position().x + 8.0F);

        position_y(m_parent->position().y + 2.0F);
        /*if (m_parent->is_ducking()) {
            position_add_y(4.0F);
        }*/
        if (m_parent->is_ducking() or !m_parent->is_carrying()) {
            m_parent->is_carrying(false);
            velocity({ m_parent->velocity().x, velocity().y });
            if (sprite::is_leftward(m_sprite)) {
                velocity_add_x(-0.5F);
                position_add_x(-8.0F);
            }
            else {
                velocity_add_x(0.5F);
                position_add_x(8.0F);
            }
            velocity({ velocity().x, -1.0F });

            if (m_parent->is_on_ground()) {
                m_next_state = m_prev_state;
            }
            else {
                if (m_prev_state == state::Type::walk) {
                    m_next_state = state::Type::upended;
                }
                else if (m_prev_state == state::Type::upended) {
                    m_next_state = state::Type::walk;
                }
            }

            m_parent = nullptr;
            m_is_first_state_update = true;
        }
    }
    void Bug::state_dead() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_left_dead = m_time_to_be_dead;
            reset_anim("dead");
            //console::log("prev state: ", entity::to_string(m_prev_state), "\n");
            if (m_prev_state == state::Type::upended or m_prev_state == state::Type::bounce) {
                sprite::is_upended(m_sprite, true);
            }
            for (auto& i : m_aabbs) {
                aabb::is_active(i, false);
            }
            particle::spawn_fan(this,
                0.0F, 360.0F, 8,
                particle::Type::drop_blood,
                position() - Vec2F{ 16.0F, 8.0F },
                velocity(), 3.0F);

            //particle::spawn({ this, particle::Type::health, position() , {} });
            particle::spawn(this, particle::Type::health, position(), {});

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
                console::log("entity::Bug::dead done being dead\n");
            }
        }
        velocity({});
        moved_velocity({});
    }
    void Bug::state_hurt() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("hurt");
            m_parent = nullptr;
        }
        //console::log("entity::Bug::hurt()\n");
        deceleration({});
        set_anim("hurt");
        if (anim::is_last_frame(m_current_anim)) {
            m_next_state = state::Type::upended;
        }
    }
    void Bug::state_swim() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_sensed_objects.clear();
            m_is_on_ground = false;
        }
        //console::log("num jumps: ", (int)m_num_jumps, "\n");

        if (m_num_jumps == 1) {
            if (velocity().x < 0.0F) {
                sprite::is_leftward(m_sprite, true);
            } else {
                sprite::is_leftward(m_sprite, false);
            }
        }

        set_anim("upended");
        sprite::rect_x(m_sprite, 0);

        anim::speed(m_current_anim, 0.0F);

        if (m_num_jumps == 1) {
            acceleration(start_acceleration());
            deceleration(start_deceleration());
            max_velocity({ 10.0F, 10.0F });
        }
        else {
            acceleration({ });
            deceleration({ 0.0F, 0.1F });
            max_velocity({ 4.0F, 4.0F });

            cF32 diff_y = m_water_line_y - position_on_level().y;
            velocity_add_y(diff_y / 30.0F);

            sprite::angle(m_sprite, degrees() + 270.0F);
        }
    }
    void Bug::state_tossed() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;

            deceleration({});
        }
        if (m_is_on_ground) {
            console::log("entity::Bug::tossed, on ground state: ", to_string(m_saved_state), "\n");
            //m_next_state = state::Type::walk;
            //m_next_state = state::Type::upended;
            m_next_state = m_saved_state;
        }
        else {
            //console::log("entity::Bug toss not on ground\n");
        }
    }
    void Bug::state_upended() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        set_anim("upended");

        if (m_is_on_ground) {
            deceleration({ 0.2F, 0.0F });
        } else {
            deceleration({ 0.0F, 0.0F });
        }

        if (m_time_left_in_state > 0) {
            --m_time_left_in_state;
            if (m_time_left_in_state == 0) {
                m_next_state = state::Type::walk;
            }
        }
    }
    void Bug::state_walk() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            acceleration(start_acceleration());
            deceleration(start_deceleration());

            sprite::is_hidden(m_sprite, false);
            for (auto& i : m_aabbs) {
                aabb::is_active(i, true);
            }
            if (m_prev_state == state::Type::dead) {
                health::reset(m_health_id);
                sprite::is_leftward(m_sprite, random::number(0, 1) ? true : false);
            }
        }
        deceleration({});

        if (m_is_on_ground) {
            //console::log("entity::Bug walk on ground\n");
            moved_velocity({});
            velocity({ sprite::is_leftward(m_sprite) ? -0.5F : 0.5F, velocity().y });
        }
        else {
            //console::log("entity::Bug walk not on ground\n");
        }
        m_current_anim = anim("idle");
    }    
}