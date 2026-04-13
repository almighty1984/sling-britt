export module aabb.trait;
//import aabb;
import types;
import std;

export namespace entity {
    class Object;
}
export namespace aabb {
    struct Info {
        I32             id = -1;
        entity::Object* owner = nullptr;
    };
    using cInfo = const Info;

    class Trait {
    protected:
        std::vector<I32> m_aabbs;

    public:
        std::vector<I32>& aabb_ids() { return m_aabbs; } void aabb_ids(const std::vector<I32> ids) { m_aabbs = ids; }

        virtual void collide_x(cInfo our, cInfo other) {}
        virtual void collide_y(cInfo our, cInfo other) {}
    };
}