export module entity.spring;
import anim;
import console;
import entity;
import aabb;
import sound;
import types;

export namespace entity {
    class Spring : public Object {
    public:
        Spring() {
            //load_config("res/entity/spring_U.cfg");
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            cType other_type = other.owner->type();

            if ((other_type == Type::brick || other_type == Type::bug) && !other.owner->parent() ||
                other_type == Type::frog  ||
                other_type == Type::player) {
                //if (other.owner->parent()) return;
                
                reset_anim("dead");
                m_time_left_dead = m_time_to_be_dead;
                
                sound_position("dead", { (position().x + 8.0F) / WINDOW_W / 2.0F, (position().y + 8.0F) / WINDOW_H / 2.0F });
                sound_play("dead");
            }

        }
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        }
    };
}