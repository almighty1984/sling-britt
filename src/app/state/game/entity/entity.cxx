module;
#include <cmath>
#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

module entity;
import camera;
import aabb;
import console;
import health;
import sound;
import sprite;
import particle_system;
import aabb.trait;
import aabb.config;
import anim.config;

namespace entity {
    void Object::draw_debug(std::unique_ptr<Window>& window) {
        sprite::draw(window, m_sprite_id);
        for (auto& i : m_aabb_ids) {
            aabb::draw(window, i);
        }
    }
    Object::~Object() {
        //console::log(class_name(), "::~Object() ", entity::to_string(m_type), "\n");
        camera::remove_transform_id(m_transform_id);
        for (auto& i : m_aabb_ids) {
            aabb::erase(i);
        }
        m_aabb_ids.clear();
        m_input_objects.clear();

        for (const auto& [key, anim_id] : m_anim_ids) {
            anim::erase(anim_id);
        }
        for (const auto& [key, sound_id] : m_sound_ids) {
            sound::erase(sound_id);
        }

        health::erase(m_health_id);
        sprite::erase(m_sprite_id);

        //if (m_start_offset == Vec2F{ 0.0F, 0.0F}) {
        if (!transform::is_level(m_transform_id)) {
            transform::erase(m_transform_id);
        }
        //}    
    }
    bool Object::load_config(const std::filesystem::path& path) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error("config::load ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        entity::parse_config(text, this);
        sound::parse_config(text, this);

        if (transform::parse_config(text, this)) {
            camera::add_transform_id(m_transform_id);
            health::parse_config(text, this);
        }
        if (sprite::parse_config(text, this, m_transform_id, m_start_layer, m_start_offset)) {
            anim::parse_config(text, this);
        }

        aabb::parse_config(text, this);

        //const size_t aabb_set_label = text.find("AABB_Set", 0);
        //const size_t aabb_colors_label = text.find("AABB_Colors", 0);

        //if (aabb_set_label == std::string::npos || text.find("{", aabb_set_label) == std::string::npos) {
        //    return true;
        //}
        //
        //const size_t aabb_set_open = text.find("{", aabb_set_label) + 1;
        //const size_t aabb_set_close = text.find("\n}", aabb_set_open);

        //if (aabb_set_close == std::string::npos) {
        //    return false;
        //}
        //std::vector<I32> aabb_ids{};

        //size_t aabb_open = text.find("{", aabb_set_open + 1);

        //while (aabb_open < aabb_set_close) {

        //    RectF aabb_rect{ 0.0F, 0.0F, 16.0F, 16.0F };
        //    aabb::Name aabb_name = aabb::Name::none;

        //    size_t end_line = text.find("\n", aabb_open);
        //    size_t aabb_close = text.find("}", aabb_open);

        //    size_t prev_endl = text.rfind("\n", aabb_open);
        //    if (aabb_close < end_line) {

        //        // See if there's a name in front
        //        size_t name_end = text.rfind("=", aabb_open);

        //        if (name_end > prev_endl && name_end != std::string::npos) {
        //            //console::log("\n\nname end: ", text.at(name_end), "\n\n");
        //            //--name_end;
        //            while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
        //                --name_end;
        //                //console::log("\nname end: ", text.at(name_end), "\n");
        //                //console::log("name_end: ", name_end, "\n");
        //            }
        //            if (name_end > prev_endl) {
        //                size_t name_start = name_end;
        //                while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
        //                    --name_start;
        //                }
        //                const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

        //                aabb_name = aabb::string_to_name(aabb_name_str);

        //                //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");
        //            }
        //        }
        //        size_t value_0 = aabb_open + 1;
        //        while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
        //        size_t comma_0 = text.find(",", value_0);
        //        aabb_rect.x = std::stof(text.substr(value_0, comma_0 - value_0));

        //        size_t value_1 = comma_0 + 1;
        //        while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
        //        size_t comma_1 = text.find(",", value_1);
        //        aabb_rect.y = std::stof(text.substr(value_1, comma_1 - value_1));

        //        size_t value_2 = comma_1 + 1;
        //        while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
        //        size_t comma_2 = text.find(",", value_2);
        //        aabb_rect.w = std::stof(text.substr(value_2, comma_2 - value_2));

        //        size_t value_3 = comma_2 + 1;
        //        while (text.at(value_3) == '	' || text.at(value_3) == ' ') ++value_3;
        //        size_t comma_3 = text.find(",", value_3);
        //        aabb_rect.h = std::stof(text.substr(value_3, comma_3 - value_3));

        //        //out_rects.emplace_back(rect);
        //        aabb_open = text.find("{", end_line);
        //    }

        //    cI32 aabb_id = aabb::make(m_transform_id, { m_start_offset.x + aabb_rect.x,
        //                                                m_start_offset.y + aabb_rect.y,
        //                                                aabb_rect.w,
        //                                                aabb_rect.h });
        //    aabb::name(aabb_id, aabb_name);
        //    aabb::owner(aabb_id, this);
        //    //aabb::get(aabb_id)->update();
        //    aabb_ids.emplace_back(aabb_id);

        //    //aabb_set_label = aabb_set_close;
        //}
        //m_aabb_ids = aabb_ids;

        //if (aabb_colors_label == std::string::npos || text.find("{", aabb_colors_label) == std::string::npos) {
        //    return true;
        //}
        //
        //const size_t aabb_colors_open = text.find("{", aabb_colors_label) + 1;
        //const size_t aabb_colors_close = text.find("\n}", aabb_colors_open);

        //if (aabb_colors_close != std::string::npos) {
        //    std::vector<I32> aabb_ids{};

        //    size_t color_open = text.find("{", aabb_colors_open + 1);

        //    while (color_open < aabb_colors_close) {
        //        aabb::Name aabb_name = aabb::Name::none;
        //        Color aabb_color;

        //        size_t end_line = text.find("\n", color_open);
        //        size_t color_close = text.find("}", color_open);

        //        size_t prev_endl = text.rfind("\n", color_open);
        //        if (color_close < end_line) {
        //            size_t name_end = text.rfind("=", color_open);

        //            if (name_end > prev_endl && name_end != std::string::npos) {
        //                //console::log("\n\nname end: ", text.at(name_end), "\n\n");
        //                //--name_end;
        //                while (text.at(name_end - 1) == '	' || text.at(name_end - 1) == ' ') {
        //                    --name_end;
        //                    //console::log("\nname end: ", text.at(name_end), "\n");
        //                    //console::log("name_end: ", name_end, "\n");
        //                }
        //                if (name_end > prev_endl) {
        //                    size_t name_start = name_end;
        //                    while (text.at(name_start - 1) != '	' && text.at(name_start - 1) != ' ' && name_start > prev_endl) {
        //                        --name_start;
        //                    }

        //                    const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

        //                    //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");

        //                    aabb_name = aabb::string_to_name(aabb_name_str);
        //                }
        //            }

        //            size_t value_0 = color_open + 1;
        //            while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
        //            size_t comma_0 = text.find(",", value_0);
        //            aabb_color.r = std::stoi(text.substr(value_0, comma_0 - value_0));

        //            size_t value_1 = comma_0 + 1;
        //            while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
        //            size_t comma_1 = text.find(",", value_1);
        //            aabb_color.g = std::stoi(text.substr(value_1, comma_1 - value_1));

        //            size_t value_2 = comma_1 + 1;
        //            while (text.at(value_2) == '	' || text.at(value_2) == ' ') ++value_2;
        //            size_t comma_2 = text.find(",", value_2);
        //            aabb_color.b = std::stoi(text.substr(value_2, comma_2 - value_2));

        //            color_open = text.find("{", end_line);
        //        }

        //        for (auto& aabb_id : m_aabb_ids) {
        //            if (aabb::name(aabb_id) == aabb_name) {
        //                aabb::color(aabb_id, aabb_color);
        //                aabb::start_color(aabb_id, aabb_color);
        //            }
        //        }
        //        //aabb_set_label = aabb_colors_close;
        //    }
        //}
        return true;
    }
}