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

namespace entity {
    void Object::draw(std::unique_ptr<Window>& window) {
        sprite::draw(window, m_sprite);
    }
    void Object::draw_aabb(std::unique_ptr<Window>& window) {        
        for (auto& i : m_aabbs) {
            aabb::draw(window, i);
        }
    }
    Object::~Object() {
        //console::log(class_name(), "::~Object() ", entity::to_string(m_type), "\n");
        camera::remove_transform(m_transform);
        for (auto& i : m_aabbs) {
            aabb::erase(i);
        }
        m_aabbs.clear();
        m_input_objects.clear();

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
            console::error("config::load ", path, " not found\n");
            return false;
        }
        std::ostringstream oss{};
        oss << in_file.rdbuf();

        const std::string text = oss.str();

        entity::parse_config(text, this);
        sound::parse_config(text, this);

        if (transform::parse_config(text, this)) {
            camera::add_transform(m_transform);
            health::parse_config(text, this);
        }
        if (sprite::parse_config(text, this, m_transform, m_start_layer, m_start_offset)) {
            anim::parse_config(text, this);
        }

        aabb::parse_config(text, this);

        return true;
    }
}