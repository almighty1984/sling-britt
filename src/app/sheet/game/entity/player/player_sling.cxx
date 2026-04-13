module entity.player;
import app.config;
import console;
import sprite;
import transform;

void entity::Player::state_sling() {
    if (!m_parent) {
        m_next_state = state::Type::run;
        return;
    }
    
    Vec2F sling_position = m_parent->position() + m_parent->start_offset();  // level transform + start offset
    Vec2F offset_from_sling = position() - sling_position;

    cVec2F prev_pos = position();
    cVec2F level_pos = transform::position(transform::level_id());
    
    cVec2F pos_on_level = position() - transform::position(transform::level_id());
    if (m_is_first_state_update) {
        m_is_first_state_update = false;
        //m_next_state = entity::Type::sling;

        for (auto& i : m_aabbs) {
            aabb::is_active(i, false);
            if (!aabb::is_active(i) and aabb::name(i) == aabb::Name::body) {
                aabb::is_active(i, true);
            }
        }

        m_rotation_speed = -velocity().y * velocity().x * 0.75F;
        //m_rotation_speed = -(velocity().x * velocity().y);
        //m_rotation_speed = offset_from_sling.x / 4.0F + offset_from_sling.y / 4.0F/* + velocity().y / 2.0F*/;

        cF32 lower_limit = 3.0F,
             upper_limit = 8.0F;

        if (!sprite::is_leftward(m_sprite)) {
            if (m_rotation_speed > -lower_limit) {
                m_rotation_speed = -lower_limit;
            } else if (m_rotation_speed < -upper_limit) {
                m_rotation_speed = -upper_limit;
            }
        } else {
            if (m_rotation_speed < lower_limit) {
                m_rotation_speed = lower_limit;
            } else if (m_rotation_speed > upper_limit) {
                m_rotation_speed = upper_limit;
            }
        }
        console::log("rotation speed: ", m_rotation_speed, "\n");

        //m_rotation_speed = velocity().y;

        //m_rotation_speed = 0.0F;

        console::log("m_rotation_speed: ", m_rotation_speed, "\n");

        cF32 radians = std::atan2f(offset_from_sling.y, offset_from_sling.x);
        cF32 angle = radians * 180.0F / 3.1415926535F;


        console::log(class_name(), "::state_sling angle: ", angle, "\n");

        sprite::origin(m_sprite, { 32.0F, 20.0F });
        sprite::angle(m_sprite, angle - 90.0F);

        //console::log(class_name(), "::state_sling() offset from sling: ", offset_from_sling.x, " ", " ", offset_from_sling.y, "\n");

        velocity( { 0.0F, 0.0F } );
        acceleration({ 0.1F, 0.1F });
        deceleration({ 0.0F, 0.0F });

        reset_anim("sling");

        max_velocity(m_sling_max_velocity);
    }
    m_is_on_ground = false;
    anim::speed(m_current_anim, 0.0F);
    

    cF32 rotate_dec = offset_from_sling.x / 80.0F;
    cF32 rotate_acc = 0.25f;
    m_rotation_speed += rotate_dec;
    
    sprite::angle_add(m_sprite, m_rotation_speed);

    if (is_pressed(key_left)) {
        m_rotation_speed += rotate_acc;
    }
    if (is_pressed(key_right)) {
        m_rotation_speed -= rotate_acc;        
    }    

    position({ std::cosf(sprite::radians(m_sprite) + 3.1415956535F / 2.0F) * 28.0F + level_pos.x + m_parent->start_offset().x,
               std::sinf(sprite::radians(m_sprite) + 3.1415956535F / 2.0F) * 28.0F + level_pos.y + m_parent->start_offset().y });

    offset_from_sling = position() - sling_position;
    sprite::offset(m_sprite, {-24.0F - offset_from_sling.x, -10.0F - offset_from_sling.y});

    if (!is_pressed(key_melee)) {
        //release(key_melee);        
        m_next_state = state::Type::run;

        
        m_time_left_jump_again = m_time_to_jump_again;
        m_num_jumps = 0;
        reset_anim("jump_spin");
        
        sound_pitch("jump", 1.122454545454545F);

        velocity((position() - prev_pos) * 1.5F);        

        console::log("player_sling() velocity: ", velocity().x, " ", velocity().y, "\n");

        sprite_offset(sprite_start_offset());
        sprite_angle(0.0F);
        
        sprite_is_leftward(velocity().x < 0.0F);

        sound_position("sling_detach", { position().x / (app::config::extent().x / 2.0F), position().y / (app::config::extent().y / 2.0F) });
        sound_play("sling_detach");
    }

    m_rotation_speed *= 0.99f;

    if (m_rotation_speed < -13.0F) {
        m_rotation_speed = -13.0F;
    }
    if (m_rotation_speed > 13.0F) {
        m_rotation_speed = 13.0F;
    }

    //console::log(class_name(), "::sling() rotation_speed: ", rotation_speed, "\n");
}