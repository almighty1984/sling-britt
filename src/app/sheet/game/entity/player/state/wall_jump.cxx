module entity.player;

namespace entity {
    void Player::state_wall_jump(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_time_in_state = 0;
            m_time_left_skidding = 0;
            //m_num_jumps = 0;
            max_velocity(m_ground_max_velocity * 1.2F);
        }
        ++m_time_in_state;

        //console::log(class_name(), "::state_wall_jump()\n");

        //reset_anim("jump");

        velocity({ 0.0F, 0.0F });
        //--m_time_left_jumping_wall;
        if (is_pressed(key_left)) {            
            direction_x(1.0F);
        }
        if (is_pressed(key_right)) {            
            direction_x(-1.0F);
        }
        sprite_is_leftward(!m_is_near_wall_L);
        if (m_time_in_state >= m_time_to_wall_jump) {
            console::log(class_name(), "::state_wall_jump()\n");
            if (direction().x > 0.0F) {                
                velocity_x(m_is_near_wall_L ? 1.0F : -1.5F);
            } else {                
                velocity_x(m_is_near_wall_L ? 1.5F : -1.0F);
            }
            direction({});
            jump();
            unlock(key_jump);            
            m_next_state = m_saved_state;
            console::log(class_name(), "::state_wall_jump() next state: ", state::to_string(m_next_state), "\n");
        }
        
    }
}