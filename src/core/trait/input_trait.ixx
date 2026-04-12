export module input.trait;
import input;
import types;

export namespace input {
    class Trait {
    protected:
        I32 m_input = -1;
    public:
        bool is_pressed(input::Key k)    const { return input::is_pressed(m_input, k); }
        bool is_locked(input::Key k)     const { return input::is_locked(m_input, k);  }
        void press(input::Key k)         const { input::press(m_input, k);             }
        void release(input::Key k)       const { input::release(m_input, k);           }
        void lock(input::Key k)          const { input::lock(m_input, k);              }
        void unlock(input::Key k)        const { input::unlock(m_input, k);            }

        bool is_pressed(input::Button b) const { return input::is_pressed(m_input, b); }
        bool is_locked(input::Button b)  const { return input::is_locked(m_input, b);  }
        void press(input::Button b)      const { input::press(m_input, b);             }
        void release(input::Button b)    const { input::release(m_input, b);           }
        void lock(input::Button b)       const { input::lock(m_input, b);              }
        void unlock(input::Button b)     const { input::unlock(m_input, b);            }
    };
}