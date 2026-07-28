module sheet.game;
import sheet.game.save;
import camera;
import console;
import entity.bee;
import entity.bee_hive;
import entity.brick;
import entity.bridge;
import entity.bug;
import entity.frog;
import entity.grass;
import entity.conduit;
import entity.flag;
import entity.lever;
import entity.logic_and;
import entity.logic_not;
import entity.logic_or;
import entity.mole;
import entity.spring;
import entity.train;
import entity.track;
import entity.water_line;
import level_config;
import random;
import sprite;

namespace sheet {
    std::map<U16, std::string> Game::load_types_from_bin() {
        std::map<U16, std::string> types_map{};

        const std::filesystem::path path = std::filesystem::current_path() / "res" / "types.bin";

        std::ifstream in_file(path, std::ios::in | std::ios::binary);
        if (!in_file.is_open()) return {};

        U16 type_count = 0;
        in_file.read((I8*)&type_count, sizeof(U16));

        console::log("sheet::Game::load_types_from_bin() count: ", type_count, "\n");

        for (U16 i = 0; i < type_count; ++i) {
            U16 number = 0;
            char type[32] = {};
            in_file.read((I8*)&number, sizeof(U16));
            in_file.read((I8*)type, sizeof(char) * 32);

            //console::log("type: ", number, " ", type, "\n");

            types_map.emplace(number, type);

        }
        in_file.close();
        return types_map;
    }


