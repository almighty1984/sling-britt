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
        std::unordered_map<std::string, I32> m_sound_ids;
    public:
        I32 sound_id(const std::string& s) const {
            auto it = m_sound_ids.find(s);
            if (it != m_sound_ids.end()) {
                return it->second;
            }
            return -1;
        }
        void sound_id(const std::string& s, cI32 id) {
            m_sound_ids[s] = id;
        }
        void sound_pause(const std::string& s)              const { sound::pause(sound_id(s));              }
        void sound_play(const std::string& s)               const { sound::play(sound_id(s));               }
        void sound_stop(const std::string& s)               const { sound::stop(sound_id(s));               }
        void sound_pitch(const std::string& s, cF32 p)      const { sound::pitch(sound_id(s), p);           }
        void sound_position(const std::string& s, cVec2F p) const { sound::position(sound_id(s), p);        }
        bool sound_is_looped(const std::string& s)          const { return sound::is_looped(sound_id(s));   }
        void sound_is_looped(const std::string& s, bool q)  const { sound::is_looped(sound_id(s), q);       }
        bool sound_is_paused(const std::string& s)          const { return sound::is_paused(sound_id(s));   }
        bool sound_is_playing(const std::string& s)         const { return sound::is_playing(sound_id(s));  }
        bool sound_is_stopped(const std::string& s)         const { return sound::is_stopped(sound_id(s));  }
    };
    bool parse_config(const std::string& text, Trait* owner);
}

