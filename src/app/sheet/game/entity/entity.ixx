export module entity;
import app.config;
import anim;
import camera;
import console;
import types;
import window;
import aabb.trait;
import anim.trait;
import health.trait;
import sound.trait;
import sprite.trait;
import transform.trait;
import state.trait;
import std;

export namespace entity {
    /*void clear_parse_configs() {
        s_parsed_configs.clear();
    }*/
    class Config {    
        U16 m_time_to_be_alive = U16_MAX,
            m_time_to_be_dead = 0,
            m_time_to_hurt = 10,
            m_time_to_interact = 10,
            m_time_to_rise = 10,
            m_time_to_spawn_sense = 10;                
        F32 m_rotation_speed_limit = 1.0F,
            m_rotation_acc = 0.0F,
            m_rotation_dec = 0.0F,
            m_weight = 1.0F;
        bool m_is_rotation_clockwise = false;

    public:
        U16 time_to_be_alive()    const { return m_time_to_be_alive;    } void time_to_be_alive(cU16 t)    { m_time_to_be_alive    = t; }
        U16 time_to_be_dead()     const { return m_time_to_be_dead;     } void time_to_be_dead(cU16 t)     { m_time_to_be_dead     = t; }
        U16 time_to_hurt()        const { return m_time_to_hurt;        } void time_to_hurt(cU16 t)        { m_time_to_hurt        = t; }
        U16 time_to_interact()    const { return m_time_to_interact;    } void time_to_interact(cU16 t)    { m_time_to_interact    = t; }
        U16 time_to_rise()        const { return m_time_to_rise;        } void time_to_rise(cU16 t)        { m_time_to_rise        = t; }
        U16 time_to_spawn_sense() const { return m_time_to_spawn_sense; } void time_to_spawn_sense(cU16 t) { m_time_to_spawn_sense = t; }

        F32 rotation_speed_limit() const { return m_rotation_speed_limit; } void rotation_speed_limit(cF32 l) { m_rotation_speed_limit = l; }
        F32 rotation_acc()         const { return m_rotation_acc;         } void rotation_acc(cF32 a)         { m_rotation_acc         = a; }
        F32 rotation_dec()         const { return m_rotation_dec;         } void rotation_dec(cF32 d)         { m_rotation_dec         = d; }

        bool is_rotation_clockwise() const { return m_is_rotation_clockwise; }  void is_rotation_clockwise(bool q) { m_is_rotation_clockwise = q; }

        F32 weight() const { return m_weight; } void weight(cF32 w) { m_weight = w; }
    };
    Config parse_config(const std::string& text);
}

export namespace aabb   {
    std::vector<I32> parse_config(const std::string& text, entity::Object* owner);
}
export namespace health {
    struct Config {
        I32 id       = -1;
        F32 max      = 0.0F,
            amount   = 0.0F,
            regen    = 0.0F;
        Vec2F offset = {};
    };
    Config parse_config(const std::string& text, cI32 transform);
}

