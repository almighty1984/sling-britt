module entity.particle.rock;
import particle_system;

namespace entity {
    bool ParticleRock::hurt(Object* culprit) {
        if (!culprit or m_state == state::Type::dead or m_next_state == state::Type::dead) return false;
        m_next_state = state::Type::dead;

        /*particle::spawn(this,
                        particle::Type::hit,
                        position() - Vec2F{ 6.0F, 6.0F } + velocity(),
                        {});*/

        cVec2F sound_pos = { position().x - app::config::extent().x / 2.0F,
                             position().y - app::config::extent().y / 2.0F };

        //console::log(class_name(), "::hurt() sound position: ", sound_pos.x, " ", sound_pos.y, "\n");

        if (!sound_is_playing("dead")) {
            sound_position("dead", sound_pos);
            sound_play("dead");
        }

        switch (culprit->type()) {
            case Type::brick: {
                sound_position("block", sound_pos);
                sound_play("block");
                break;
            }
            case Type::bee:
            case Type::bee_hive:
            case Type::bug:            
            case Type::frog:
            case Type::mole:
            case Type::player: {
                sound_position("hit", sound_pos);
                sound_play("hit");
                break;
            }
        }

        particle::spawn(this, particle::Type::hit, position() - Vec2F{ 4.0F, 4.0F }, {});
        return true;
    }
}