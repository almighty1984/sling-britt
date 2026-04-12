module;
#include <string>
#include <sstream>
#include <iostream>
#include <deque>

export module console;

export namespace console {
    enum class Type {
        log = 0,
        warning = 1,
        error = 2
    };
}

std::string s_log, s_prev_text, s_prev_command;

std::deque<std::pair<console::Type, std::string>> s_lines;

//template<typename T>
//concept StringLike = std::is_convertible_v<T, std::string_view>;

export namespace console {
    std::string& get_log() { return s_log; }

    std::deque<std::pair<Type, std::string>>& get_lines() { return s_lines; }

    void log() { }
    template <typename First, typename... Rest> //requires StringLike<First> and (StringLike<Rest> and ...)    
    void log(First first, Rest... rest) {
        std::ostringstream oss;
        oss << first;
        std::string text = oss.str();    // might need c_str()
        if (!text.empty()) {
            std::cout << text;
            if (text.back() != '\n') {
                s_prev_text += text;
            }
            else {
                s_log.append(s_prev_text + text);
                s_lines.push_back({ Type::log, s_prev_text + text });
                if (s_lines.size() > 35) {
                    s_lines.pop_front();
                }
                s_prev_text.clear();
            }
        }
        log(rest...);
    }

    void warning() { }
    template <typename First, typename... Rest>
    void warning(First first, Rest... rest) {
        std::ostringstream oss;
        oss << first;
        std::string text = oss.str();
        if (!text.empty()) {
            std::cout << text;
            if (text.back() != '\n') {
                s_prev_text += text;
            } else {
                s_log.append(s_prev_text + text);
                s_lines.push_back({ Type::warning, s_prev_text + text });
                if (s_lines.size() > 35) {
                    s_lines.pop_front();
                }
                s_prev_text.clear();
            }
        }
        warning(rest...);
    }


    void error() { }
    template <typename First, typename... Rest>
    void error(First first, Rest... rest) {
        std::ostringstream oss;
        oss << first;
        std::string text = oss.str();
        if (!text.empty()) {
            std::cout << text;
            if (text.back() != '\n') {
                s_prev_text += text;
            } else {
                s_log.append(s_prev_text + text);
                s_lines.push_back({ Type::error, s_prev_text + text });
                if (s_lines.size() > 35) {
                    s_lines.pop_front();
                }
                s_prev_text.clear();
            }
        }
        error(rest...);
    }
}