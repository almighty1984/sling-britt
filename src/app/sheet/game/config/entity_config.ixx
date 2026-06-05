export module entity.config;
import entity;
import types;
import std;

export namespace entity {
    Config parse_config(const std::string& text) {
        const size_t config_label = text.find("Config", 0);
        if (config_label == std::string::npos or text.find("{", config_label) == std::string::npos) {
            return {};
        }
        Config config{};
        const size_t config_open = text.find("{", config_label) + 1;
        const size_t config_close = text.find("\n}", config_open);
        if (config_close == std::string::npos) return {};

        const size_t time_to_be_alive_label = text.find("time_to_be_alive", config_open);
        if (time_to_be_alive_label < config_close) {
            const size_t end_line = text.find("\n", time_to_be_alive_label);
            size_t value_start = text.find("=", time_to_be_alive_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.time_to_be_alive(std::stoi(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t time_to_be_dead_label = text.find("time_to_be_dead", config_open);
        if (time_to_be_dead_label < config_close) {
            const size_t end_line = text.find("\n", time_to_be_dead_label);
            size_t value_start = text.find("=", time_to_be_dead_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.time_to_be_dead(std::stoi(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t time_to_hurt_label = text.find("time_to_hurt", config_open);
        if (time_to_hurt_label < config_close) {
            const size_t end_line = text.find("\n", time_to_hurt_label);
            size_t value_start = text.find("=", time_to_hurt_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.time_to_hurt(std::stoi(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t time_to_spawn_sense_label = text.find("time_to_spawn_sense", config_open);
        if (time_to_spawn_sense_label < config_close) {
            const size_t end_line = text.find("\n", time_to_spawn_sense_label);
            size_t value_start = text.find("=", time_to_spawn_sense_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.time_to_spawn_sense(std::stoi(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t rotation_speed_limit_label = text.find("rotation_speed_limit", config_open);
        if (rotation_speed_limit_label < config_close) {
            const size_t end_line = text.find("\n", rotation_speed_limit_label);
            size_t value_start = text.find("=", rotation_speed_limit_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.rotation_speed_limit(std::stof(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t rotation_acc_label = text.find("rotation_acc", config_open);
        if (rotation_acc_label < config_close) {
            const size_t end_line = text.find("\n", rotation_acc_label);
            size_t value_start = text.find("=", rotation_acc_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.rotation_acc(std::stof(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t rotation_dec_label = text.find("rotation_dec", config_open);
        if (rotation_dec_label < config_close) {
            const size_t end_line = text.find("\n", rotation_dec_label);
            size_t value_start = text.find("=", rotation_dec_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.rotation_dec(std::stof(text.substr(value_start, end_line - value_start)));
            }
        }
        const size_t is_rotation_clockwise_label = text.find("is_rotation_clockwise", config_open);
        if (is_rotation_clockwise_label < config_close) {
            const size_t end_line = text.find("\n", is_rotation_clockwise_label);
            size_t value_start = text.find("=", is_rotation_clockwise_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;                
                auto val = text.substr(value_start, end_line - value_start);
                config.is_rotation_clockwise(val == "true" or val == "1");
            }
        }
        const size_t weight_label = text.find("weight", config_open);
        if (weight_label < config_close) {
            const size_t end_line = text.find("\n", weight_label);
            size_t value_start = text.find("=", weight_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.weight(std::stof(text.substr(value_start, end_line - value_start)));
            }
        }
        return config;
    }
}