export namespace entity {
    class Object : public transform::Trait,
                   public    sprite::Trait,                   
                   public      anim::Trait,
                   public    health::Trait,
                   public     sound::Trait,
                   public      aabb::Trait,
                   public     state::Trait {
    protected:
        Object* m_parent = nullptr,
              * m_child  = nullptr;

        Type m_type = Type::none;
        U8 m_number = 0;
        
        U16 m_input_limit = U16_MAX;
        std::list<Object*> m_input_objects;

        std::list<Object*> m_sensed_objects;

        Vec2F m_sensed_offset = {};
        
        U8 m_num_jumps   = 0,
           m_start_layer = 0;

        std::unordered_map<Type, U8> m_time_left_colliding_with;
        U8 m_time_left_colliding = 0;

        Config m_config;

        U16 m_time_left_alive          =  U16_MAX,
            m_time_left_blocked        =  0,
            m_time_left_dead           =  0,            
            m_time_left_hurt           =  0,
            m_time_left_interacting    =  0,
            m_time_left_rising         =  0,
            m_time_left_to_spawn_sense =  0,
            m_time_left_to_react       =  0;

        Vec2F m_start_offset        = { 0.0F, 0.0F },
              m_start_sprite_offset = { 0.0F, 0.0F },
              m_force               = { 0.0F, 0.0F };
        
        F32 m_water_line_y   = 0.0F;

        F32 m_rotation_speed = 0.0F;

        F32 m_radians = 0.0F;

        bool m_is_first_update = true;

        bool m_is_carrying    = false,
             m_is_near_wall_L = false,
             m_is_near_wall_R = false,
             m_is_on_ground   = false,
             m_is_on_slope    = false,
             m_is_powered     = false;

    public:
        Object* parent() const { return m_parent; } void parent(Object* p) { m_parent = p; }        

        virtual const char* class_name() { return "entity::Object"; }

        virtual ~Object();
        
        virtual bool load_config(const std::filesystem::path& path);

        
        virtual bool hurt(Object* culprit) { return false; }
        virtual void interact(Object* object) {}
        

        virtual void update(cF32 dt = 1.0F) {
            if (m_current_anim == -1) {
                return;
            }

            if (m_time_left_dead > 0) {
                //console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                if (m_time_left_dead != U16_MAX) {
                    --m_time_left_dead;
                }
                if (m_current_anim != anim("dead")) {
                    m_current_anim = anim("dead");
                    anim::source_x(anim("dead"), anim::first_frame(anim("dead")));                    
                }
            }
            else {
                if (m_time_left_alive != U16_MAX) {
                    --m_time_left_alive;
                }
                if (m_current_anim != anim("idle")) {
                    m_current_anim = anim("idle");
                    
                    anim::source_x(anim("idle"), anim::first_frame(anim("idle")));                    
                }
            }

            sprite_rect(anim::source(m_current_anim));
        }
        virtual void draw(std::unique_ptr<Window>& window);
        virtual void draw_aabb(std::unique_ptr<Window>& window);

        Config config() const { return m_config; }

        bool add_input(Object* object) {
            if (!object) return false;
            if (m_input_objects.size() >= m_input_limit or
                std::find(m_input_objects.begin(), m_input_objects.end(), object) != m_input_objects.end()) {
                return false;
            }
            if (object->is_powered()) {
                m_is_powered = true;
            }
            //console::log(to_string(m_type), " add_input: ", to_string(object->type()), "\n");
            m_input_objects.emplace_back(object);
            return true;
        }
        //Object* input(size_t i) { return i < m_input_objects.size() ? m_input_objects.at(i) : nullptr; }
        const std::list<Object*>& inputs() { return m_input_objects; }
        size_t num_inputs() { return m_input_objects.size(); }

        bool add_inputs_from(Object* object) {
            if (!object) {
                console::log(class_name(), "::add_inputs_from() object is null\n");
                return false;
            }
            for (auto& input : object->inputs()) {
                add_input(input);
            }
            return true;
        }
        bool remove_input(Object* object) {
            m_input_objects.remove(object);
            return true;
        }

        bool add_sensed(Object* object) {            
            if (std::find(m_sensed_objects.begin(), m_sensed_objects.end(), object) != m_sensed_objects.end()) {
                return false;
            }
            m_sensed_objects.emplace_back(object);
            return true;
        }
        Vec2F sensed_position() const { return m_sensed_offset; } void sensed_position(cVec2F p) { m_sensed_offset = p; }

        bool is_blocked()        const { return m_time_left_blocked > 0;        }
        bool is_carrying()       const { return m_is_carrying;                  } void is_carrying(bool q)    { m_is_carrying    = q; }
        bool is_dead()           const { return m_time_left_dead > 0;           }        
        bool is_hurting()        const { return m_time_left_hurt > 0;           }
        bool is_near_wall_L()    const { return m_is_near_wall_L;               } void is_near_wall_L(bool q) { m_is_near_wall_L = q; } 
        bool is_near_wall_R()    const { return m_is_near_wall_R;               } void is_near_wall_R(bool q) { m_is_near_wall_R = q; }
        bool is_on_ground()      const { return m_is_on_ground;                 } void is_on_ground(bool q)   { m_is_on_ground   = q; }
        bool is_on_slope()       const { return m_is_on_slope;                  } void is_on_slope(bool q)    { m_is_on_slope    = q; }
        bool is_interacting()    const { return m_time_left_interacting > 0;    }
        bool is_powered()        const { return m_is_powered;                   } void is_powered(bool q)     { m_is_powered     = q; }
        bool is_tossed()         const { return m_state == state::Type::tossed; }
        bool is_reacting()       const { return m_time_left_to_react > 0;       }
        
