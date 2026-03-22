export module input.trait;
import input;
import types;

export namespace input {
    class Trait {
    protected:
        I32 m_input_id = -1;
    public:
        bool is_pressed(input::Key k) { return input::is_pressed(m_input_id, k); }
        bool is_locked(input::Key k)  { return input::is_locked(m_input_id, k);  }
        void press(input::Key k)      { input::press(m_input_id, k);             }
        void release(input::Key k)    { input::release(m_input_id, k);           }
        void lock(input::Key k)       { input::lock(m_input_id, k);              }
        void unlock(input::Key k)     { input::unlock(m_input_id, k);            }

        bool is_pressed(input::Button b) { return input::is_pressed(m_input_id, b); }
        bool is_locked(input::Button b)  { return input::is_locked(m_input_id, b);  }
        void press(input::Button b)      { input::press(m_input_id, b);             }
        void release(input::Button b)    { input::release(m_input_id, b);           }
        void lock(input::Button b)       { input::lock(m_input_id, b);              }
        void unlock(input::Button b)     { input::unlock(m_input_id, b);            }
    };
}