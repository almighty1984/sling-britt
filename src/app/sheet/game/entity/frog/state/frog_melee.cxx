module entity.frog;

namespace entity {
    void Frog::state_melee(cF32 dt) {
        if (!m_is_on_ground) return;

        if (m_sensed_position == Vec2F{}) {
            console::log(class_name(), "::state_melee() empty sensed position, back to idle\n");
            m_next_state = state::Type::idle;
            return;
        }

        set_anim("melee");

        sprite_is_leftward(m_sensed_position.x < position().x);


        m_tounge_start = position() + Vec2F{ 8.0F, 6.0F };
        m_tounge_end = m_sensed_position + Vec2F{ 0.0F, 0.0F };
        //console::log("tounge start: ", m_tounge_start.x, " ", m_tounge_start.y, "\n");
        //console::log("tounge end: ", m_tounge_end.x, " ", m_tounge_end.y, "\n");

        line::size(m_tounge_line, 2);
        line::max_length(m_tounge_line, 48.0F - anim::source(m_current_anim).x / 16.0F);

        if (line::max_length(m_tounge_line) < 16.0F) {
            line::max_length(m_tounge_line, 16.0F);
        }

        console::log(class_name(), "::state_melee() tounge max length: ", line::max_length(m_tounge_line), "\n");

        //console::log(class_name(), "::state_melee() tounge max length: ", line::max_length(m_tounge_line), "\n");

        line::is_hidden(m_tounge_line, false);
        line::layer(m_tounge_line, sprite_layer() + 1);
        line::color(m_tounge_line, { 191, 0, 0 });
        line::set(m_tounge_line, m_tounge_start, m_tounge_end);

        //m_tounge_end = line::end(m_tounge_line);

        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            reset_anim("melee");

            sound_position("melee", { position().x - app::config::extent().x / 2.0F,
                                      position().y - app::config::extent().y / 2.0F });
            sound_play("melee");
            
            //cVec2F tounge_vector = (m_tounge_end - m_tounge_start;
            cVec2F tounge_vector = line::end(m_tounge_line) - line::start(m_tounge_line);
            cF32 tounge_length = line::length(tounge_vector);

            //console::log(class_name(), "::state_melee() tounge vector: ", tounge_vector.x, " ", tounge_vector.y, "\n");
            console::log(class_name(), "::state_melee() tounge length: ", tounge_length, "\n");

            Vec2F melee_velocity = (tounge_vector / tounge_length) * 8.0F;

            //console::log(class_name(), "::melee() sensed position: ", m_sensed_position.x, " ", m_sensed_position.y, "\n");
            //console::log(class_name(), "::melee melee velocity: ", melee_velocity.x, " ", melee_velocity.y, "\n");

            particle::spawn({ .parent = this,
                              .type = particle::Type::melee,
                              .position = { position().x + 4.0F, position().y + 4.0F },
                              .velocity = melee_velocity });
        }


        if (anim::is_last_frame(m_current_anim)) {
            //console::log("is last frame: ", anim()->is_last_frame(), "\n");                
            //line::is_hidden(m_tounge_line, true);
            m_next_state = state::Type::jump;
        }

    }
}