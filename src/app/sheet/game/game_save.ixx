export module sheet.game.save;
import std;
import console;
import types;

static inline std::vector<std::filesystem::path>                s_visited_levels;
static inline std::map<std::filesystem::path, std::vector<U16>> s_picked_coins;

static inline std::filesystem::path s_current_level_path;

export namespace sheet::game {
    void clear_current_save_data() {
        console::log("state::game::clear_current_save_data()\n");
        s_visited_levels.clear();
        s_picked_coins.clear();
    }

    std::filesystem::path& current_level_path() { return s_current_level_path; }
    void current_level_path(const std::filesystem::path& level_path) {
        s_current_level_path = level_path;
    }

    std::vector<std::filesystem::path>& visited_levels() { return s_visited_levels; }

    std::vector<U16>& picked_coins_in_current_level() { return s_picked_coins[s_current_level_path]; };

    bool add_visited_level(const std::filesystem::path& level_path) {
        if (level_path.empty()) {
            console::error("state::game::add_visited_level() level path empty!\n");
            return false;
        }
        //s_current_level_path = level_path;
        for (auto& i : s_visited_levels) {
            if (i.compare(level_path) == 0) {
                console::log("state::game::add_visited_level() level path aldready added\n");
                return false;
            }
        }
        s_visited_levels.push_back(level_path);
        std::sort(s_visited_levels.begin(), s_visited_levels.end());
        return true;
    }

    bool add_picked_coin(cU16 n) {
        if (s_current_level_path.empty()) {
            console::error("state::game::add_picked_coin() level path empty!\n");
            return false;
        }
        for (auto& i : s_picked_coins[s_current_level_path]) {
            if (i == n) {
                console::log("state::game::add_picked_coin() aldready added\n");
                return false;
            }
        }
        s_picked_coins[s_current_level_path].push_back(n);
        std::sort(s_picked_coins[s_current_level_path].begin(), s_picked_coins[s_current_level_path].end());
        //++s_num_coins;
        return true;
    }
    bool is_coin_picked(cU16 n) {
        if (s_current_level_path.empty()) {
            console::error("state::game::is_coin_picked() level path empty!\n");
            return false;
        }
        for (auto& i : s_picked_coins[s_current_level_path]) {
            if (i == n) {
                return true;
            }
        }
        return false;
    }



    const std::string read_save(cU8 slot) {
        std::ostringstream oss;
        oss << (int)slot;
        std::ifstream in_file("res/save/" + oss.str() + ".bin", std::ios::in | std::ios::binary);
        char level_path[32]{};
        U8 num_visited_levels = 0;
        in_file.read((I8*)level_path, sizeof(char) * 32);
        in_file.read((I8*)&num_visited_levels, sizeof(U8));

        console::log("state::game::read_save() level_path: ", level_path, "\n");
        //console::log("\n\nstate::game::read_save() num_visited_levels: ", (int)num_visited_levels, "\n\n");


        for (U8 i = 0; i < num_visited_levels; ++i) {
            char visited_level_path[32]{};
            in_file.read((I8*)visited_level_path, sizeof(char) * 32);
            add_visited_level(visited_level_path);
            //s_numVisitedScreensLevel = 0;
            //inFile.read((i8*)&s_numVisitedScreensLevel, sizeof(u8));
            //u8 visitedScreen = 0;
            //for (u8 j = 0; j < s_numVisitedScreensLevel; ++j) {
                //inFile.read((i8*)&visitedScreen, sizeof(u8));
                //SavesManager::addVisitedScreen(levelPath, (int)visitedScreen);
            //}
            U8 num_picked_coins_in_level = 0;
            in_file.read((I8*)&num_picked_coins_in_level, sizeof(U8));
            char picked_coin = 0;
            for (U8 j = 0; j < num_picked_coins_in_level; ++j) {
                in_file.read((I8*)&picked_coin, sizeof(U8));
                s_current_level_path = visited_level_path;
                add_picked_coin((int)picked_coin);
            }
        }

        s_current_level_path = level_path;
        return std::string(level_path);
    }
    void write_save(cU8 slot) {
        if (s_current_level_path.empty()) {
            console::error("state::game::write_save() level_path empty!\n");
            return;
        }
        console::log("state::game::write_save() ", s_current_level_path.string(), "...");

        std::ofstream out_file("res/save/0.bin", std::ios::out | std::ios::binary);
        
        char save_path[32]{};
        s_current_level_path.string().copy(save_path, 32);

        out_file.write((I8*)save_path, sizeof(char) * 32);

        console::log("visited levels size: ", s_visited_levels.size(), "\n");

        cU8 num_visited_levels = (U8)s_visited_levels.size();
        out_file.write((I8*)&num_visited_levels, sizeof(U8));


        for (auto& visited_level_path : s_visited_levels) {
            const char* visited_level_path_c_str = visited_level_path.string().c_str();

            out_file.write((I8*)visited_level_path_c_str, sizeof(U8) * 32);

            cU8 num_picked_coins_in_level = (U8)s_picked_coins[visited_level_path].size();

            out_file.write((I8*)&num_picked_coins_in_level, sizeof(U8));

            U8* temp_array = new U8[num_picked_coins_in_level];
            U8 picked_coin = 0;
            for (U8 j = 0; j < num_picked_coins_in_level; ++j) {
                picked_coin = (int)s_picked_coins[visited_level_path].at(j);
                temp_array[j] = picked_coin;
            }
            out_file.write((I8*)temp_array, sizeof(U8) * (num_picked_coins_in_level));
            delete[] temp_array;
        }






        console::log("done.\n");
    }

}