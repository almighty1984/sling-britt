module entity.particle.down_thrust;

namespace entity {    
    void ParticleDownThrust::collide_y(aabb::cInfo our, aabb::cInfo other) {
        collide_x(our, other);
    }
}