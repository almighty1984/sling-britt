export module aabb.config;
import aabb;
import entity;
import types;
import std;

export namespace aabb {
    bool parse_config(const std::string& text, entity::Object* owner) {
        const size_t aabb_set_label = text.find("AABB_Set", 0);
        const size_t aabb_colors_label = text.find("AABB_Colors", 0);

        if (aabb_set_label == std::string::npos) {
            //goto end_of_aabb_block;
            return false;
        }
        if (text.find("{", aabb_set_label) != std::string::npos) {
            const size_t aabb_set_open = text.find("{", aabb_set_label) + 1;
            const size_t aabb_set_close = text.find("\n}", aabb_set_open);

            if (aabb_set_close != std::string::npos) {
                std::vector<I32> aabb_ids{};

                size_t aabb_open = text.find("{", aabb_set_open + 1);

                while (aabb_open < aabb_set_close) {

                    RectF aabb_rect{ 0.0F, 0.0F, 16.0F, 16.0F };
                    aabb::Name aabb_name = aabb::Name::none;

                    size_t end_line = text.find("\n", aabb_open);
                    size_t aabb_close = text.find("}", aabb_open);

                    size_t prev_endl = text.rfind("\n", aabb_open);
                    if (aabb_close < end_line) {

                        // See if there's a name in front
                        size_t name_end = text.rfind("=", aabb_open);

                        if (name_end > prev_endl and name_end != std::string::npos) {
                            //console::log("\n\nname end: ", text.at(name_end), "\n\n");
                            //--name_end;
                            while (text.at(name_end - 1) == '	' or text.at(name_end - 1) == ' ') {
                                --name_end;
                                //console::log("\nname end: ", text.at(name_end), "\n");
                                //console::log("name_end: ", name_end, "\n");
                            }
                            if (name_end > prev_endl) {
                                size_t name_start = name_end;
                                while (text.at(name_start - 1) != '	' and text.at(name_start - 1) != ' ' and name_start > prev_endl) {
                                    --name_start;
                                }
                                const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                                aabb_name = aabb::string_to_name(aabb_name_str);

                                //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");
                            }
                        }
                        size_t value_0 = aabb_open + 1;
                        while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                        size_t comma_0 = text.find(",", value_0);
                        aabb_rect.x = std::stof(text.substr(value_0, comma_0 - value_0));

                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                        size_t comma_1 = text.find(",", value_1);
                        aabb_rect.y = std::stof(text.substr(value_1, comma_1 - value_1));

                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' or text.at(value_2) == ' ') ++value_2;
                        size_t comma_2 = text.find(",", value_2);
                        aabb_rect.w = std::stof(text.substr(value_2, comma_2 - value_2));

                        size_t value_3 = comma_2 + 1;
                        while (text.at(value_3) == '	' or text.at(value_3) == ' ') ++value_3;
                        size_t comma_3 = text.find(",", value_3);
                        aabb_rect.h = std::stof(text.substr(value_3, comma_3 - value_3));

                        //out_rects.emplace_back(rect);
                        aabb_open = text.find("{", end_line);
                    }

                    cI32 aabb_id = aabb::make(owner->transform(), { owner->start_offset().x + aabb_rect.x,
                                                                       owner->start_offset().y + aabb_rect.y,
                                                                       aabb_rect.w,
                                                                       aabb_rect.h });
                    aabb::name(aabb_id, aabb_name);
                    aabb::owner(aabb_id, owner);
                    //aabb::get(aabb_id)->update();
                    aabb_ids.emplace_back(aabb_id);

                    //aabb_set_label = aabb_set_close;
                }
                owner->aabb_ids(aabb_ids);
            }
        }
        if (aabb_colors_label == std::string::npos) {
            //goto end_of_aabb_block;
            return true;
        }
        if (text.find("{", aabb_colors_label) != std::string::npos) {
            const size_t aabb_colors_open = text.find("{", aabb_colors_label) + 1;
            const size_t aabb_colors_close = text.find("\n}", aabb_colors_open);

            if (aabb_colors_close != std::string::npos) {
                std::vector<I32> aabb_ids{};

                size_t color_open = text.find("{", aabb_colors_open + 1);

                while (color_open < aabb_colors_close) {
                    aabb::Name aabb_name = aabb::Name::none;
                    Color aabb_color;

                    size_t end_line = text.find("\n", color_open);
                    size_t color_close = text.find("}", color_open);

                    size_t prev_endl = text.rfind("\n", color_open);
                    if (color_close < end_line) {
                        size_t name_end = text.rfind("=", color_open);

                        if (name_end > prev_endl and name_end != std::string::npos) {
                            //console::log("\n\nname end: ", text.at(name_end), "\n\n");
                            //--name_end;
                            while (text.at(name_end - 1) == '	' or text.at(name_end - 1) == ' ') {
                                --name_end;
                                //console::log("\nname end: ", text.at(name_end), "\n");
                                //console::log("name_end: ", name_end, "\n");
                            }
                            if (name_end > prev_endl) {
                                size_t name_start = name_end;
                                while (text.at(name_start - 1) != '	' and text.at(name_start - 1) != ' ' and name_start > prev_endl) {
                                    --name_start;
                                }

                                const std::string aabb_name_str = text.substr(name_start, name_end - name_start);

                                //console::log("entity::Object::load_config(", path, ") aabb_name_str: ", aabb_name_str, "\n");

                                aabb_name = aabb::string_to_name(aabb_name_str);
                            }
                        }

                        size_t value_0 = color_open + 1;
                        while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                        size_t comma_0 = text.find(",", value_0);
                        aabb_color.r = std::stoi(text.substr(value_0, comma_0 - value_0));

                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                        size_t comma_1 = text.find(",", value_1);
                        aabb_color.g = std::stoi(text.substr(value_1, comma_1 - value_1));

                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' or text.at(value_2) == ' ') ++value_2;
                        size_t comma_2 = text.find(",", value_2);
                        aabb_color.b = std::stoi(text.substr(value_2, comma_2 - value_2));

                        color_open = text.find("{", end_line);
                    }

                    for (auto& aabb_id : owner->aabb_ids()) {
                        if (aabb::name(aabb_id) == aabb_name) {
                            aabb::color(aabb_id, aabb_color);
                            //aabb::start_color(aabb_id, aabb_color);
                        }
                    }
                    //aabb_set_label = aabb_colors_close;
                }
            }
        }
        return true;
    }
}