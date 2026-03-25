module;
#include <fstream>
#include <filesystem>
#include <string>

export module app.config;
import console;
import types;

namespace app::config {
    static U8 s_scale = 1;
    static state::Type s_start_state = state::Type::none;

    static Vec2U s_window_size = { 320, 180 };
}

export namespace app::config {
    U8 scale() { return s_scale; }
    state::Type start_state() { return s_start_state; }
    Vec2U window_size() { return s_window_size; }

    bool load(const std::filesystem::path& path) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error("app::config::load() ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();
        std::string text = oss.str();

        {
            const size_t app_label = text.find("App", 0);
            if (app_label != std::string::npos) {
                if (text.find("{", app_label) != std::string::npos) {
                    const size_t app_open = text.find("{", app_label) + 1;

                    const size_t app_close = text.find("\n}", app_open);
                    if (app_close != std::string::npos) {
                        const size_t start_state_label = text.find("start_state", app_open);
                        size_t end_line = text.find("\n", start_state_label);
                        size_t start_state_start = text.find("=", start_state_label);
                        if (start_state_start < end_line) {
                            ++start_state_start;
                            while (text.at(start_state_start) == '	' || text.at(start_state_start) == ' ') {
                                ++start_state_start;
                            }
                            const std::string start_state_str = text.substr(start_state_start, end_line - start_state_start);
                            s_start_state = state::type_from(start_state_str);

                            console::warning("app::config::load() start_state: ", start_state_str, "\n");
                        }
                    }
                }
            }
        }
        {
            const size_t window_label = text.find("Window", 0);
            if (window_label != std::string::npos) {
                if (text.find("{", window_label) != std::string::npos) {
                    const size_t window_open = text.find("{", window_label) + 1;

                    const size_t window_close = text.find("\n}", window_open);
                    if (window_close != std::string::npos) {


                        const size_t scale_label = text.find("scale", window_open);
                        size_t end_line = text.find("\n", scale_label);
                        size_t scale_start = text.find("=", scale_label);
                        if (scale_start < end_line) {
                            ++scale_start;
                            while (text.at(scale_start) == '	' || text.at(scale_start) == ' ') {
                                ++scale_start;
                            }
                            const std::string scale_str = text.substr(scale_start, end_line - scale_start);

                            console::warning("app::config::load() scale: ", s_scale, "\n");
                            s_scale = std::stoi(scale_str);
                        }



                        const size_t size_label = text.find("size", window_open);
                        if (size_label < window_close) {
                            const size_t end_line = text.find("\n", size_label);
                            size_t size_start = text.find("=", size_label);
                            if (size_start < end_line) {
                                const size_t size_open = text.find("{", size_start);
                                const size_t size_close = text.find("}", size_start);

                                if (size_open < end_line && size_close < end_line) {
                                    size_t value_0 = size_open + 1;
                                    while (text.at(value_0) == '	' || text.at(value_0) == ' ') ++value_0;
                                    const size_t comma_0 = text.find(",", value_0);
                                    if (comma_0 < end_line) {
                                        s_window_size.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                                    }
                                    size_t value_1 = comma_0 + 1;
                                    while (text.at(value_1) == '	' || text.at(value_1) == ' ') ++value_1;
                                    size_t value_1_end = size_close;
                                    while (text.at(value_1_end) == '	' || text.at(value_1_end) == ' ') --value_1_end;

                                    s_window_size.y = std::stof(text.substr(value_1, value_1_end - value_1));


                                    console::warning("app::config::load() size: ", s_window_size.x, " ", s_window_size.y, "\n");
                                    
                                }
                            }
                        }


                    }
                }
            }
        }

        

        return true;
    }
}