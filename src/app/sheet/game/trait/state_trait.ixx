export module state.trait;
import console;
import types;

export namespace state {
    class Trait {
    protected:
        Type  m_state       = Type::none,
              m_next_state  = Type::none,
              m_prev_state  = Type::none,
              m_start_state = Type::none,
              m_saved_state = Type::none;

        bool  m_is_first_state_update = true;

        U16 m_time_left_in_state      = 0,
            m_time_left_in_next_state = 0,
            m_time_in_state           = 0,
            m_time_to_be_in_state =     0;

    public:
        void next_state(cType t) { m_next_state = t; }
        void state(cType t)      { m_state = t; }

        Type state()       const { return m_state; }
        Type start_state() const { return m_start_state; }
        Type prev_state()  const { return m_prev_state;  }

        U16 time_left_in_next_state() const { return m_time_left_in_next_state; } void time_left_in_next_state(cU16 t) { m_time_left_in_next_state = t; }

        virtual void state_attack(cF32 dt)     {}
        virtual void state_blocked(cF32 dt)    {}
        virtual void state_bounce(cF32 dt)     {}
        virtual void state_carried(cF32 dt)    {}
        virtual void state_charge(cF32 dt)     {}
        virtual void state_climb(cF32 dt)      {}
        virtual void state_dead(cF32 dt)       {}
        virtual void state_dive(cF32 dt)       {}
        virtual void state_duck(cF32 dt)       {}
        virtual void state_enter(cF32 dt)      {}
        virtual void state_exit(cF32 dt)       {}
        virtual void state_heal(cF32 dt)       {}
        virtual void state_hurt(cF32 dt)       {}
        virtual void state_idle(cF32 dt)       {}
        virtual void state_jump(cF32 dt)       {}
        virtual void state_ledge(cF32 dt)      {}
        virtual void state_melee(cF32 dt)      {}
        virtual void state_walk(cF32 dt)       {}
        virtual void state_run(cF32 dt)        {}
        virtual void state_shoot(cF32 dt)      {}
        virtual void state_sling(cF32 dt)      {}
        virtual void state_stunned(cF32 dt)    {}
        virtual void state_swim(cF32 dt)       {}
        virtual void state_tossed(cF32 dt)     {}
        virtual void state_upended(cF32 dt)    {}
        virtual void state_slide_wall(cF32 dt) {}
        virtual void state_jump_wall(cF32 dt)  {}

        void state_update(cF32 dt) {
            if (m_next_state != m_state) {
               /* m_prev_state = m_state;
                m_state = m_next_state;
                m_is_first_state_update = true;*/

                if (m_time_left_in_state > 0) {
                    --m_time_left_in_state;
                    console::log("state::Trait::state_update() m_time_left_in_state: ", m_time_left_in_state, "\n");
                }
                if (m_time_left_in_state == 0) {
                    m_prev_state = m_state;
                    m_state = m_next_state;
                    m_is_first_state_update = true;
                    //m_time_left_in_state = m_time_left_in_next_state; // FROG did it
                }
            }

            switch (m_state) {
                case Type::attack:     state_attack(dt);     break;
                case Type::blocked:    state_blocked(dt);    break;
                case Type::bounce:     state_bounce(dt);     break;
                case Type::carried:    state_carried(dt);    break;
                case Type::charge:     state_charge(dt);     break;
                case Type::climb:      state_climb(dt);      break;
                case Type::dead:       state_dead(dt);       break;
                case Type::dive:       state_dive(dt);       break;
                case Type::duck:       state_duck(dt);       break;
                case Type::enter:      state_enter(dt);      break;
                case Type::exit:       state_exit(dt);       break;
                case Type::heal:       state_heal(dt);       break;
                case Type::hurt:       state_hurt(dt);       break;
                case Type::idle:       state_idle(dt);       break;
                case Type::jump:       state_jump(dt);       break;
                case Type::ledge:      state_ledge(dt);      break;
                case Type::melee:      state_melee(dt);      break;
                case Type::walk:       state_walk(dt);       break;
                case Type::run:        state_run(dt);        break;
                case Type::shoot:      state_shoot(dt);      break;
                case Type::sling:      state_sling(dt);      break;
                case Type::stunned:    state_stunned(dt);    break;
                case Type::swim:       state_swim(dt);       break;
                case Type::tossed:     state_tossed(dt);     break;
                case Type::upended:    state_upended(dt);    break;
                case Type::slide_wall: state_slide_wall(dt); break;
                case Type::jump_wall:  state_jump_wall(dt);  break;
                default:                                     break;
            }
        }
    };
}