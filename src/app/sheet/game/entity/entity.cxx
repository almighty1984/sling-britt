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
import transform;

static inline std::map<std::filesystem::path, entity::Config> s_parsed_configs;

namespace entity {
    I32 Object::aabb(aabb::cName name) {
        for (auto& i : m_aabbs) {
            if (aabb::name(i) == name) {
                return i;
            }
        }
        return -1;
    }
    //void Object::draw(std::unique_ptr<Window>& window) {
    //    sprite::draw(window, m_sprite);
    //}
    //void Object::draw_aabb(std::unique_ptr<Window>& window) {        
    //    for (auto& i : m_aabbs) {
    //        aabb::draw(window, i);
    //    }
    //}
    Object::~Object() {
        //console::log(class_name(), "::~Object() ", entity::to_string(m_type), "\n");
        camera::remove_transform(m_transform);
        for (auto& i : m_aabbs) {
            aabb::erase(i);
        }
        m_aabbs.clear();
        m_inputs.clear();

        for (const auto& [key, anim] : m_anims) {
            anim::erase(anim);
        }
        for (const auto& [key, sound_id] : m_sounds) {
            sound::erase(sound_id);
        }

        health::erase(m_health_id);
        sprite::erase(m_sprite);

        //if (m_start_offset == Vec2F{ 0.0F, 0.0F}) {
        if (!transform::is_level(m_transform)) {
            transform::erase(m_transform);
        }
        //}
    }
    bool Object::load_config(const std::filesystem::path& path) {
        std::ifstream in_file(path);
        if (!in_file) {
            console::error(class_name(), "::load_config(", path, ") not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();
        
        //if (s_parsed_configs.count(path)) {
        //    m_config = s_parsed_configs.at(path);
        //    //console::log(class_name(), "::load_config() ", to_string(m_type), " already loaded\n");
        //} else {
            m_config = parse_config(text);
            s_parsed_configs[path] = m_config;
        //}
        m_time_left_alive = m_config.time_to_be_alive();
        m_time_left_dead = m_config.time_to_be_dead();

        m_sounds = sound::parse_config(text);

        if (transform::parse_config(text, this)) {
            camera::add_transform(m_transform);
            auto health_config = health::parse_config(text, m_transform);
            m_health_id = health_config.id;
            health_max(health_config.amount);
            health_amount(health_config.amount);
            health_regen(health_config.regen);
            health_offset(health_config.offset);
        }
        m_sprite = sprite::parse_config(text, m_transform, m_start_layer, m_start_offset);

        if (m_sprite >= 0) {
            m_anims = anim::parse_config(text, m_sprite);
            if (!m_anims.empty()) {
                m_current_anim = m_anims["idle"];
            }
        }

        m_aabbs = aabb::parse_config(text, m_transform, start_offset());
        for (auto& aabb : m_aabbs) {
            aabb::owner(aabb, this);
        }

        return true;
    }
}