module entity.water_line;

namespace entity {
    void WaterLine::collide_x(aabb::cInfo our, aabb::cInfo other) {
        //if (other.owner and other.owner->type() == Type::player) {
        collide_y(our, other);
        //}
    }
}