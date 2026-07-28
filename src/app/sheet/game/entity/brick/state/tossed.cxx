module entity.brick;

namespace entity {
    void Brick::state_tossed(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            m_is_on_ground = false;
            m_weight = m_config.weight();

            deceleration({ 0.0F, 0.0F });
            //m_parent = nullptr;
        }
        if (m_is_on_ground) {
            deceleration({ 0.1F, 0.0F });
            m_next_state = state::Type::idle;
        } else {
            //console::log(class_name(), " toss not on ground\n");
        }
    }
}