export module aabb.trait;
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
        constexpr bool is_valid(size_t i) { return (i >= 0 and i < m_aabbs.size()) ? true : false; }
    public:
        std::vector<I32>& aabbs() { return m_aabbs; } void aabbs(const std::vector<I32> ids) { m_aabbs = ids; }
        //I32 aabb(cI32 i) { return is_valid(i) ? m_aabbs.at(i) : -1; }

        virtual I32 aabb(aabb::cName name) { return -1; }

        virtual void collide_x(cInfo our, cInfo other) {}
        virtual void collide_y(cInfo our, cInfo other) {}
    };
}