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

        bool m_is_in_view = false;
    public:        
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner or !other.owner) return;

            entity::cType other_type = other.owner->type();

            cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                                aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
            cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                                  aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

            cF32 our_w = our_points.w - our_points.x;
            cF32 our_mid_x = our_points.x + our_w / 2.0f;

            cF32 other_w = other_points.w - other_points.x;
            cF32 other_mid_x = other_points.x + other_w / 2.0f;

            if (other_type == entity::Type::player and
                (anim::type(other.owner->current_anim()) == anim::Type::run or anim::type(other.owner->current_anim()) == anim::Type::walk) and
                (sprite::rect(other.owner->sprite()).x == 64 or sprite::rect(other.owner->sprite()).x == 320)) {
                return;
            }

            if (other_type == entity::Type::bug  or
                other_type == entity::Type::frog or
                other_type == entity::Type::mole or
                other_type == entity::Type::player) {
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

                sprite::rect_x(m_sprite, tile_x);
                //m_time_left_dead = m_time_to_be_dead;
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        };

        void update() override {
            if (m_is_first_update) {
                m_is_first_update = false;
                m_random_time_offset = random::number(0, m_time_limit / 4);
                //console::log("m_random_time_offset: ", (I16)m_random_time_offset, "\n");
                //console::log("grass origin: ", sprite()->origin.x, " ", sprite()->origin.y, "\n");
            }
            //console::log("pos: ", position().x, " ", m_start_offset.x, "\n");            
            if (camera::position.x          > m_start_offset.x or
                camera::position.x + 320.0f < m_start_offset.x
                ) {
                m_is_in_view = false;
                for (auto& i : m_aabbs) {
                    aabb::is_active(i, false);
                }
                //return;
            } else {
                m_is_in_view = true;
                for (auto& i : m_aabbs) {                    
                    aabb::is_active(i, true);                    
                }
            }
            
            ++m_time_in_state; 
            if (m_time_in_state > m_time_limit + m_random_time_offset) {
                m_time_in_state = 0;
                                
                if (m_is_in_view) {
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