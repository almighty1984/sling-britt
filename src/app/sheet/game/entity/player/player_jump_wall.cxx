module entity.player;

namespace entity {
    void Player::state_jump_wall() {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;

            m_time_in_state = 0;
            m_num_jumps = 0;
            max_velocity(m_ground_max_velocity * 1.2F);
        }
        ++m_time_in_state;

        console::log(class_name(), "::state_jump_wall()\n");
                
        //reset_anim("jump");

        velocity({ 0.0F, 0.0F });
        //--m_time_left_jumping_wall;
        if (is_pressed(key_left)) {
            m_is_to_wall_jump_left = true;
        }
        if (is_pressed(key_right)) {
            m_is_to_wall_jump_left = false;
        }
        sprite_is_leftward(!m_is_wall_to_left);
        if (m_time_in_state >= m_time_to_jump_wall) {

            console::log(class_name(), "::state_jump_wall()\n");
            if (m_is_to_wall_jump_left) {
                //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                velocity_x(m_is_wall_to_left ? 1.0F : -1.5F);
            } else {
                //console::log("m_is_wall_to_left: ", m_is_wall_to_left, "\n");
                velocity_x(m_is_wall_to_left ? 1.5F : -1.0F);
            }
            //console::log("velocity.y: ", velocity().y, "\n");
            //m_is_on_ground = true;
            jump();
            unlock(key_jump);
            //m_next_state = state::Type::run;
            m_next_state = m_saved_state;

            console::log(class_name(), "::state_jump_wall() next state: ", state::to_string(m_next_state), "\n");
        }
        
    }
}