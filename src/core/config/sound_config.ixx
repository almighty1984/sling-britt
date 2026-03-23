module;
#include <filesystem>
#include <string>

export module sound.config;
//import console;
import sound;
import sound.trait;
import types;

export namespace sound {
    bool parse_config(const std::string& text, Trait* owner) {
        const size_t sounds_label = text.find("Sounds", 0);
        if (sounds_label == std::string::npos || text.find("{", sounds_label) == std::string::npos) {
            return false;
        }
        const size_t sounds_open = text.find("{", sounds_label) + 1;
        const size_t sounds_close = text.find("\n}", sounds_open);
        if (sounds_close != std::string::npos) {
            size_t label_start = sounds_open + 1;
            while (1) {
                const size_t end_line = text.find("\n", label_start + 1);
                if (end_line > sounds_close) break;

                while ((text.at(label_start) == '	' || text.at(label_start) == ' ' || text.at(label_start) == '\n') && label_start < end_line) {
                    ++label_start;
                }
                size_t current_equals = text.find("=", label_start);
                if (current_equals > sounds_close) break;

                size_t label_end = text.find("=", label_start);
                while ((text.at(label_end - 1) == '	' || text.at(label_end - 1) == ' ') && label_end > label_start) {
                    --label_end;
                }
                const std::string sound_label = text.substr(label_start, label_end - label_start);

                size_t value_start = current_equals + 1;
                while ((text.at(value_start) == '	' || text.at(value_start) == ' ') && value_start < end_line) {
                    ++value_start;
                }
                size_t value_end = end_line;
                while ((text.at(value_end - 1) == '	' || text.at(value_end - 1) == ' ') && value_end > value_start) {
                    --value_end;
                }
                const std::filesystem::path sound_path = "res/sound/" + text.substr(value_start, value_end - value_start);

                //console::log("entity::load_config() ", sound_path, "\n\n");

                //m_sound_ids[sound_label] = sound::make(sound_path);

                cI32 id = sound::make(sound_path);
                owner->sound_id(sound_label, id);

                label_start = end_line;
            }
        }
        return true;
    }
}