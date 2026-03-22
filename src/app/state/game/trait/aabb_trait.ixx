module;
#include <string>
#include <vector>

export module aabb.trait;
//import aabb;
import types;

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
        std::vector<I32> m_aabb_ids;

    public:
        std::vector<I32>& aabb_ids() { return m_aabb_ids; } void aabb_ids(const std::vector<I32> ids) { m_aabb_ids = ids; }

        virtual void collide_x(cInfo our, cInfo other) {}
        virtual void collide_y(cInfo our, cInfo other) {}
    };
}