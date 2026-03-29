module;
#include <SFML/Audio.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <memory>

export module sound;
import console;
import types;

std::unordered_map<std::filesystem::path, std::unique_ptr<sf::SoundBuffer>> s_sound_buffers;

static sf::SoundBuffer& load_sound_buffer(const std::filesystem::path& path) {
    if (s_sound_buffers.find(path) == s_sound_buffers.end()) {
        s_sound_buffers[path] = std::make_unique<sf::SoundBuffer>(path);
    }
    return *s_sound_buffers[path];
}

class Sound {
    std::unique_ptr<sf::Sound> m_sound;
    std::filesystem::path m_path;
    Vec2F m_position;
public:
    I32 id = -1;
    //Sound() {}
    Sound(const std::filesystem::path& path) {
        load(path);
    }
    void load(const std::filesystem::path& path) {
        m_path = path;
        m_sound = std::make_unique<sf::Sound>(load_sound_buffer(path));        

        //m_sound->setLooping(false);
    }
    ~Sound() {
        m_sound.release();
    }
    Vec2F position() const { return m_position; }
    void position(cVec2F p) {
        if (!m_sound) return;
        m_position = p;
        m_sound->setPosition(sf::Vector3f{ p.x, p.y, 1.0F });
        
        //m_sound->setMinDistance(1.0F);
        //m_sound->setRelativeToListener(true);
    }
    F32 offset() const { return m_sound->getPlayingOffset().asSeconds(); }
    bool is_looped() const { return m_sound->isLooping(); } void is_looped(const bool b) { m_sound->setLooping(b); }

    void play()  { if (m_sound) m_sound->play();  } bool is_playing() const { return m_sound->getStatus() == sf::SoundSource::Status::Playing; }
    void stop()  { if (m_sound) m_sound->stop();  } bool is_stopped() const { return m_sound->getStatus() == sf::SoundSource::Status::Stopped; }
    void pause() { if (m_sound) m_sound->pause(); } bool is_paused()  const { return m_sound->getStatus() == sf::SoundSource::Status::Paused; }
    
    F32 pitch() const { return m_sound ? m_sound->getPitch() : 0.0F; } void pitch(cF32 p) { if (m_sound) m_sound->setPitch(p); }
};

std::vector<Sound*> s_sounds;
std::vector<I32>    s_unused_ids;

export namespace sound {
    bool     is_valid(size_t i) { return (i >= 0 and i < s_sounds.size() and s_sounds.at(i)) ? true : false; }
    size_t   size() { return s_sounds.size(); }

    I32 make(const std::filesystem::path& path) {
        //s_sounds.emplace_back(std::make_unique<Sound>(path));
        //return s_sounds.size() - 1;

        Sound* object = new Sound(path);
        if (!s_unused_ids.empty()) {
            object->id = s_unused_ids.back();
            s_unused_ids.pop_back();
            if (!s_sounds.empty() and object->id >= 0 and object->id < s_sounds.size() and s_sounds.at(object->id)) {
                delete s_sounds.at(object->id);
                s_sounds.at(object->id) = nullptr;
            }
        } else {
            object->id = s_sounds.size();
            s_sounds.emplace_back(nullptr);
        }
        s_sounds.at(object->id) = object;
        return object->id;
    }
    void play(cI32 i)               { if (is_valid(i)) s_sounds.at(i)->play();  }
    void stop(cI32 i)               { if (is_valid(i)) s_sounds.at(i)->stop();  }
    void pause(cI32 i)              { if (is_valid(i)) s_sounds.at(i)->pause(); }
    bool is_looped(cI32 i)          { return is_valid(i) and s_sounds.at(i)->is_looped(); }
    void is_looped(cI32 i, bool is) { if (is_valid(i)) s_sounds.at(i)->is_looped(is); }
    bool is_playing(cI32 i)         { return is_valid(i) and s_sounds.at(i)->is_playing(); }
    bool is_stopped(cI32 i)         { return is_valid(i) and s_sounds.at(i)->is_stopped(); }
    bool is_paused(cI32 i)          { return is_valid(i) and s_sounds.at(i)->is_paused();  }
    F32 pitch(cI32 i)               { return is_valid(i) ? s_sounds.at(i)->pitch() : 0.0F; }    
    void pitch(cI32 i, cF32 v)      { if (is_valid(i))  s_sounds.at(i)->pitch(v); }
    void position(cI32 i, cVec2F p) { if (is_valid(i)) s_sounds.at(i)->position(p); }
    bool erase(cI32 i) {
        if (!is_valid(i)) {
            console::log("sound::erase ", i, " can't do it!\n");
            return false;
        }
        //console::log("sound::erase ", i, "\n");
            
        if (s_sounds.at(i)->is_playing()) {
            s_sounds.at(i)->stop();
        }
        delete s_sounds.at(i);
        s_sounds.at(i) = nullptr;
        s_unused_ids.emplace_back(i);
        return true;
    }
    void clear() {
        for (I32 i = 0; i < s_sounds.size(); ++i) {            
            erase(i);
        }
        s_sounds.clear();
        s_unused_ids.clear();
    }
}