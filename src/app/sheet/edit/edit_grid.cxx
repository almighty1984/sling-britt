module sheet.edit;
import console;
import transform;
import sprite;
import types;

namespace sheet {
    cI32 Edit::grid_sprite_at_offset(cVec2F offset) {
        if (m_grid_sprites.empty()) return -1;

        cF32 x = offset.x - std::fmodf(offset.x, 256.0F);
        cF32 y = offset.y - std::fmodf(offset.y, 256.0F);

        for (auto& i : m_grid_sprites) {
            if (sprite::offset(i) == Vec2F{ x, y }) {
                return i;
            }
        }
        return -1;
    }
    cI32 Edit::grid_map_sprite_at_offset(cVec2F offset) {
        if (m_grid_map_sprites.empty()) return -1;

        cF32 x = offset.x - std::fmodf(offset.x, 16.0F);
        cF32 y = offset.y - std::fmodf(offset.y, 16.0F);

        for (auto& i : m_grid_map_sprites) {
            if (sprite::offset(i) == Vec2F{ x, y }) {
                return i;
            }
        }
        return -1;
    }
    bool Edit::add_grid_at_offset(cVec2F offset) {
        for (auto& i : m_grid_sprites) {
            if (sprite::offset(i) == offset) {
                //console::log("sheet::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " already added\n");
                return false;
            }
        }
        cI32 grid_sprite = sprite::make(m_grid_texture_path);
        m_grid_sprites.emplace_back(grid_sprite);
        //sprite::id(grid_sprite, grid_sprite);
        sprite::rect(grid_sprite, { 0, 0, 256, 256 });
        sprite::offset(grid_sprite, offset);
        sprite::transform(grid_sprite, m_grid_transform);
        sprite::layer(grid_sprite, GRID_LAYER);
        sprite::is_hidden(grid_sprite, m_is_hidden_grid);
        
        cI32 grid_map_sprite = sprite::make(m_grid_map_texture_path);
        m_grid_map_sprites.emplace_back(grid_map_sprite);
        //sprite::id(grid_map_sprite, grid_map_sprite);
        sprite::rect(grid_map_sprite, { 0, 0, 16, 16 });
        sprite::offset(grid_map_sprite, offset / 16.0F);
        sprite::transform(grid_map_sprite, m_grid_map_transform);
        sprite::layer(grid_map_sprite, GRID_MAP_LAYER);
        sprite::is_hidden(grid_map_sprite, m_is_hidden_grid_map);

        //console::log("sheet::Edit::add_grid_at_offset ", offset.x, " ", offset.y, " sprite id: ", grid_sprite, " map sprite id: ", grid_map_sprite, "\n");
        return true;
    }
    bool Edit::erase_grid_at_offset(cVec2F offset) {
        if (m_grid_sprites.empty() or m_grid_sprites.size() < 2) {
            //console::log("sheet::Edit::erase_grid_at_offset grid size is 1");
            return false;
        }
        cI32 grid_sprite = Edit::grid_sprite_at_offset(offset);
        if (grid_sprite == -1) return false;

        //console::log("sheet::Edit::erase_grid_at_offset sprite: ", grid_sprite, "\n");

        bool is_found = false;
        std::vector<I32> resized_grid_sprites;
        for (auto& i : m_grid_sprites) {
            if (i == grid_sprite) {
                is_found = true;
            } else {
                resized_grid_sprites.emplace_back(i);
            }
        }
        if (!is_found) {
            //console::log("sheet::Edit::erase_grid_at_offset grid_sprite not found ", grid_sprite, "\n");
            return false;
        }
        m_grid_sprites.clear();
        m_grid_sprites = resized_grid_sprites;
        //console::log("sheet::Edit::erase_grid_at_offset grid_sprites size: ", m_grid_sprites.size(), "\n");
        sprite::erase(grid_sprite);

        Vec2F grid_offset = offset / 16.0F;
        //console::log("sheet::Edit::erase_grid_at_offset grid_offset: ", grid_offset.x, " ", grid_offset.y, "\n");        

        cI32 grid_map_sprite = Edit::grid_map_sprite_at_offset(grid_offset);
        if (grid_map_sprite == -1) return false;
        if (!is_found) {
            console::log("sheet::Edit::erase_grid_at_offset grid_map_sprite not found ", grid_map_sprite, "\n");
            return false;
        }
        is_found = false;
        std::vector<I32> resized_grid_map_sprites;
        for (auto& i : m_grid_map_sprites) {
            if (i == grid_map_sprite) {
                is_found = true;
            } else {
                resized_grid_map_sprites.emplace_back(i);
            }
        }

        m_grid_map_sprites.clear();
        m_grid_map_sprites = resized_grid_map_sprites;
        //console::log("sheet::Edit::erase_grid_at grid_map_sprites size: ", m_grid_map_sprites.size(), "\n");
        sprite::erase(grid_map_sprite);

        return true;
    }
    bool Edit::clear_grid_sprites() {
        /*if (m_grid_transform != -1) {
            transform::erase(m_grid_transform);
            m_grid_transform = -1;
        }*/

        /*for (size_t i = 1; i < m_grid_sprites.size(); ++i) {
            sprite::erase(m_grid_sprites.at(i));
        }
        m_grid_sprites.resize(1);
        for (size_t i = 1; i < m_grid_map_sprites.size(); ++i) {
            sprite::erase(m_grid_sprites.at(i));
        }
        m_grid_map_sprites.resize(1);*/

        for (auto& i : m_grid_sprites) {            
            if (sprite::transform(i) == m_grid_transform) {
                sprite::erase(i);
            }
        }
        for (auto& i : m_grid_map_sprites) {            
            if (sprite::transform(i) == m_grid_map_transform) {
                sprite::erase(i);
            }
        }
        m_grid_sprites.clear();
        m_grid_map_sprites.clear();
        return true;
    }
}