    void Game::check_to_add_input_from(entity::Object* trigger_entity) {
        //if (trigger_entity->type() != entity::Type::track_trigger_UL) {
        //    //console::log("")
        //    return;
        //}

        entity::Object* current_entity = nullptr;

        entity::Object* prev_entity = trigger_entity;
        entity::Object* start_of_loop_entity = nullptr;

        Vec2F current_pos = {};

        Vec2F offset_to_next = { 0.0F, -16.0F },
            offset_to_next2 = { 0.0F,   0.0F },
            offset_to_next3 = {};
        for (auto& entity : m_unlocked_entities) {
            if (entity->start_offset() == prev_entity->start_offset() + offset_to_next) {
                //console::log("sheet::Game::check_to_add_input_from() entity above trigger: ", entity::to_string(entity->type()), "\n");
                //current_offset = trigger_entity->start_offset() + Vec2F{ 0.0F,-16.0F };
                bool is_track_found = false;
                switch (entity->type()) {
                case entity::Type::conduit_corner_UR: {
                    offset_to_next = { -16.0F, 0.0F };
                    break;
                }
                case entity::Type::track_trigger_UL: {
                    offset_to_next = { -16.0F, 0.0F };
                    is_track_found = true;
                    start_of_loop_entity = entity;
                    break;
                }
                case entity::Type::conduit_corner_UL: {
                    offset_to_next = { 16.0F, 0.0F };
                    break;
                }
                case entity::Type::track_trigger_UR: {
                    offset_to_next = { 16.0F, 0.0F };
                    is_track_found = true;
                    start_of_loop_entity = entity;
                    break;
                }
                case entity::Type::conduit_UD: {
                    offset_to_next = { 0.0F, -16.0F };
                    break;
                }
                case entity::Type::track_UD: {
                    is_track_found = false;
                    offset_to_next = { 0.0F, -16.0F };
                    break;
                }
                }

                entity->add_input(prev_entity);

                console::log(to_string(entity->type()), " add input: ", to_string(prev_entity->type()), "\n");

                entity->direction(offset_to_next);
                prev_entity = entity;

                current_entity = entity;
                current_pos = entity->start_offset();

                if (is_track_found) {
                    console::log("state::Game::check_to_add_input_from() found trigger direction: ", entity->direction().x, " ", entity->direction().y, "\n");
                    break;
                }
            }
        }



        while (1) {
        start_of_loop:

            cVec2F pos = current_entity->start_offset(),
                prev_pos = prev_entity->start_offset();

            std::vector<Vec2F> offsets_to_check;
            //console::log(class_name(), "::check_to_add_input_from() current entity: ", to_string(current_entity->type()), "\n");
            switch (current_entity->type()) {
            case entity::Type::track_trigger_UL: {
                offsets_to_check.push_back({ -16.0F, 0.0F });
                //offset_to_next = { -16.0F, 0.0F };
                start_of_loop_entity = current_entity;
                break;
            }
            case entity::Type::track_trigger_UR: {
                offsets_to_check.push_back({ 16.0F, 0.0F });
                start_of_loop_entity = current_entity;
                break;
            }
            case entity::Type::conduit_UD:
            case entity::Type::logic_not_UD:
            case entity::Type::track_UD: {
                offsets_to_check.push_back(prev_pos.y < pos.y ? Vec2F{ 0.0F,  16.0F } : Vec2F{ 0.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_LR:
            case entity::Type::track_LR: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_corner_DL:
            case entity::Type::track_corner_DL: {
                offsets_to_check.push_back(prev_pos.x > pos.x ? Vec2F{ 0.0F, -16.0F } : Vec2F{ 16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_corner_UL:
            case entity::Type::track_corner_UL: {
                offsets_to_check.push_back(prev_pos.x > pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ 16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_corner_UR:
            case entity::Type::track_corner_UR: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_corner_DR:
            case entity::Type::track_corner_DR: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 0.0F, -16.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_L_1x1_0:
            case entity::Type::track_L_1x1_0: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_L_1x1_1:
            case entity::Type::track_L_1x1_1: {
                offsets_to_check.push_back(prev_pos.y < pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ 0.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_R_1x1_0:
            case entity::Type::track_R_1x1_0: {
                offsets_to_check.push_back(prev_pos.y > pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ 0.0F,  16.0F });
                break;
            }
            case entity::Type::conduit_R_1x1_1:
            case entity::Type::track_R_1x1_1: {
                offsets_to_check.push_back(prev_pos.y < pos.y ? Vec2F{ -16.0F,   0.0F } : Vec2F{ 0.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_L_1x2_0:
            case entity::Type::track_L_1x2_0: {
                offsets_to_check.push_back(prev_pos.y < pos.y ? Vec2F{ 0.0F,  16.0F } : Vec2F{ -16.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_L_1x2_1:
            case entity::Type::track_L_1x2_1: {
                //offset_to_next = prev_pos.y < pos.y ? Vec2F{ 16.0F,  16.0F } : Vec2F{ 0.0F, -16.0F };

                if (prev_pos.y < pos.y) {
                    offsets_to_check.push_back({ 16.0F,  0.0F });
                    offsets_to_check.push_back({ 0.0F, 16.0F });
                    offsets_to_check.push_back({ 16.0F, 16.0F });
                } else {
                    offsets_to_check.push_back({ 0.0F, -16.0F });
                }
                break;
            }
            case entity::Type::conduit_R_1x2_0:
            case entity::Type::track_R_1x2_0: {
                offsets_to_check.push_back(prev_pos.y < pos.y ? Vec2F{ 0.0F,   16.0F } : Vec2F{ 16.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_R_1x2_1:
            case entity::Type::track_R_1x2_1: {
                if (prev_pos.y < pos.y) {
                    offsets_to_check.push_back({ -16.0F,  16.0F });
                    offsets_to_check.push_back({ 0.0F,  16.0F });
                } else {
                    offsets_to_check.push_back({ 0.0F, -16.0F });
                }
                break;
            }
            case entity::Type::conduit_L_2x1_0:
            case entity::Type::track_L_2x1_0: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 16.0F,  16.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            case entity::Type::conduit_L_2x1_1:
            case entity::Type::track_L_2x1_1: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F, -16.0F });
                break;
            }
            case entity::Type::conduit_R_2x1_0:
            case entity::Type::track_R_2x1_0: {
                offsets_to_check.push_back(prev_pos.y > pos.y ? Vec2F{ 16.0F,   0.0F } : Vec2F{ -16.0F,  16.0F });
                break;
            }
            case entity::Type::conduit_R_2x1_1:
            case entity::Type::track_R_2x1_1: {
                if (prev_pos.x < pos.x) {
                    offsets_to_check.push_back({ 16.0F, -16.0F });
                    offsets_to_check.push_back({ 16.0F,   0.0F });
                } else {
                    offsets_to_check.push_back({ -16.0F,  0.0F });
                }
                break;
            }
            case entity::Type::logic_not_LR: {
                offsets_to_check.push_back(prev_pos.x < pos.x ? Vec2F{ 16.0F, 0.0F } : Vec2F{ -16.0F,   0.0F });
                break;
            }
            }

            //console::log("current: ", to_string(current_entity->type()), " offset to next: ", offset_to_next.x, " ", offset_to_next.y, "\n");

            for (auto& offset_to_check : offsets_to_check) {
                auto it = std::find_if(m_unlocked_entities.begin(), m_unlocked_entities.end(),
                    [current_pos, offset_to_check](const entity::Object* e) {
                        return (is_conduit(e->type()) or is_logic(e->type()) or is_track(e->type())) and
                            //offset_to_check != Vec2F{} and
                            e->start_offset() == current_pos + offset_to_check;
                    });

                if (it == m_unlocked_entities.end()) continue;

                if ((*it) == start_of_loop_entity) {
                    console::log("1: found start of loop: ", to_string((*it)->type()), "\n");
                    return;
                }
                prev_entity = current_entity;
                (*it)->add_input(prev_entity);

                //console::log(to_string((*it)->type()), " add input: ", to_string(prev_entity->type()), "\n");
                (*it)->direction(pos - prev_pos);
                //console::log("1: ", to_string((*it)->type()), " ", (*it)->direction().x, " ", (*it)->direction().y, "\n");
                current_entity = (*it);
                current_pos = (*it)->start_offset();

                goto start_of_loop;
            }

            console::log(class_name(), "::check_to_add_input_from() no path forward found.\n");
            return;
        }
    }

    void Game::connect_bridges(cVec2F offset_to_next) {        
        console::log(class_name(), "::connect_bridges()\n");
        std::list<I32> bridges_L;
        std::list<I32> bridges_R;
        for (I16 bridge_i = 0; bridge_i < m_bridge_entities.size(); ++bridge_i) {
            if (!m_bridge_entities.at(bridge_i)) continue;

            if (bridge_i == 0 or m_bridge_entities.at(bridge_i)->start_position().x - m_bridge_entities.at(bridge_i - 1)->start_position().x > 16.0F) {
                m_bridge_entities.at(bridge_i)->is_near_wall_L(true);
                bridges_L.push_back(bridge_i);
            }
            if (bridge_i > 1) {
                if (bridge_i == m_bridge_entities.size() - 1
                    or m_bridge_entities.at(bridge_i + 1)->start_position().x - m_bridge_entities.at(bridge_i)->start_position().x > 16.0F
                    ) {
                    //console::log("\n\nfound the last bridge ", bridge_i, "\n\n");
                    m_bridge_entities.at(bridge_i)->is_near_wall_R(true);
                    bridges_R.push_back(bridge_i);
                }
            }
        }

        console::log("num bridges L: ", bridges_L.size(), "\n");
        console::log("num bridges R: ", bridges_R.size(), "\n");



        for (auto& bridge_L_i : bridges_L) {
            entity::Object* prev_entity = nullptr;
            entity::Object* bridge_entity_L = m_bridge_entities.at(bridge_L_i);
            entity::Object* bridge_entity_R = m_bridge_entities.at(bridges_R.front());
            entity::Object* current_entity = m_bridge_entities.at(bridge_L_i);

            if (!current_entity) continue;

            for (auto& bridge_entity : m_bridge_entities) {
                if (!bridge_entity) continue;

                if (bridge_entity->start_position() == current_entity->start_position() + offset_to_next) {
                    console::log("found bridge entity: ", to_string(bridge_entity->type()), "\n");


                    current_entity->add_root(bridge_entity_L);
                    current_entity->add_root(bridge_entity_R);

                    prev_entity = current_entity;
                    current_entity = bridge_entity;

                    if (prev_entity) {
                        prev_entity->add_child(current_entity);
                    }
                    current_entity->add_child(prev_entity);
                }
                if (current_entity->is_near_wall_R()) {
                    console::log("found bridge R: ", to_string(bridge_entity->type()), "\n");
                    bridges_R.pop_front();
                    break;
                }
            }
        }
    }

    void Game::connect_water() {
        for (I16 water_i = 0; water_i < m_water_entities.size(); ++water_i) {
            if (!m_water_entities.at(water_i)) continue;

            if (m_water_entities.at(water_i)->type() == entity::Type::water_line_L) {
                m_water_entities.at(water_i)->add_input(m_water_entities.at(water_i + 1));
                //console::log("water_L 0\n");
            } else if (m_water_entities.at(water_i)->type() == entity::Type::water_line_R) {
                m_water_entities.at(water_i)->add_input(m_water_entities.at(water_i - 1));
                //console::log("water_R 15\n");
            } else {
                if (water_i - 1 >= 0 and m_water_entities.at(water_i - 1)) {
                    m_water_entities.at(water_i)->add_input(m_water_entities.at(water_i - 1));
                }
                if (water_i + 1 < m_water_entities.size() and m_water_entities.at(water_i + 1)) {
                    m_water_entities.at(water_i)->add_input(m_water_entities.at(water_i + 1));
                }
                //console::log("water inputs: ", m_water_entities.at(water_i)->inputs.size(), "\n");
            }
        }
    }

    void Game::load_level(const std::filesystem::path& path) {
        console::log("Game::load_level()\n");

        std::map<U16, std::string> types_map = load_types_from_bin();

        std::vector<sprite::Data> sprite_data = sprite::load_level_data(path);

        Vec2I highest_grid_coord = { 0, 0 };
        std::vector<Vec2I> grid_coords{};

        U8 num_water_entity_per_tile = 4;

        Vec2F offset_between_bridges = { 8.0F, 0.0F };

        for (auto& i : sprite_data) {
            cVec2F tile_offset = { (F32)i.x, (F32)i.y };
            if (tile_offset.x + 16.0F > m_num_level_tiles.x) m_num_level_tiles.x = tile_offset.x + 16.0F;
            if (tile_offset.y + 16.0F > m_num_level_tiles.y) m_num_level_tiles.y = tile_offset.y + 16.0F;
            
            m_visible_layers.insert(i.layer);

            if (i.tile_set != 255) {
                cI32 sprite = sprite::make(tile_set_texture_path(i.tile_set));

                m_level_sprites.emplace_back(sprite);

                //sprite::id(sprite, sprite);
                sprite::tile_set(sprite, i.tile_set);
                sprite::layer(sprite, i.layer);
                sprite::offset(sprite, tile_offset);
                sprite::rect(sprite, Rect<I32>{ i.source_x, i.source_y, 16, 16 });
                sprite::transform(sprite, m_level_transform);
            }
            else if (i.tile_set == 255) {
                cU16 tile_x = i.source_x / 16;
                cU16 tile_y = i.source_y / 16;

                cU16 tile_number = tile_x + tile_y * 32;

                U16 entity_number = 0;

                if (types_map.count(tile_number)) {
                    std::string type_str = types_map.at(tile_number);

                    entity::Type entity_type = entity::string_to_type(type_str);

                    if (type_str.substr(0, 4) == "coin") {
                        entity_type = entity::Type::coin;
                    }

                    if (start_info().type == start::Type::center and entity_type == entity::Type::level_center) {
                        //console::log("\n\nsheet::Game::load_level() start position: ", tile_offset.x, " ", tile_offset.y, "\n\n");
                        m_start_position = tile_offset; //+ Vec2F{ 0.0F, -4.0F };
                        m_player.start_layer(i.layer);
                        sprite::layer(m_player.sprite(), i.layer);
                    }
                    else if (start_info().type == start::Type::L_0 and entity_type == entity::Type::level_L_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.start_layer(i.layer);
                        sprite::layer(m_player.sprite(), i.layer);
                        sprite::is_leftward(m_player.sprite(), false);
                    }
                    else if (start_info().type == start::Type::R_0 and entity_type == entity::Type::level_R_0) {
                        if (tile_offset.y > m_start_position.y) {
                            m_start_position = tile_offset;
                        }
                        m_player.start_layer(i.layer);
                        sprite::layer(m_player.sprite(), i.layer);
                        sprite::is_leftward(m_player.sprite(), true);
                    }

                    if (entity_type == entity::Type::none) continue;

                    std::filesystem::path config_path = "res/entity/" + types_map.at(tile_number) + ".cfg";
                    //console::log("entity config path: ", config_path, "\n");

                    U8    num_created   = 1;
                    Vec2F added_offset  = { 0.0F, 0.0F },
                          add_to_offset = { 0.0F, 0.0F };

                    bool is_random_leftward = false;


                    switch (entity_type) {
                        case entity::Type::bee: {
                            m_entities.emplace_back(new entity::Bee);
                            break;
                        }
                        case entity::Type::bee_hive: {
                            m_entities.emplace_back(new entity::BeeHive);
                            break;
                        }
                        case entity::Type::brick: {
                            m_entities.emplace_back(new entity::Brick);
                            break;
                        }
                        case entity::Type::bridge: {
                            //console::log("sheet::Game::load_level() found bridge: ", (int)i.layer, " ", tile_offset.x, " ", tile_offset.y, "\n\n");
                            //num_created = 2;
                            //add_to_offset.x = 8.0F;

                            offset_between_bridges = { 16.0F, 0.0F };

                            m_entities.emplace_back(new entity::Bridge);
                            m_bridge_entities.emplace_back(m_entities.back());


                            //m_entities.emplace_back(new entity::Bridge);
                            //m_bridge_entities.emplace_back(m_entities.back());
                            break;
                        }
                        case entity::Type::bug: m_entities.emplace_back(new entity::Bug); break;
                        case entity::Type::frog: {
                            m_entities.emplace_back(new entity::Frog);
                            is_random_leftward = true;
                            break;
                        }
                        case entity::Type::grass_0:
                        case entity::Type::grass_1:
                        case entity::Type::grass_2:
                        case entity::Type::grass_0_L_1x1:
                        case entity::Type::grass_1_L_1x1:
                        case entity::Type::grass_2_L_1x1:
                        case entity::Type::grass_0_L_2x1_0:
                        case entity::Type::grass_1_L_2x1_0:
                        case entity::Type::grass_0_L_2x1_1:
                        case entity::Type::grass_1_L_2x1_1:
                        case entity::Type::grass_2_L_2x1_0:
                        case entity::Type::grass_2_L_2x1_1:
                        case entity::Type::grass_0_R_1x1:
                        case entity::Type::grass_1_R_1x1:
                        case entity::Type::grass_2_R_1x1:
                        case entity::Type::grass_0_R_2x1_0:
                        case entity::Type::grass_1_R_2x1_0:
                        case entity::Type::grass_0_R_2x1_1:
                        case entity::Type::grass_1_R_2x1_1:
                        case entity::Type::grass_2_R_2x1_0:
                        case entity::Type::grass_2_R_2x1_1: {
                            num_created = 6;
                            is_random_leftward = true;

                            if (entity::is_grass_2(entity_type)) {
                                config_path = "res/entity/grass_2.cfg";
                            } else if (entity::is_grass_1(entity_type)) {
                                num_created = 5;
                                config_path = "res/entity/grass_1.cfg";
                            } else {
                                num_created = 3;
                                config_path = "res/entity/grass_0.cfg";
                            }
                            add_to_offset.x = 16.0F / num_created;

                            switch (entity_type) {
                                case entity::Type::grass_0_L_1x1:
                                case entity::Type::grass_1_L_1x1:
                                case entity::Type::grass_2_L_1x1: {
                                    added_offset.y = 15.0F;
                                    add_to_offset.y = -add_to_offset.x;
                                    break;
                                }
                                case entity::Type::grass_0_R_1x1:
                                case entity::Type::grass_1_R_1x1:
                                case entity::Type::grass_2_R_1x1: {
                                    added_offset.y = 2.0F;
                                    add_to_offset.y = add_to_offset.x;
                                    break;
                                }
                                case entity::Type::grass_0_L_2x1_0:
                                case entity::Type::grass_1_L_2x1_0:
                                case entity::Type::grass_2_L_2x1_0: {
                                    added_offset.y = 15.0F;
                                    add_to_offset.y = -add_to_offset.x / 2.0F;
                                    break;
                                }
                                case entity::Type::grass_0_L_2x1_1:
                                case entity::Type::grass_1_L_2x1_1:
                                case entity::Type::grass_2_L_2x1_1: {
                                    added_offset.y = 7.0F;
                                    add_to_offset.y = -add_to_offset.x / 2.0F;
                                    break;
                                }
                                case entity::Type::grass_0_R_2x1_0:
                                case entity::Type::grass_1_R_2x1_0:
                                case entity::Type::grass_2_R_2x1_0: {
                                    added_offset.y = 2.0F;
                                    add_to_offset.y = add_to_offset.x / 2.0F;
                                    break;
                                }
                                case entity::Type::grass_0_R_2x1_1:
                                case entity::Type::grass_1_R_2x1_1:
                                case entity::Type::grass_2_R_2x1_1: {
                                    added_offset.y = 10.0F;
                                    add_to_offset.y = add_to_offset.x / 2.0F;
                                    break;
                                }
                            }
                            for (U8 i_create = 0; i_create < num_created; ++i_create) {
                                //m_entities.emplace_back(std::make_unique<entity::Grass>());
                                m_entities.emplace_back(new entity::Grass);
                                //m_entities.back()->number(i_create);
                                m_entities.back()->transform(m_level_transform);
                            }
                            break;
                        }
                        case entity::Type::conduit_corner_DL:
                        case entity::Type::conduit_corner_DR:
                        case entity::Type::conduit_corner_UL:
                        case entity::Type::conduit_corner_UR:
                        case entity::Type::conduit_LR:
                        case entity::Type::conduit_UD:
                        case entity::Type::conduit_L_1x1_0:
                        case entity::Type::conduit_L_1x1_1:
                        case entity::Type::conduit_L_1x2_0:
                        case entity::Type::conduit_L_1x2_1:
                        case entity::Type::conduit_L_2x1_0:
                        case entity::Type::conduit_L_2x1_1:
                        case entity::Type::conduit_R_1x1_0:
                        case entity::Type::conduit_R_1x1_1:
                        case entity::Type::conduit_R_1x2_0:
                        case entity::Type::conduit_R_1x2_1:
                        case entity::Type::conduit_R_2x1_0:
                        case entity::Type::conduit_R_2x1_1: {
                            m_entities.emplace_back(new entity::Conduit);
                            m_entities.back()->transform(m_level_transform);
                            m_unlocked_entities.emplace_back(m_entities.back());

                            //config_path = "res/entity/conduit.cfg";
                            break;
                        }
                        case entity::Type::track_trigger_UL:
                        case entity::Type::track_trigger_UR:
                        case entity::Type::track_UD:
                        case entity::Type::track_LR:
                        case entity::Type::track_corner_UL:
                        case entity::Type::track_corner_UR:
                        case entity::Type::track_corner_DL:
                        case entity::Type::track_corner_DR:
                        case entity::Type::track_L_1x1_0:
                        case entity::Type::track_L_1x1_1:
                        case entity::Type::track_R_1x1_0:
                        case entity::Type::track_R_1x1_1:
                        case entity::Type::track_L_1x2_0:
                        case entity::Type::track_L_1x2_1:
                        case entity::Type::track_R_1x2_0:
                        case entity::Type::track_R_1x2_1:
                        case entity::Type::track_L_2x1_0:
                        case entity::Type::track_L_2x1_1:
                        case entity::Type::track_R_2x1_0:
                        case entity::Type::track_R_2x1_1: {
                            //console::log("Game::load_level() found track ", entity::to_string(entity_type), "\n");
                            m_entities.emplace_back(new entity::Track);
                            m_entities.back()->transform(m_level_transform);
                            m_unlocked_entities.emplace_back(m_entities.back());
                            break;
                        }
                        case entity::Type::level_center: {
                            m_entities.emplace_back(new entity::Flag);
                            m_entities.back()->transform(m_level_transform);
                            break;
                        }
                        case entity::Type::logic_and_LR:
                        case entity::Type::logic_and_UD: {
                            m_entities.emplace_back(new entity::LogicAnd);
                            m_entities.back()->transform(m_level_transform);
                            m_unlocked_entities.emplace_back(m_entities.back());
                            break;
                        }                        
                        case entity::Type::logic_not_LR:
                        case entity::Type::logic_not_UD: {
                            m_entities.emplace_back(new entity::LogicNot);
                            m_entities.back()->transform(m_level_transform);
                            m_unlocked_entities.emplace_back(m_entities.back());
                            break;
                        }
                        case entity::Type::logic_or_LR:
                        case entity::Type::logic_or_UD: {
                            m_entities.emplace_back(new entity::LogicOr);
                            m_entities.back()->transform(m_level_transform);
                            m_unlocked_entities.emplace_back(m_entities.back());
                            break;
                        }
                        case entity::Type::mole: {
                            m_entities.emplace_back(new entity::Mole);
                            break;
                        }
                        case entity::Type::spring_U: {
                            m_entities.emplace_back(new entity::Spring);
                            m_entities.back()->transform(m_level_transform);
                            break;
                        }
                        case entity::Type::train_platform:
                        case entity::Type::train_saw: {
                            m_entities.emplace_back(new entity::Train);
                            //m_entities.back()->position() = tile_offset;
                            break;
                        }
                        case entity::Type::trigger: {
                            //cU8 entity_number = entity::number_in_type(entity_type);
                            //console::log("trigger number: ", (int)entity_number, "\n");
                            //console::log("trigger_", (int)entity_number, " = ", LevelConfig::trigger_path(entity_number), "\n");

                            //m_entities.emplace_back(std::make_unique<entity::Lever>());
                            m_entities.emplace_back(new entity::Lever);
                            m_entities.back()->transform(m_level_transform);

                            config_path = "res/entity/trigger.cfg";
                            //entity_type = entity::Type::lever;

                            m_trigger_entities.emplace_back(m_entities.back());
                            break;
                        }

                        case entity::Type::water_line:
                        case entity::Type::water_line_L:
                        case entity::Type::water_line_R: {
                            num_created = num_water_entity_per_tile;
                            add_to_offset.x = 16.0F / num_water_entity_per_tile;

                            for (U8 i_create = 0; i_create < num_created; ++i_create) {
                                m_entities.emplace_back(new entity::WaterLine);
                                //m_entities.back()->number(i_create);
                                m_water_entities.emplace_back(m_entities.back());

                                //m_water_entities.emplace_back(std::make_unique<entity::WaterLine>());
                                //m_water_entities.back()->number(i_create);

                                //m_unlocked_entities.emplace_back(m_entities.back());
                            }


                            //console::log("water entity objects size: ", m_water_entities.size(), "\n");
                            config_path = "res/entity/water_line.cfg";
                            break;
                        }
                        default: {
                            bool is_to_create = true;
                            if (entity_type == entity::Type::coin) {
                                entity_number = entity::number_in_type(type_str);
                                config_path = "res/entity/coin.cfg";
                                if (sheet::game::is_coin_picked(entity_number)) {
                                    num_created = 0;
                                }
                                //console::log("sheet::Game::load_level() coin ", entity_number, " created: ", (int)num_created, "\n\n");
                            }
                            if (num_created != 0) {
                                //m_entities.emplace_back(std::make_unique<entity::Object>());
                                m_entities.emplace_back(new entity::Object);
                                m_entities.back()->transform(m_level_transform);
                            }
                            break;
                        }
                    }
                        
                    for (U8 i_created = 0; i_created < num_created; ++i_created) {
                        const size_t entity_i = m_entities.size() - num_created + i_created;

                        if (!m_entities.at(entity_i)) continue;

                        // only keep L and R type at the very edge
                        entity::Type changed_entity_type = entity_type;
                        if (entity_type == entity::Type::water_line_L and i_created > 0 or
                            entity_type == entity::Type::water_line_R and i_created < num_created - 1) {
                            changed_entity_type = entity::Type::water_line;
                        }

                        m_entities.at(entity_i)->type(changed_entity_type);
                        m_entities.at(entity_i)->number(entity_number);
                                                        
                        m_entities.at(entity_i)->start_layer(i.layer);

                        if (transform::is_level(m_entities.at(entity_i)->transform())) {
                            m_entities.at(entity_i)->start_offset({ tile_offset.x + added_offset.x,
                                                                          tile_offset.y + added_offset.y });
                            //console::log("start offset: ", m_entities.at(entity_i)->start_offset().x, " ", m_entities.at(entity_i)->start_offset().y, "\n");
                        }

                        m_entities.at(entity_i)->load_config(config_path);

                        if (entity_type == entity::Type::level_center) {
                            m_entities.at(entity_i)->start_layer(i.layer - 1);
                            m_entities.at(entity_i)->sprite_layer(i.layer - 1);
                        }

                        if (is_random_leftward) {
                            sprite::is_leftward(m_entities.at(entity_i)->sprite(), random::number(0, 1));
                        }

                        if (!transform::is_level(m_entities.at(entity_i)->transform())) {
                            m_entities.at(entity_i)->position({ tile_offset.x + added_offset.x,
                                                                tile_offset.y + added_offset.y });
                            m_entities.at(entity_i)->start_position(m_entities.at(entity_i)->position());
                            //camera::add_transform(m_entities.back()->transform()->id);

                            /*if (entity::is_water(entity_type)) {
                                console::log("water start_position.y: ", m_entities.at(entity_i)->transform()->start_position.y, "\n");
                            }*/
                        }

                        added_offset += add_to_offset;
                    }
                    
                }
            }
            cVec2I grid_coord = { (i.x - i.x % 256 + 256) / 256 - 0, (i.y - i.y % 256 + 256) / 256 - 0 };
            //console::log(class_name(), "::load_level() collision grid coord: ", collision_grid_coord.x, " ", collision_grid_coord.y, "\n");
            if (std::find(grid_coords.begin(), grid_coords.end(), grid_coord) == grid_coords.end()) {
                grid_coords.emplace_back(grid_coord);
                //m_level_quad_trees.emplace_back(std::make_pair(grid_coord, new QuadTreeNode({ 0, 0, 256, 256 })));
            }
            if (grid_coord.x > highest_grid_coord.x) {
                highest_grid_coord.x = grid_coord.x;
            }
            if (grid_coord.y > highest_grid_coord.y) {
                highest_grid_coord.y = grid_coord.y;
            }
            if (std::find(m_grid_coords.begin(), m_grid_coords.end(), grid_coord) == m_grid_coords.end()) {
                m_grid_coords.push_back(grid_coord);
                console::log("m_grid_coords: ", m_grid_coords.size(), "\n");
            }            
        }

        m_visible_layers.insert(m_player.start_layer() - 1);
        m_visible_layers.insert(m_player.start_layer() + 1);

        console::log(class_name(), "::load_level() highest quad tree node: ", highest_grid_coord.x, " ", highest_grid_coord.y, "\n");

        camera::level_scroll_limit = highest_grid_coord * 256 - Vec2I{ m_window_w, m_window_h };

        console::warning(class_name(), "::Game::load_level() window extent: ", m_window_w, " ", m_window_h, "\n");

        console::log(class_name(), "::load_level() camera::level_scroll_limit: ", camera::level_scroll_limit.x, " ", camera::level_scroll_limit.y, "\n");

        //console::log("m_water_entities size: ", m_water_entities.size(), "\n");

        //console::log("num bridge entities: ", m_bridge_entities.size(), "\n\n\n");

        
        /*F32 closest_bridge_dist_L = 100000.0F;
        I32 closest_bridge_L = -1;
        for (I16 bridge_i = 0; bridge_i < m_bridge_entities.size(); ++bridge_i) {
            for (auto& l : bridges_L) {
                if (std::abs(m_bridge_entities.at(bridge_i)->start_position().x - m_bridge_entities.at(l)->start_position().x) < closest_bridge_dist_L) {
                    closest_bridge_dist_L = std::abs(m_bridge_entities.at(bridge_i)->start_position().x - m_bridge_entities.at(l)->start_position().x);
                    closest_bridge_L = bridge_i;
                    console::log("closest bridge dist L: ", closest_bridge_dist_L, "\n");
                }
            }
            if (closest_bridge_L != -1) {
                m_bridge_entities.at(bridge_i)->add_child(m_bridge_entities.at(closest_bridge_L));
            }            
        }

        F32 closest_bridge_dist_R = 100000.0F;
        I32 closest_bridge_R = -1;
        for (I16 bridge_i = m_bridge_entities.size() - 1; bridge_i >= 0; --bridge_i) {
            for (auto& r : bridges_R) {
                if (std::abs(m_bridge_entities.at(bridge_i)->start_position().x - m_bridge_entities.at(r)->start_position().x) < closest_bridge_dist_R) {
                    closest_bridge_dist_R = std::abs(m_bridge_entities.at(bridge_i)->start_position().x - m_bridge_entities.at(r)->start_position().x);
                    closest_bridge_R = bridge_i;

                    console::log("closest bridge dist R: ", closest_bridge_dist_R, "\n");
                }
            }
            if (closest_bridge_R != -1) {
                m_bridge_entities.at(bridge_i)->add_child(m_bridge_entities.at(closest_bridge_R));
            }
        }*/


        connect_water();
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(m_water_entities.begin(), m_water_entities.end(), gen);

        //for (auto& entity : m_entities)   check_entities_to_add_input_from(entity);

        for (auto& trigger_entity : m_trigger_entities) {
            check_to_add_input_from(trigger_entity);
        }

        connect_bridges(offset_between_bridges);


        /*for (auto& entity : m_unlocked_entities) {
            if (!entity) continue;
            console::log(to_string(entity->type()), " direction: ", entity->direction().x, " ", entity->direction().y, "\n");
        }*/
        //console::log("\n\nsheet::Game::load_level() start position: ", m_start_position.x, " ", m_start_position.y, " layer: ", (I32)m_player.start_layer(), "\n\n");


        console::log(class_name(), "::load_level() farthest tile: ", m_num_level_tiles.x, " ", m_num_level_tiles.y, "\n");
    }
}