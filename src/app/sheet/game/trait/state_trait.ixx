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

        virtual void state_blocked()    {}
        virtual void state_bounce()     {}
        virtual void state_carried()    {}
        virtual void state_dead()       {}
        virtual void state_dive()       {}
        virtual void state_enter()      {}
        virtual void state_exit()       {}
        virtual void state_heal()       {}
        virtual void state_hurt()       {}
        virtual void state_idle()       {}
        virtual void state_jump()       {}
        virtual void state_ledge()      {}
        virtual void state_melee()      {}
        virtual void state_walk()       {}
        virtual void state_run()        {}
        virtual void state_shoot()      {}
        virtual void state_sling()      {}
        virtual void state_stunned()    {}
        virtual void state_swim()       {}
        virtual void state_tossed()     {}
        virtual void state_upended()    {}
        virtual void state_slide_wall() {}
        virtual void state_jump_wall()  {}

        void state_update() {
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
            case Type::blocked:    state_blocked();    break;
            case Type::bounce:     state_bounce();     break;
            case Type::carried:    state_carried();    break;
            case Type::dead:       state_dead();       break;
            case Type::dive:       state_dive();       break;
            case Type::enter:      state_enter();      break;
            case Type::exit:       state_exit();       break;
            case Type::heal:       state_heal();       break;
            case Type::hurt:       state_hurt();       break;
            case Type::idle:       state_idle();       break;
            case Type::jump:       state_jump();       break;
            case Type::ledge:      state_ledge();      break;
            case Type::melee:      state_melee();      break;
            case Type::walk:       state_walk();       break;
            case Type::run:        state_run();        break;
            case Type::shoot:      state_shoot();      break;
            case Type::sling:      state_sling();      break;
            case Type::stunned:    state_stunned();    break;
            case Type::swim:       state_swim();       break;
            case Type::tossed:     state_tossed();     break;
            case Type::upended:    state_upended();    break;
            case Type::slide_wall: state_slide_wall(); break;
            case Type::jump_wall:  state_jump_wall();  break;
            default:                                   break;
            }
        }
    };
}