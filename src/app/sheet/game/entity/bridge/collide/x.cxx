module entity.bridge;

namespace entity {
    void Bridge::collide_x(aabb::cInfo our, aabb::cInfo other) {
        collide_y(our, other);
    }
}