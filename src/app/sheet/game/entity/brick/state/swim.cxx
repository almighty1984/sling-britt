module entity.brick;

namespace entity {
    void Brick::state_swim(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
        }
        if (m_is_on_ground) {
            deceleration({ 0.05F, 0.05F });
        }
        else {
            deceleration({ 0.0F, 0.0F });
        }
        acceleration({ 0.05F, 0.05F });
        max_velocity({ 1.0F, 1.0F });
    }
}