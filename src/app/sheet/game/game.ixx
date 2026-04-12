export module sheet.game;
import sheet;
import bitmap_text;
import console;
import camera;
import aabb;
import entity;
import entity.particle;
import entity.player;
import input;
import line;
import particle_system;
import plane;
import quad_tree;
import sprite;
import transform;
import types;
import std;

export namespace sheet {
    class Game : public Object {
    private:
        U16 m_window_w = 0,
            m_window_h = 0;

        I32 m_level_transform = -1;

        U32 m_timer = 0;
        entity::Player m_player;
        //entity::Player m_player2;
        //entity::Player m_player3;

        Vec2F m_start_position = { 0.0F, 0.0F };
        //entity::Type m_level_start = entity::Type::level_start;

        std::vector<I32> m_level_sprites;

        std::vector<entity::Object*> m_entity_objects,
                                     m_bridge_entity_objects,
                                     m_trigger_entity_objects,
                                     m_water_entity_objects,
                                     m_unlocked_entity_objects;

        std::vector<std::pair<Vec2I, QuadTreeNode*>> m_level_quad_trees;

        std::filesystem::path m_level_path = "res/level/test.bin";
        
        Multiplane m_bg_planes;

        U8 m_time_left_moving_camera = 0;

    public:
        Game() = delete;
        Game(cU16 window_w, cU16 window_h, const std::filesystem::path level_path, start::Info start_info);
        ~Game() {
            clear();
        }        
        void clear() {
            console::log("sheet::Game::clear()\n");
            camera::remove_transform(m_level_transform);
            camera::remove_transform(m_player.transform());
            camera::difference = 0.0F;
            camera::clear();

            m_visible_layers.clear();
            //console::log("sheet::Game::clear() quad trees\n");
            for (auto& i : m_level_quad_trees) {
                if (i.second) delete i.second;
            }
            m_level_quad_trees.clear();
            
            //console::log("sheet::Game::clear() entities\n");
            for (auto& i : m_entity_objects) {
                if (i) delete i;
            }
            m_entity_objects.clear();
            //console::log("sheet::Game::clear() water entities\n");
            m_water_entity_objects.clear();
            //console::log("sheet::Game::clear() unlocked entities\n");
            m_unlocked_entity_objects.clear();
            //console::log("sheet::Game::clear() trigger entities\n");
            m_trigger_entity_objects.clear();

            //console::log("sheet::Game::clear() level sprites\n");
            for (auto& i : m_level_sprites) {
                //std::cout << "sheet::Game::clear() level sprite: " << i << "\n";
                sprite::erase(i);
            }
            m_level_sprites.clear();

            //console::log("sheet::Game::clear() aabb\n");
            aabb::clear();
            //console::log("sheet::Game::clear() particle\n");
            particle::clear();

            transform::erase(m_level_transform);
            input::erase(m_input);
        }
        void check_to_add_input_from(entity::Object* trigger_entity);

        void update(cF32 ts) override;
        void update_unlocked() override;
        void draw(std::unique_ptr<Window>& window, cU8 layer) override;

        std::filesystem::path level_path_to_save() override {
            return m_level_path;
        }

        std::map<U16, std::string> load_types_from_bin();

        //Vec2F level_velocity() const { return transform::velocity(m_level_transform); } void level_velocity(cVec2F v) { transform::velocity(m_level_transform, v); }                
        //Vec2F level_position() const { return transform::position(m_level_transform); } void level_position(cVec2F p) { transform::position(m_level_transform, p); }
        //void level_position_x(cF32 x) { transform::position_x(m_level_transform, x); }
        //void level_position_y(cF32 y) { transform::position_y(m_level_transform, y); }

        void load_level(const std::filesystem::path& path);

        void quad_trees_check_collision();
    };
}