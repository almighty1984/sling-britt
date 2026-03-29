module sheet.game;
import health;
import line;

namespace sheet {
    void Game::draw(std::unique_ptr<Window>& window, cU8 layer) {
        //for (auto& i : sprite::ids_in_layer(layer)) {
        //    sprite::draw(window, i);
        //}
        ////health::draw(window, layer);
        //
        //for (auto& i : line::ids_in_layer(layer)) {
        //    if (!line::is_hidden(i) and !line::is_aabb(i) or m_is_drawing_debug) {
        //        line::draw(window, i);
        //    }
        //}
        //return;

        if (m_is_drawing_quad_tree and layer == NUM_VISIBLE_LAYERS - 1) {
            for (auto& i : m_level_quad_trees) {
                i.second->draw(window);
            }
        }
        
        for (auto& i : m_level_sprite_ids) {
            if (sprite::layer(i) == layer) {
                sprite::draw(window, i);
            }
        }
        if (sprite::layer(m_player.sprite_id()) == layer) {            
            m_player.draw(window);
            if (m_is_drawing_debug) {
                m_player.draw_aabb(window);
            }
        }
        for (auto& i : m_entity_objects) {
            if (i and sprite::layer(i->sprite_id()) == layer) {
                if (sprite::is_debug(i->sprite_id())) {
                    if (m_is_drawing_debug) {
                        i->draw(window);                        
                    }
                }
                else {
                    i->draw(window);
                }

                if (m_is_drawing_debug) {
                    i->draw_aabb(window);
                }
            }
        }
        for (auto& i : m_water_entity_objects) {
            if (i and i->start_layer() == layer) {                
                i->draw(window);
                if (m_is_drawing_debug) {
                    i->draw_aabb(window);
                }
            }
        }        
        
        particle::draw(window, layer);
        if (m_is_drawing_debug) {
            particle::draw_aabb(window, layer);
        }

        health::draw(window, layer);    
    }
}