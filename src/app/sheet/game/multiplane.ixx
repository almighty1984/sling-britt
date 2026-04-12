export module plane;
import console;
import sprite;
import transform;
import types;
import std;

export class Multiplane {
    std::vector<I32>   m_sprites;
    std::vector<Vec2F> m_scroll_amount;

    std::map<I32, I32> m_transforms_map;

    U8 m_num_layers  = 0,
        m_num_rows    = 0,
        m_num_columns = 0;
public:
    Multiplane() : m_sprites(),
                    m_transforms_map(),
                    m_scroll_amount(),                   
                    m_num_layers(0),
                    m_num_columns(0) {            
    }                    
    ~Multiplane() {
        console::log("Multiplane::~Multiplane()\n");
        clear();
    }
    void clear() {
        console::log("Multiplane::clear()\n");
        for (auto& i : m_transforms_map) {
            transform::erase(i.second);
        }
        m_transforms_map.clear();
        for (auto& i : m_sprites) {
            sprite::erase(i);
        }
        m_sprites.clear();
        m_scroll_amount.clear();
    }
    std::vector<I32> get_sprites() const { return m_sprites; }
    //std::vector<I32> const get_transforms() { return m_transforms; }

    void set_velocity(cVec2F v) {
        if (m_transforms_map.empty()) return;

        for (U8 layer = 0; layer < m_num_layers; layer += 1) {
            if (layer < m_scroll_amount.size()) {
                transform::velocity(m_transforms_map.at(layer), { v.x * (m_scroll_amount.at(layer)).x, 0.0F });
            }
        }
    }
    void create(cU8 columns, cU8 rows) {
        console::log("Multiplane::create ", (int)columns, "x", (int)rows, "\n");
        load_config("res/multiplane/forest.cfg", columns, rows);
    }
    bool load_config(const std::filesystem::path& path, cU8 columns, cU8 rows) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error("Multiplane::load_config() ", path, " not found!\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        in_file.close();

        const std::string text = oss.str();

        size_t start_find_pos = 0;

        //clear();

        while (1) {
            const size_t plane_label = text.find("Plane", start_find_pos);
            if (plane_label == std::string::npos) {
                break;
            }
            console::log("Multiplane::load_config plane_label: ", plane_label, "\n");
            start_find_pos = plane_label + 5;                                
            size_t plane_open = text.find("{", plane_label);
            //console::log("Multiplane::load_config start_find_pos: ", start_find_pos, "\n");
            std::filesystem::path texture_path{};
            RectI rect = {};
            Vec2F velocity = {};

            U8 layer = 0;

            if (plane_open == std::string::npos) {
                console::log("plane_open == std::string::npos\n");
                break;
            }

            ++plane_open;
            size_t plane_close = plane_open;

            while (1) {
                plane_close = text.find("}", plane_close);
                if (plane_close == std::string::npos) return false;

                ++plane_close;
                while (text.at(plane_close) == '	' or text.at(plane_close) == ' ' or text.at(plane_close) == '\n') ++plane_close;
                if (text.at(plane_close) == '}') {
                    break;
                }
                    
            }

            /*if (plane_close == std::string::npos) {
                console::log("plane_close == std::string::npos\n");
                break;
            }*/

            const size_t texture_label = text.find("texture", plane_open);
            if (texture_label < plane_close) {
                const size_t end_line = text.find("\n", texture_label);
                size_t texture_start = text.find("=", texture_label);
                if (texture_start < end_line) {
                    ++texture_start;
                    while (text.at(texture_start) == '	' or text.at(texture_start) == ' ') ++texture_start;
                    texture_path = "res/texture/" + text.substr(texture_start, end_line - texture_start);
                    //console::log("texture: ", texture_path, "\n");

                    start_find_pos = end_line;
                }
            }
            const size_t rect_label = text.find("rect", plane_open);
            if (rect_label < plane_close) {
                const size_t end_line = text.find("\n", rect_label);
                const size_t rect_start = text.find("=", rect_label);
                if (rect_start < end_line) {
                    const size_t rect_open = text.find("{", rect_start);
                    const size_t rect_close = text.find("}", rect_start);

                    if (rect_open < end_line and rect_close < end_line) {                                        
                        size_t value_0 = rect_open + 1;
                        while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            rect.x = std::stoi(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                        const size_t comma_1 = text.find(",", value_1);
                        if (comma_1 < end_line) {
                            rect.y = std::stoi(text.substr(value_1, comma_1 - value_1));
                        }
                        size_t value_2 = comma_1 + 1;
                        while (text.at(value_2) == '	' or text.at(value_2) == ' ') ++value_2;
                        const size_t comma_2 = text.find(",", value_2);
                        if (comma_2 < end_line) {
                            rect.w = std::stoi(text.substr(value_2, comma_2 - value_2));
                        }

                        size_t value_3 = comma_2 + 1;
                        while (text.at(value_3) == '	' or text.at(value_3) == ' ') ++value_3;
                        size_t value_3_end = rect_close;
                        while (text.at(value_3_end) == '	' or text.at(value_3_end) == ' ') --value_3_end;

                        rect.h = std::stoi(text.substr(value_3, value_3_end - value_3));
                        //console::log("entity::Object::load_config rect: ", rect.x, " ", rect.y, " ", rect.w, " ", rect.h, "\n");

                        start_find_pos = end_line;
                    }
                }
            }
            const size_t velocity_label = text.find("velocity", plane_open);
            if (velocity_label < plane_close) {
                const size_t end_line = text.find("\n", velocity_label);
                const size_t velocity_start = text.find("=", velocity_label);
                if (velocity_start < end_line) {
                    const size_t velocity_open = text.find("{", velocity_start);
                    const size_t velocity_close = text.find("}", velocity_start);

                    if (velocity_open < end_line and velocity_close < end_line) {

                        size_t value_0 = velocity_open + 1;
                        while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                        const size_t comma_0 = text.find(",", value_0);
                        if (comma_0 < end_line) {
                            velocity.x = std::stof(text.substr(value_0, comma_0 - value_0));
                        }
                        size_t value_1 = comma_0 + 1;
                        while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                        size_t value_1_end = velocity_close;
                        while (text.at(value_1_end) == '	' or text.at(value_1_end) == ' ') --value_1_end;

                        velocity.y = std::stof(text.substr(value_1, value_1_end - value_1));
                        //console::log("Multiplane::load_config velocity : ", velocity.x, " ", velocity.y, "\n");
                    }
                    start_find_pos = end_line;
                }
            }

            if (!texture_path.empty()) {
                //m_transforms.emplace_back(transform::make());

                m_transforms_map.emplace(m_num_layers, transform::make());

                m_scroll_amount.emplace_back(velocity);
                //console::log("\nplane layer: ", (int)m_num_layers, " transform id: ", m_transforms.back(), "\n\n");
                    
                for (U8 y = 0; y < rows; ++y) {
                    for (U8 x = 0; x < columns; ++x) {
                        m_sprites.emplace_back(sprite::make(texture_path));
                        sprite::transform(m_sprites.back(), m_transforms_map.at(m_num_layers));
                        sprite::offset(m_sprites.back(), Vec2F{ (F32)(x * rect.w), (F32)(y * rect.h) });

                        //console::log("multiplane sprite offset: ", sprite::offset(m_sprites.back()).x, "\n\n");
                        sprite::rect(m_sprites.back(), rect);
                        sprite::layer(m_sprites.back(), m_num_layers);
                        sprite::is_bg(m_sprites.back(), true);
                    }
                }
                m_num_rows = rows;
                m_num_columns = columns;
                ++m_num_layers;
            }
        }
        //console::log("Multiplane num layers: ", (int)m_num_layers, "\n");
        //console::log("Multiplane transforms size: ", m_transforms_map.size(), "\n");
        //console::log("Multiplane sprites size: ", m_sprites.size(), "\n");
        return true;
    }

    void update() {
        if (m_transforms_map.empty()) return;
        cF32 source_w = (F32)sprite::rect(m_sprites.back()).w;

        for (auto& i : m_transforms_map) {
            transform::position(i.second, { transform::position(i.second).x, 0.0F });
            if (transform::position(i.second).x < -256.0F) {
                transform::position(i.second, transform::position(i.second) + Vec2F{ source_w, 0.0F});
            }
            if (transform::position(i.second).x > 0.0F) {
                transform::position(i.second, transform::position(i.second) - Vec2F{ source_w, 0.0F });
            }
        }
    }
};