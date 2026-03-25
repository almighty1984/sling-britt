module;
#include <filesystem>
#include <string>

export module sprite.config;
import sprite;
import sprite.trait;
import types;

export bool sprite::parse_config(const std::string& text, Trait* owner, cI32 transform_id, cU8 layer, cVec2F start_offset) {
    if (text.find("Sprite", 0) == std::string::npos) {
        return false;
    }
    if (text.find("{", text.find("Sprite", 0)) != std::string::npos) {
        const size_t sprite_label = text.find("Sprite", 0);
        const size_t sprite_open = text.find("{", sprite_label) + 1;
        const size_t sprite_close = text.find("\n}", sprite_open);
        if (sprite_close != std::string::npos) {

            bool sprite_is_debug = false,
                sprite_is_hidden = false,
                sprite_is_leftward = false,
                sprite_is_upended = false;

            std::filesystem::path sprite_texture_path{};

            RectI sprite_source_rect = { 0, 0, 16, 16 };
            Vec2F sprite_offset{},
                sprite_origin{};

            const size_t texture_label = text.find("texture", sprite_open);
            if (texture_label < sprite_close) {
                const size_t end_line = text.find("\n", texture_label);
                size_t texture_start = text.find("=", texture_label);
                if (texture_start < end_line) {
                    ++texture_start;
                    while (text.at(texture_start) == '	' || text.at(texture_start) == ' ') ++texture_start;
                    sprite_texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);
                    //console::log("texture: ", texture_path, "\n");
                }
            }
            const size_t is_debug_label = text.find("is_debug", sprite_open);
            if (is_debug_label < sprite_close) {
                const size_t end_line = text.find("\n", is_debug_label);
                size_t is_debug_start = text.find("=", is_debug_label);
                if (is_debug_start < end_line) {
                    ++is_debug_start;
                    while (text.at(is_debug_start) == '	' || text.at(is_debug_start) == ' ') ++is_debug_start;
                    sprite_is_debug = text.substr(is_debug_start, end_line - is_debug_start) == "true" ? true : false;
                }
            }
            const size_t is_hidden_label = text.find("is_hidden", sprite_open);
            if (is_hidden_label < sprite_close) {
                const size_t end_line = text.find("\n", is_hidden_label);
                size_t is_hidden_start = text.find("=", is_hidden_label);
                if (is_hidden_start < end_line) {
                    ++is_hidden_start;
                    while (text.at(is_hidden_start) == '	' || text.at(is_hidden_start) == ' ') ++is_hidden_start;
                    sprite_is_hidden = text.substr(is_hidden_start, end_line - is_hidden_start) == "true" ? true : false;
                }
            }
            const size_t is_leftward_label = text.find("is_leftward", sprite_open);
            if (is_leftward_label < sprite_close) {
                const size_t end_line = text.find("\n", is_leftward_label);
                size_t is_leftward_start = text.find("=", is_leftward_label);
                if (is_leftward_start < end_line) {
                    ++is_leftward_start;
                    while (text.at(is_leftward_start) == '	' || text.at(is_leftward_start) == ' ') ++is_leftward_start;
                    sprite_is_leftward = text.substr(is_leftward_start, end_line - is_leftward_start) == "true" ? true : false;
                }
            }
            const size_t is_upended_label = text.find("is_upended", sprite_open);
            if (is_upended_label < sprite_close) {
                const size_t end_line = text.find("\n", is_upended_label);
                size_t is_upended_start = text.find("=", is_upended_label);
                if (is_upended_start < end_line) {
                    ++is_upended_start;
                    while (text.at(is_upended_start) == '	' || text.at(is_upended_start) == ' ') ++is_upended_start;
                    sprite_is_upended = text.substr(is_upended_start, end_line - is_upended_start) == "true" ? true : false;
                }
            }
            const size_t source_rect_label = text.find("source_rect", sprite_open);
            if (source_rect_label < sprite_close) {
                const size_t end_line = text.find("\n", source_rect_label);
                const size_t source_rect_start = text.find("=", source_rect_label);
                if (source_rect_start < end_line) {
                    const size_t source_rect_open = text.find("{", source_rect_start);
                    const size_t source_rect_close = text.find("}", source_rect_start);

                    if (source_rect_open < end_line && source_rect_close < end_line) {
                        size_t value_0 = source_rect_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_source_rect.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        const size_t comma_1 = text.find(",", value_1);
                        if (comma_1 < end_line) {
                            sprite_source_rect.y = std::stoi(text.substr(value_1, comma_1 - value_1));
                        }
                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
                        const size_t comma_2 = text.find(",", value_2);
                        if (comma_2 < end_line) {
                            sprite_source_rect.w = std::stoi(text.substr(value_2, comma_2 - value_2));
                        }
                        size_t value_3 = comma_2 + 1;
                        while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
                        size_t value_3_end = source_rect_close;
                        while (text.at(value_3_end) == '	' || text.at(value_3_end) == ' ') --value_3_end;

                        sprite_source_rect.h = std::stoi(text.substr(value_3, value_3_end - value_3));
                        //console::log("entity::Object::load_config source_rect: ", sprite_source_rect.x, " ", sprite_source_rect.y, " ", sprite_source_rect.w, " ", sprite_source_rect.h, "\n");                            
                    }
                }
            }
            const size_t offset_label = text.find("offset", sprite_open);
            if (offset_label < sprite_close) {
                const size_t end_line = text.find("\n", offset_label);
                size_t offset_start = text.find("=", offset_label);
                if (offset_start < end_line) {
                    const size_t offset_open = text.find("{", offset_start);
                    const size_t offset_close = text.find("}", offset_start);

                    if (offset_open < end_line && offset_close < end_line) {
                        size_t value_0 = offset_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_offset.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = offset_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        sprite_offset.y = std::stof(text.substr(value_1, value_1_end - value_1));
                        //console::log("entity::Object::load_config offset: ", offset.x, " ", offset.y, "\n");
                    }
                }
            }
            const size_t origin_label = text.find("origin", sprite_open);
            if (origin_label < sprite_close) {
                const size_t end_line = text.find("\n", origin_label);
                size_t origin_start = text.find("=", origin_label);
                if (origin_start < end_line) {
                    const size_t origin_open = text.find("{", origin_start);
                    const size_t origin_close = text.find("}", origin_start);

                    if (origin_open < end_line && origin_close < end_line) {
                        size_t value_0 = origin_open + 1;
                        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            sprite_origin.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = origin_close;
                        while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                        sprite_origin.y = std::stof(text.substr(value_1, value_1_end - value_1));
                    }
                }
            }

            if (sprite_texture_path.empty()) {
                sprite_texture_path = "res/texture/set_255.png";
            }
            /*if (m_sprite_id != -1) {
                sprite::erase(m_sprite_id);
            }*/
            cI32 id = sprite::make(sprite_texture_path);
            owner->sprite_id(id);

            sprite::layer(id, layer);
            sprite::is_debug(id, sprite_is_debug);
            sprite::is_hidden(id, sprite_is_hidden);
            sprite::is_leftward(id, sprite_is_leftward);
            sprite::is_upended(id, sprite_is_upended);
            sprite::source_rect(id, sprite_source_rect);

            if (sprite_origin == Vec2F{ 0.0f, 0.0f }) {
                sprite::origin(id, { sprite_source_rect.w / 2.0F, sprite_source_rect.h / 2.0F });
            } else {
                sprite::origin(id, sprite_origin);
            }

            sprite::offset(id, start_offset + sprite_offset); // Add to already set level offset

            sprite::start_offset(id, sprite::offset(id));
            //m_start_sprite_offset = sprite::offset(m_sprite_id);

            sprite::transform_id(id, transform_id);
            //console::log(path, " is_hidden: ", sprite_is_hidden, "\n");
        }
    }
    return true;
}