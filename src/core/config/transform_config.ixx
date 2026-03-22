module;
#include <string>

export module transform.config;
import transform;
import transform.trait;
import types;

export namespace transform {
    bool parse_config(const std::string& text, Trait* owner) {
        const size_t transform_label = text.find("Transform", 0);
        if (!owner || transform_label == std::string::npos || owner->transform_id() != -1) {
            //goto end_of_transform_block;
            return false;
        }

        if (text.find("{", transform_label) != std::string::npos) {
            const size_t transform_open = text.find("{", transform_label) + 1;

            cI32 id = transform::make();
            owner->transform_id(id);

            //console::log("entity ", entity::to_string(m_type), " camera add transform: ", m_transform_id, "\n");
            //camera::add_transform_id(m_transform_id);

            const size_t transform_close = text.find("\n}", transform_open);
            if (transform_close != std::string::npos) {
                const size_t velocity_limit_label = text.find("max_velocity", transform_open);
                size_t end_line = text.find("\n", velocity_limit_label);
                size_t velocity_limit_start = text.find("=", velocity_limit_label);
                if (velocity_limit_start < end_line) {
                    const size_t velocity_limit_open = text.find("{", velocity_limit_start);
                    const size_t velocity_limit_close = text.find("}", velocity_limit_start);

                    if (velocity_limit_open < end_line && velocity_limit_close < end_line) {
                        Vec2F max_velocity = {};
                        size_t value_0 = velocity_limit_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            max_velocity.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = velocity_limit_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        max_velocity.y = std::stof(text.substr(value_1, value_1_end - value_1));

                        transform::max_velocity(id, max_velocity);
                        transform::start_max_velocity(id, max_velocity);
                    }
                }

                const size_t acceleration_label = text.find("acceleration", transform_open);
                end_line = text.find("\n", acceleration_label);
                size_t acceleration_start = text.find("=", acceleration_label);
                if (acceleration_start < end_line) {
                    const size_t acceleration_open = text.find("{", acceleration_start);
                    const size_t acceleration_close = text.find("}", acceleration_start);

                    if (acceleration_open < end_line && acceleration_close < end_line) {
                        Vec2F acceleration = {};
                        size_t value_0 = acceleration_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            acceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = acceleration_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        acceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));

                        transform::acceleration(id, acceleration);
                        transform::start_acceleration(id, acceleration);
                    }
                }

                const size_t deceleration_label = text.find("deceleration", transform_open);
                end_line = text.find("\n", deceleration_label);
                size_t deceleration_start = text.find("=", deceleration_label);
                if (deceleration_start < end_line) {
                    const size_t deceleration_open = text.find("{", deceleration_start);
                    const size_t deceleration_close = text.find("}", deceleration_start);

                    if (deceleration_open < end_line && deceleration_close < end_line) {
                        Vec2F deceleration = {};

                        size_t value_0 = deceleration_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            deceleration.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = deceleration_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        deceleration.y = std::stof(text.substr(value_1, value_1_end - value_1));

                        transform::deceleration(id, deceleration);
                        transform::start_deceleration(id, deceleration);
                    }
                }

            }
        }
        return true;
    }
}