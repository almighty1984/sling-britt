export module entity.config;
import entity;
import types;
import std;

export namespace entity {
    bool parse_config(const std::string& text, Object* owner) {
        const size_t config_label = text.find("Config", 0);
        if (config_label == std::string::npos or text.find("{", config_label) == std::string::npos) {
            return false;
        }
        const size_t config_open = text.find("{", config_label) + 1;
        const size_t config_close = text.find("\n}", config_open);
        if (config_close != std::string::npos) {
            const size_t time_to_be_alive_label = text.find("time_to_be_alive", config_open);
            if (time_to_be_alive_label < config_close) {
                const size_t end_line = text.find("\n", time_to_be_alive_label);
                size_t value_start = text.find("=", time_to_be_alive_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                    owner->time_to_be_alive(std::stoi(text.substr(value_start, end_line - value_start)));
                    owner->time_left_alive(owner->time_to_be_alive());
                }
            }
            const size_t time_to_be_dead_label = text.find("time_to_be_dead", config_open);
            if (time_to_be_dead_label < config_close) {
                const size_t end_line = text.find("\n", time_to_be_dead_label);
                size_t value_start = text.find("=", time_to_be_dead_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                    owner->time_to_be_dead(std::stoi(text.substr(value_start, end_line - value_start)));
                    //m_time_left_dead = m_time_to_be_dead;
                }
            }
            const size_t time_to_hurt_label = text.find("time_to_hurt", config_open);
            if (time_to_hurt_label < config_close) {
                const size_t end_line = text.find("\n", time_to_hurt_label);
                size_t value_start = text.find("=", time_to_hurt_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                    owner->time_to_hurt(std::stoi(text.substr(value_start, end_line - value_start)));
                }
            }
            const size_t time_to_spawn_sense_label = text.find("time_to_spawn_sense", config_open);
            if (time_to_spawn_sense_label < config_close) {
                const size_t end_line = text.find("\n", time_to_spawn_sense_label);
                size_t value_start = text.find("=", time_to_spawn_sense_label);
                if (value_start < end_line) {
                    ++value_start;
                    while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                    owner->time_to_spawn_sense(std::stoi(text.substr(value_start, end_line - value_start)));
                }
            }
        }
        return true;
    }
}