export module entity.grass;
import anim;
import camera;
import aabb;
import console;
import entity;
import random;
import sprite;

export namespace entity {
    class Grass : public Object {
        I32 m_prev_source_x     = 0;

        U8 m_time_limit         = 8,
           m_random_time_offset = 0;

    public:        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner or !other.owner) return;

            cType other_type = other.owner->type();

            cVec2F our_UL = aabb::UL(our.id);
            cVec2F our_DR = aabb::DR(our.id);
            cVec2F other_UL = aabb::UL(other.id);
            cVec2F other_DR = aabb::DR(other.id);

            cVec2F our_extent = { our_DR.x - our_UL.x, our_DR.y - our_UL.y };
            cF32 our_mid_x = our_UL.x + our_extent.x / 2.0f;

            cVec2F other_extent = { other_DR.x - other_UL.x, other_DR.y - other_UL.y };
            cF32 other_mid_x = other_UL.x + other_extent.x / 2.0f;

            if (other_type == entity::Type::player and
                (anim::type(other.owner->current_anim()) == anim::Type::run or anim::type(other.owner->current_anim()) == anim::Type::walk) and
                (sprite::rect(other.owner->sprite()).x == 64 or sprite::rect(other.owner->sprite()).x == 320)) {
                return;
            }

            switch (other_type) {
                case Type::bug:
                case Type::frog:
                case Type::mole:
                case Type::player: {
                    //console::log("source x: ", other.owner->sprite()->rect.x, "\n");
                    //console::log("anim type: ", anim::to_string(other.owner->anim()->type), "\n");                

                    int num = 0;
                    if (our_mid_x > other_mid_x) {
                        //sprite::is_leftward(m_sprite, false);
                        num = (int)std::abs(our_mid_x - other_mid_x);
                    } else {
                        //sprite::is_leftward(m_sprite, true);
                        num = (int)std::abs(our_mid_x - other_mid_x);
                    }
                    int tile_x = 32 * 7 - ((num / 4) * 32);
                    tile_x -= tile_x % 32;

                    sprite_rect_x(tile_x);
                    //m_time_left_dead = m_time_to_be_dead;
                    break;
                }
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        };

        void update(cF32 dt) override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_random_time_offset = random::number(0, m_time_limit / 4);
                //console::log("m_random_time_offset: ", (I16)m_random_time_offset, "\n");
                //console::log("grass origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");
            }
            //console::log("pos: ", position().x, " ", m_start_offset.x, "\n");            
            if (!is_start_offset_in_view()) {
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }
            } else {
                for (auto& i : m_aabbs) {                    
                    aabb::is_active(i, true);                    
                }
            }
            
            ++m_time_in_state; 
            if (m_time_in_state > m_time_limit + m_random_time_offset) {
                m_time_in_state = 0;
                                
                if (is_start_offset_in_view()) {
                    m_is_first_update = true;
                }

                if (sprite::rect(m_sprite).x == 0) {
                    if (m_prev_source_x == 32) {
                        sprite::is_leftward(m_sprite, !sprite::is_leftward(m_sprite));
                    }
                    sprite::rect_x(m_sprite, 32);
                    m_prev_source_x = 0;
                } else if (sprite::rect(m_sprite).x == 32) {
                    m_prev_source_x == 0 ? sprite::rect_x(m_sprite, 64) : sprite::rect_x(m_sprite, 0);
                    m_prev_source_x = 32;
                } else if (sprite::rect(m_sprite).x == 64) {
                    m_prev_source_x == 32 ? sprite::rect_x(m_sprite, 96) : sprite::rect_x(m_sprite, 32);
                    m_prev_source_x = 64;
                } else if (sprite::rect(m_sprite).x >= 96) {
                    sprite::rect_x(m_sprite, 64);
                    m_prev_source_x = 96;
                }
            }
        }        
    };
}