export module health.config;
import health;
//import entity;
import types;
import std;

export namespace health {
    struct Config {
        I32 id       = -1;
        F32 max      = 0.0F,
            amount   = 0.0F,
            regen    = 0.0F;
        Vec2F offset = {};
    };
    Config parse_config(const std::string& text, cI32 transform) {
        const size_t health_label = text.find("Health", 0);
        if (health_label == std::string::npos or text.find("{", health_label) == std::string::npos) {
            return {};
        }
        const size_t health_open = text.find("{", health_label) + 1;
        const size_t health_close = text.find("\n}", health_open);

        if (health_close == std::string::npos) {
            return {};
        }

        Config config{};

        config.id = health::make(transform);

        const size_t amount_label = text.find("amount", health_open);
        if (amount_label < health_close) {
            const size_t end_line = text.find("\n", amount_label);
            size_t value_start = text.find("=", amount_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.amount = std::stoi(text.substr(value_start, end_line - value_start));
            }
        }
        const size_t regen_label = text.find("regen", health_open);
        if (regen_label < health_close) {
            const size_t end_line = text.find("\n", regen_label);
            size_t value_start = text.find("=", regen_label);
            if (value_start < end_line) {
                ++value_start;
                while (text.at(value_start) == '	' or text.at(value_start) == ' ') ++value_start;
                config.regen = std::stof(text.substr(value_start, end_line - value_start));                
            }
        }
        const size_t offset_label = text.find("offset", health_open);
        size_t end_line = text.find("\n", offset_label);
        size_t offset_start = text.find("=", offset_label);
        if (offset_start < end_line) {
            const size_t offset_open = text.find("{", offset_start);
            const size_t offset_close = text.find("}", offset_start);

            if (offset_open < end_line and offset_close < end_line) {                
                size_t value_0 = offset_open + 1;
                while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                const size_t comma_0 = text.find(",", value_0);
                if (comma_0 < end_line) {
                    config.offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                }
                size_t value_1 = comma_0 + 1;
                while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                size_t value_1_end = offset_close;
                while (text.at(value_1_end) == '	' or text.at(value_1_end) == ' ') --value_1_end;

                config.offset.y = std::stof(text.substr(value_1, value_1_end - value_1));                
            }
        }
        return config;
    }
}