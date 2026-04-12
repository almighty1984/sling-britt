module;
#include <filesystem>
#include <string>
#include <unordered_map>

export module sound.trait;
import sound;
import types;

export namespace sound {
    class Trait {
    protected:
        std::unordered_map<std::string, I32> m_sounds;
    public:
        I32 sound(const std::string& s) const {
            auto it = m_sounds.find(s);
            if (it != m_sounds.end()) {
                return it->second;
            }
            return -1;
        }
        void sound(const std::string& s, cI32 id) {
            m_sounds[s] = id;
        }
        void sound_pause(const std::string& s)              const { sound::pause(sound(s));              }
        void sound_play(const std::string& s)               const { sound::play(sound(s));               }
        void sound_stop(const std::string& s)               const { sound::stop(sound(s));               }
        void sound_pitch(const std::string& s, cF32 p)      const { sound::pitch(sound(s), p);           }
        void sound_position(const std::string& s, cVec2F p) const { sound::position(sound(s), p);        }
        bool sound_is_looped(const std::string& s)          const { return sound::is_looped(sound(s));   }
        void sound_is_looped(const std::string& s, bool q)  const { sound::is_looped(sound(s), q);       }
        bool sound_is_paused(const std::string& s)          const { return sound::is_paused(sound(s));   }
        bool sound_is_playing(const std::string& s)         const { return sound::is_playing(sound(s));  }
        bool sound_is_stopped(const std::string& s)         const { return sound::is_stopped(sound(s));  }
    };
    bool parse_config(const std::string& text, Trait* owner);
}

