export module entity.player;
import app.config;
import anim;
import aabb;
import console;
import health;
import input.trait;
import random;
import sound;
import sprite;
import entity;
import types;

export namespace entity {
    class Player : public Object,
                   public input::Trait {
    private:
        start::Type   m_next_level = start::Type::center;

        start::Info m_next_start = { start::Type::center, 0 };

        bool m_is_to_save = false;

        U8 m_time_left_until_down_thrust = 0, m_time_until_down_thrust = 16,
           m_time_left_bouncing          = 0, m_time_to_bounce         = 20,           
           m_time_left_hitting_ground    = 0, m_time_to_hit_ground     = 20,
           m_time_left_holding_jump      = 0, m_time_to_hold_jump      = 10,
           m_time_to_jump_wall      =  8,
           m_time_left_jump_again        = 0, m_time_to_jump_again     =  8,
           m_time_left_lever             = 0, m_time_to_lever          = 10,
           m_time_left_melee             = 0, m_time_to_melee          =  8,
           m_time_left_skidding          = 0, m_time_to_skid           = 20,
           m_time_sliding_ground         = 0;

        bool m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_down_thrusting = false,
             m_is_sliding_ground = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;

        Vec2F m_ground_max_velocity = { 2.0F, 4.0F };
        Vec2F m_sling_max_velocity  = { 4.0F, 5.0F };

        Vec2F m_slide_ground_max_velocity = { 1.4F, 0.0F };

        U8 m_time_left_in_state = 0;
        
        input::Key key_up     = input::Key::up,
                   key_down   = input::Key::down,
                   key_left   = input::Key::left,
                   key_right  = input::Key::right,
                   key_jump   = input::Key::x,
                   key_melee  = input::Key::c,
                   key_sprint = input::Key::z;

        I32 m_sling_shot_sprite    = -1,
            m_sling_shot_bg_sprite = -1;
                
    public:
        Player();
        ~Player();

        //const char* class_name() override { return "entity::\033[0;95mPlayer\033[0m"; }
        const char* class_name() override { return "entity::Player"; }

        U8 id = 0;
                
        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

                
        start::cType next_level() const { return m_next_level; }
        start::cInfo next_start() const { return m_next_start; }

        bool is_to_save() const { return m_is_to_save; }    void is_to_save(bool q) { m_is_to_save = q; }

        bool hurt(Object* culprit) override;
        void interact(Object* object) override;

        void spawn_down_thrust(cVec2F position);

        void state_dead()       override;
        void state_dive()       override;
        void state_ledge()      override;
        void state_run()        override;
        void state_swim()       override;
        void state_shoot()      override;
        void state_sling()      override;
        void state_slide_wall() override;
        void state_jump_wall()  override;

        void update() override;

        void draw(std::unique_ptr<Window>& window) override {
            sprite::draw(window, m_sling_shot_bg_sprite);
            Object::draw(window);
            sprite::draw(window, m_sling_shot_sprite);

        }


        void jump() {
            lock(key_jump);
            m_time_left_jump_again = m_time_to_jump_again;
            m_is_on_ground = false;
            m_is_on_slope = false;
            m_is_hovering = false;
            m_is_sliding_ground = false;
            m_time_left_holding_jump = m_time_to_hold_jump;
            
            m_time_left_ducking = 0;
            m_time_left_rising = 0;
            
            position_add({ 0.0F, -2.0F });

            F32 pitch = random::real_number(-0.02F, 0.02F);

            ++m_num_jumps;
            if (m_is_carrying) {
                m_num_jumps = 1;
            }
            if (m_num_jumps == 3) {
                m_time_left_skidding = 0;
                velocity_y(-3.0F);
                reset_anim("jump_spin");
                pitch += 1.122454545454545F;
            } else {
                if (m_num_jumps == 2) {
                    velocity_y(-2.2F);
                    reset_anim("jump");
                    pitch += 1.059454545454545F;
                } else {
                    velocity_y(-2.0F);
                    reset_anim("jump");
                    pitch += 1.0F;
                }
                if (m_time_left_skidding > 0) {
                    m_time_left_skidding = 0;
                    reset_anim("jump_skid");
                }
            }
            sound_pitch("jump", pitch);
            sound_position("jump", { (position().x + 8.0F) / (app::config::extent().x / 2.0F),
                                     (position().y + 8.0F) / (app::config::extent().y / 2.0F) });
            sound_play("jump");
        }

    };
}