module entity.bug;

namespace entity {
    void Bug::state_bounce(cF32 dt) {
        if (m_is_first_state_update) {
            m_is_first_state_update = false;
            /*sound::position(sound("bounce"), { position().x - app::config::extent().x / 2.0F, position().y - app::config::extent().y / 2.0F });
            sound::play(sound("bounce"));*/

            reset_anim("bounce");
        }
        m_parent = nullptr;
        //int num_frames = anim()->texture_extent.x / anim()->source.w;
        //console::log("num_frames: ", anim()->current_frame(), "\n");
        if (anim::current_frame(m_current_anim) + 1 == anim::num_frames(m_current_anim)) {
            m_next_state = state::Type::upended;
        }
    }
}