        Type  type()   const { return m_type;   } void type(cType t)   { m_type = t;   }
        U8    number() const { return m_number; } void number(cU8 n)   { m_number = n; }
        
        Vec2F position_on_level() const { return camera::position + position(); }
        Vec2F start_position_on_level() const { return camera::position + start_position(); }

        F32 rotation_speed() const { return m_rotation_speed; }
        F32 radians() const { return m_radians; }  void radians(cF32 r) { m_radians = r; }
        F32 degrees() const { return m_radians * 180.0F / 3.1415926535F; }

        Vec2F force() const { return m_force; }    void force(cVec2F f) { m_force = f; }
        void  force_x(cF32 x) { m_force.x = x; }
        void  force_y(cF32 y) { m_force.y = y; }

        Vec2F start_offset() const { return m_start_offset; } void start_offset(cVec2F offset) { m_start_offset = offset; }
        U8    start_layer()  const { return m_start_layer;  } void start_layer(cU8 layer)      { m_start_layer = layer;   }

        U8   time_left_colliding_with(cType type)           { return m_time_left_colliding_with[type]; }
        void time_left_colliding_with(cType type, cU8 time) { m_time_left_colliding_with[type] = time; }

        U16 time_left_dead()   const { return m_time_left_dead;   } void time_left_dead(cU16 t)   { m_time_left_dead   = t; }
        U16 time_left_alive()  const { return m_time_left_alive;  } void time_left_alive(cU16 t)  { m_time_left_alive  = t; }
        //U16 time_to_be_dead()  const { return m_config.time_to_be_dead();  } void time_to_be_dead(cU16 t)  { m_config.time_to_be_dead(t); }
        //U16 time_to_be_alive() const { return m_config.time_to_be_alive(); } void time_to_be_alive(cU16 t) { m_config.time_to_be_alive(t); }

        U16 time_left_hurt()   const { return m_time_left_hurt;   } void time_left_hurt(cU16 t)   { m_time_left_hurt   = t; }        

        //U16 time_to_spawn_sense()   const { return m_config.time_to_spawn_sense();   } void time_to_spawn_sense(cU16 t)   { m_config.time_to_spawn_sense(t); }

        U16 time_left_interacting() const { return m_time_left_interacting; } void time_left_interacting(cU16 t) { m_time_left_interacting = t; }
        U16 time_left_in_state()    const { return m_time_left_in_state;    } void time_left_in_state(cU16 t)    { m_time_left_in_state    = t; }
        U16 time_in_state()         const { return m_time_in_state;         } void time_in_state(cU16 t)         { m_time_in_state         = t; }


        U8 num_jumps() const { return m_num_jumps; }   void num_jumps(cU8 n) { m_num_jumps = n; }

        bool is_in_view() { return position().x + sprite_rect().w > 0.0F and position().x < app::config::extent().x and
                                   position().y + sprite_rect().h > 0.0F and position().y < app::config::extent().y; }

        bool is_start_in_view() { return start_position().x + sprite_rect().w > 0.0F and start_position().x < app::config::extent().x and
                                         start_position().y + sprite_rect().h > 0.0F and start_position().y < app::config::extent().y; }

        bool is_start_offset_in_view() { return camera::position.x < m_start_offset.x and camera::position.x + app::config::extent().x > m_start_offset.x; }

        //Vec2F sensed_position() const { return m_sensed_offset; } void sensed_position(cVec2F p) { m_sensed_offset = p; }

        I32 aabb(aabb::cName name) override;
    };
}