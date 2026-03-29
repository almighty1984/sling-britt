export module app.config;
import console;
import types;
import std;

namespace app::config {
    static U8 s_scale = 1;
    static sheet::Type s_start_sheet = sheet::Type::none;

    static Vec2U s_extent = { 320, 180 };
    static RectU s_view   = { 0, 0, 320, 180 };
}

export namespace app::config {
    U8 scale()                { return s_scale;       }
    sheet::Type start_sheet() { return s_start_sheet; }
    Vec2U extent()            { return s_extent;      }
    RectU view()              { return s_view;        } void view(cRectU v) { s_view = v; }

    bool load(const std::filesystem::path& path) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error("app::config::load() ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();
        std::string text = oss.str();

        const size_t app_label = text.find("App", 0);
        if (app_label != std::string::npos) {
            if (text.find("{", app_label) != std::string::npos) {
                const size_t app_open = text.find("{", app_label) + 1;

                const size_t app_close = text.find("\n}", app_open);
                if (app_close != std::string::npos) {
                    const size_t start_sheet_label = text.find("start_sheet", app_open);
                    size_t end_line = text.find("\n", start_sheet_label);
                    size_t start_sheet_start = text.find("=", start_sheet_label);
                    if (start_sheet_start < end_line) {
                        ++start_sheet_start;
                        while (text.at(start_sheet_start) == '	' or text.at(start_sheet_start) == ' ') {
                            ++start_sheet_start;
                        }
                        const std::string start_sheet_str = text.substr(start_sheet_start, end_line - start_sheet_start);
                        s_start_sheet = sheet::type_from(start_sheet_str);

                        console::warning("app::config::load() start_sheet: ", start_sheet_str, "\n");
                    }
                }
            }
        }
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
                        while (text.at(scale_start) == '	' or text.at(scale_start) == ' ') {
                            ++scale_start;
                        }
                        const std::string scale_str = text.substr(scale_start, end_line - scale_start);

                        console::warning("app::config::load() scale: ", s_scale, "\n");
                        s_scale = std::stoi(scale_str);
                    }

                    const size_t extent_label = text.find("extent", window_open);
                    if (extent_label < window_close) {
                        const size_t end_line = text.find("\n", extent_label);
                        size_t extent_start = text.find("=", extent_label);
                        if (extent_start < end_line) {
                            const size_t extent_open = text.find("{", extent_start);
                            const size_t extent_close = text.find("}", extent_start);

                            if (extent_open < end_line and extent_close < end_line) {
                                size_t value_0 = extent_open + 1;
                                while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                                const size_t comma_0 = text.find(",", value_0);
                                if (comma_0 < end_line) {
                                    s_extent.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                                }
                                size_t value_1 = comma_0 + 1;
                                while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                                size_t value_1_end = extent_close;
                                while (text.at(value_1_end) == '	' or text.at(value_1_end) == ' ') --value_1_end;

                                s_extent.y = std::stof(text.substr(value_1, value_1_end - value_1));

                                console::warning("app::config::load() extent: ", s_extent.x, " ", s_extent.y, "\n");                                    
                            }
                        }
                    }


                }
            }
        }

        

        return true;
    }
}