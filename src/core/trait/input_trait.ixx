export module input.trait;
import input;
import types;

export namespace input {
    class Trait {
    protected:
        I32 m_input_id = -1;
    public:
        bool is_pressed(input::Key k)    const { return input::is_pressed(m_input_id, k); }
        bool is_locked(input::Key k)     const { return input::is_locked(m_input_id, k);  }
        void press(input::Key k)         const { input::press(m_input_id, k);             }
        void release(input::Key k)       const { input::release(m_input_id, k);           }
        void lock(input::Key k)          const { input::lock(m_input_id, k);              }
        void unlock(input::Key k)        const { input::unlock(m_input_id, k);            }

        bool is_pressed(input::Button b) const { return input::is_pressed(m_input_id, b); }
        bool is_locked(input::Button b)  const { return input::is_locked(m_input_id, b);  }
        void press(input::Button b)      const { input::press(m_input_id, b);             }
        void release(input::Button b)    const { input::release(m_input_id, b);           }
        void lock(input::Button b)       const { input::lock(m_input_id, b);              }
        void unlock(input::Button b)     const { input::unlock(m_input_id, b);            }
    };
}