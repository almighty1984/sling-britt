export module anim.config;
import anim;
import entity;
import types;
import std;

export namespace anim {
    bool parse_config(const std::string& text, entity::Object* owner) {
        if (text.find("Animations", 0) == std::string::npos) {
            return false;
        }
        const size_t animations_label = text.find("Animations", 0);

        if (text.find("{", animations_label) == std::string::npos) {
            return false;
        }
        const size_t animations_open = text.find("{", animations_label) + 1;
        const size_t animations_close = text.find("\n}", animations_open);
        if (animations_close == std::string::npos) {
            return false;
        }
        size_t current_equals = animations_open;

        while (1) {
            current_equals = text.find("=", current_equals + 1);
            if (current_equals > animations_close) {
                return false;
            }
            const size_t end_line = text.find("\n", current_equals);
            const size_t current_open = text.find("{", current_equals);
            const size_t current_close = text.find("}", current_equals);

            if (current_open < end_line and current_close < end_line) {
                U16 source_y = 0;
                F32 speed = 0.0F;
                U16 num_loops = 0;

                size_t value_0 = current_open + 1;
                while (text.at(value_0) == '	' or text.at(value_0) == ' ') ++value_0;
                const size_t comma_0 = text.find(",", value_0);
                if (comma_0 < end_line) {
                    source_y = std::stoi(text.substr(value_0, comma_0 - value_0));
                }
                size_t value_1 = comma_0 + 1;
                while (text.at(value_1) == '	' or text.at(value_1) == ' ') ++value_1;
                const size_t comma_1 = text.find(",", value_1);
                if (comma_1 < end_line) {
                    speed = std::stof(text.substr(value_1, comma_1 - value_1));
                }
                size_t value_2 = comma_1 + 1;
                while (text.at(value_2) == '	' or text.at(value_2) == ' ') ++value_2;
                size_t value_2_end = current_close;
                while (text.at(value_2_end) == '	' or text.at(value_2_end) == ' ') --value_2_end;
                num_loops = std::stoi(text.substr(value_2, value_2_end - value_2));

                size_t current_label_start = text.rfind("\n", current_equals);
                while (text.at(current_label_start) == '	' or text.at(current_label_start) == ' ' or text.at(current_label_start) == '\n') ++current_label_start;
                size_t current_label_end = current_equals;
                while (text.at(current_label_end - 1) == '	' or text.at(current_label_end - 1) == ' ') --current_label_end;

                std::string current_label_str = text.substr(current_label_start, current_label_end - current_label_start);

                //m_current_anim = m_anims[current_label_str] = anim::make();
                
                cI32 id = anim::make();
                owner->anim(current_label_str, id);

                cI32 sprite = owner->sprite();

                anim::texture_size(id, sprite::texture_size(sprite));
                anim::source(id, { 0, source_y, sprite::rect(sprite).w, sprite::rect(sprite).h });
                anim::loops(id, num_loops);
                anim::type(id, anim::string_to_type(current_label_str));

                anim::first_frame(id, 0);
                int num_frames = anim::texture_size(id).x / sprite::rect(sprite).w;
                anim::last_frame(id, num_frames - 1);
                if (speed < 0.0F) {
                    speed = -speed;
                    anim::is_reverse(id, true);

                    anim::first_frame(id, num_frames - 1);
                    anim::last_frame(id, 0);
                    //console::log("anim: ", anim::to_string(anim::type(m_current_anim)), " speed: ", speed, "\n");                            
                }
                anim::speed(id, speed);

                owner->current_anim(id);
            }
        }
        return true;
    }
}
