module;
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <unordered_map>
export module entity.player;
import anim;
import aabb;
import console;
import health;
import input.trait;
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
           m_time_left_ducking           = 0, m_time_to_duck           = 10,
           m_time_left_rising            = 0, m_time_to_rise           = 10,
           m_time_left_hitting_ground    = 0, m_time_to_hit_ground     = 20,
           m_time_left_holding_jump      = 0, m_time_to_hold_jump      = 10,
           m_time_left_jumping_wall      = 0, m_time_to_jump_wall      =  8,
           m_time_left_jump_again        = 0, m_time_to_jump_again     =  8,
           m_time_left_lever             = 0, m_time_to_lever          = 10,
           m_time_left_melee             = 0, m_time_to_melee          =  8,
           m_time_left_skidding          = 0, m_time_to_skid           = 20,
           m_time_sliding_wall           = 0;

        bool m_is_wall_to_left      = false,
             m_is_to_wall_jump_left = false;

        bool m_is_down_thrusting = false,                
             m_is_sliding_ground = false,
             m_is_sliding_wall   = false,
             m_is_climbing_ledge = false,
             m_is_hovering       = false;

        Vec2F m_ground_velocity_limit = { 2.0F, 4.0F };
        Vec2F m_sling_velocity_limit  = { 4.0F, 5.0F };

        Vec2F m_slide_ground_velocity_limit = { 1.4F, 0.0F };

        U8 m_time_left_in_state = 0;
        
        input::Key key_up     = input::Key::up,
                   key_down   = input::Key::down,
                   key_left   = input::Key::left,
                   key_right  = input::Key::right,
                   key_jump   = input::Key::x,
                   key_melee  = input::Key::c,
                   key_sprint = input::Key::z;
                
    public:
        Player();
        ~Player();

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

        void state_dead()  override;
        void state_dive()  override;
        void state_ledge() override;
        void state_run()   override;
        void state_swim()  override;
        void state_sling() override;

        void update() override;
    };
}