export module entity.flag;
import anim;
import console;
import entity;

export namespace entity {
    class Flag : public Object {
    public:
        Flag() {
        }
        void collide_x(aabb::cInfo our, aabb::cInfo other) override {
            if (!our.owner or !other.owner) return;
            entity::cType other_type = other.owner->type();
            switch (other_type) {
                case Type::player: {
                    m_time_left_dead = m_time_to_be_dead;
                    break;
                }
            }
        };
        void collide_y(aabb::cInfo our, aabb::cInfo other) override {
            collide_x(our, other);
        };        
    };
}