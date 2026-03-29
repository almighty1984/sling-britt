export module entity.lever;
import anim;
import console;
import entity;
import aabb;
import sprite;

export namespace entity {
    class Lever : public Object {

        void flip_it() {
            if (m_time_left_dead == 0) {
                m_time_left_dead = U16_MAX;
                anim::source_x(anim_id("dead"), 0);
                anim::speed(anim_id("dead"), 0.5F);
            } else {
                m_time_left_dead = 0;
                anim::source_x(anim_id("idle"), 0);
                anim::speed(anim_id("idle"), 0.5F);
            }

            /*for (auto& i : m_input_objects) {                
                i->time_left_dead(m_time_left_dead);
            }*/
            
        }
    public:        
        Lever() {
            m_is_powered = true;
            m_time_left_dead = U16_MAX;
            m_time_left_alive = 0;
            //m_time_to_collide = 20;
            //load_config("res/entity/lever.cfg");
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner or !other.owner or !other.owner->parent()) return;
            //console::log("Lever collide_x ", (int)m_time_left_dead, "\n");
            cType other_type = other.owner->type();
            aabb::Name other_name = aabb::name(other.id);

            state::cType other_state = other.owner->state();

            
            if (other_type == entity::Type::particle_interact) {
                //console::log("entity::Lever::collide_x() other state: ", to_string(other_state), "\n");
                if (other_state == state::Type::run /*and !other.owner->parent()->is_carrying()*/) {
                    flip_it();
                }
            }
        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            //collide_x(our, other);
        }
        void update() override {
            
            /*if (m_time_left_colliding > 0) {
                console::log("m_time_left_colliding: ", (int)m_time_left_colliding, "\n");
                --m_time_left_colliding;
            }*/
            //console::log("num outputs: ", output_objects.size(), "\n");
            if (!m_input_objects.empty()) {
                //console::log("trigger ", m_start_offset.x, " ", m_start_offset.y, " has ", m_input_objects.size(), " inputs\n");

                /*if (inputs().back()->inputs.empty()) {
                    inputs().back()->add_input(std::shared_ptr<Object>(this));
                }*/
                
                //m_time_left_dead = inputs().back()->is_dead() ? -1 : 0;
                if (m_input_objects.back()->is_dead() and m_time_left_dead == 0 or
                    !m_input_objects.back()->is_dead() and m_time_left_dead > 0) {
                    flip_it();
                }
            }

            

            if (m_time_left_dead > 0) {
                m_time_left_alive = 0;
                //console::log("m_time_left_dead: ", (int)m_time_left_dead, "\n");
                //console::log("dead\n");
                if (m_time_left_dead > 0) {
                    if (m_time_left_dead != U16_MAX) {
                        --m_time_left_dead;
                    }
                }
                m_current_anim_id = anim_id("dead"); 
                //console::error("trigger dead\n");
            } else {
                //console::log("alive\n");
                m_time_left_alive = U16_MAX;
                m_current_anim_id = anim_id("idle");
                //console::log("trigger alive\n");
            }
            
            /*for (auto& i : output_objects) {
                i->time_left_alive(m_time_left_alive);
            }*/

            if (anim::is_last_frame(m_current_anim_id)) {
                anim::speed(m_current_anim_id, 0.0F);
            }
            //if (anim::source(m_current_anim_id).x ==
            //        (anim::source(m_current_anim_id).w * anim::texture_w(m_current_anim_id) / anim::source(m_current_anim_id).w)
            //        - anim::source(m_current_anim_id).w) {
            //    anim::speed(m_current_anim_id, 0.0F);
            //    // stop at last frame
            //}

           
            if (m_is_first_state_update) {
                m_is_first_state_update = false;
                anim::speed(m_current_anim_id, 0.0F);
                anim::source_x(m_current_anim_id,
                    (anim::source(m_current_anim_id).w *
                    anim::texture_size(m_current_anim_id).x / anim::source(m_current_anim_id).w)
                    - anim::source(m_current_anim_id).w);
                // start at last frame
            }
            sprite::source_rect(m_sprite_id, anim::source(m_current_anim_id));

        }
        
    };
}