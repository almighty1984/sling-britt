module;
#include <vector>
#include <string>
#include <list>
#include <memory>
#include <sstream>

export module particle_system;
import console;
import entity;
import entity.particle;
import entity.particle.brick;
import entity.particle.bubble;
import entity.particle.down_thrust;
import entity.particle.health;
import entity.particle.interact;
import entity.particle.melee;
import entity.particle.sense;
import entity.particle.shot;
import entity.particle.drop;
import sprite;
import transform;
import types;
import window;

struct Spawn {
    entity::Object* parent   = nullptr;
    particle::Type  type     = particle::Type::none;
    Vec2F           position = { 0.0F, 0.0F },
                    velocity = { 0.0F, 0.0F };
    entity::State   state    = entity::State::none;
};
using cSpawn = const Spawn;

std::vector<Spawn>           s_to_spawn;
std::list<entity::Particle*> s_particle_entities;

export namespace particle {
    void draw(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : s_particle_entities) {
            if (i && i->start_layer() == layer && !i->is_to_erase()) {
                i->draw(window);
            }
        }
    }
    void draw_aabb(std::unique_ptr<Window>& window, cU8 layer) {
        for (auto& i : s_particle_entities) {
            if (i && i->start_layer() == layer && !i->is_to_erase()) {
                i->draw_aabb(window);                
            }
        }
    }
    void clear() {
        s_to_spawn.clear();        
        for (auto it = s_particle_entities.begin(); it != s_particle_entities.end(); ++it) {
            if (!(*it)) continue;
            delete (*it);
        }
        s_particle_entities.clear();
    }
    void spawn(cSpawn to_spawn) {
        s_to_spawn.emplace_back(to_spawn);
    }
    void spawn(entity::Object* parent, particle::cType type, cVec2F position, cVec2F velocity, entity::cState state = entity::State::none) {
        spawn(Spawn{ .parent   = parent,
                .type     = type,
                .position = position,
                .velocity = velocity,
                .state    = state });
    }
    void spawn_fan(entity::Object* parent, cF32 start_angle, cF32 end_angle, cU8 num, particle::cType type, cVec2F position, cVec2F velocity, cF32 speed, entity::cState state = entity::State::none) {
        cF32 end_angle_adjusted = start_angle > end_angle ? end_angle + 360.0F : end_angle;
        cF32 angle_to_radians = (3.1415926535F / 180.0F);
        cF32 end_radian = end_angle_adjusted * angle_to_radians;
        cF32 start_radian = start_angle * angle_to_radians;
        cF32 radian_range = end_radian - start_radian;

        F32 radians = start_radian;
        for (U8 i = 0; i <= num; ++i) {
            spawn({ .parent   = parent,
                    .type     = type,
                    .position = position,
                    .velocity = velocity + Vec2F{ std::cosf(radians), std::sinf(radians) } * speed,
                    .state    = state });
            radians += (radian_range / num);
        }
    }
    void update() {
        for (auto& i : s_particle_entities) {
            if (i) i->update();
        }        
    }
    void check_to_erase() {  
        for (auto it = s_particle_entities.begin(); it != s_particle_entities.end(); ++it) {            
            if (*it && (*it)->is_to_erase()) {
                delete (*it);
                it = s_particle_entities.erase(it);
                if (it == s_particle_entities.end()) {
                    break;
                }
            }
        }
    }
    void check_to_spawn() {
        //console::log("particles: ", s_particle_entities.size(), "\n");

        for (auto& to_spawn : s_to_spawn) {
            if (!to_spawn.parent) {
                console::log("particle::check_to_spawn() particle with no parent\n");
                continue;
            }

            Vec2F position      = to_spawn.position;
            Vec2F velocity      = to_spawn.velocity;
            entity::State state = to_spawn.state;

            if (to_spawn.type == Type::bubble) {
                console::log("spawn bubble\n");
                s_particle_entities.emplace_back(new entity::ParticleBubble);
                s_particle_entities.back()->load_config("res/entity/particle/bubble.cfg");
                //m_entity_objects.back()->direction({ (F32)random::number(0, 1), 0.0F });
            }
            else if (to_spawn.type == Type::health) {
                s_particle_entities.emplace_back(new entity::ParticleHealth);
                s_particle_entities.back()->load_config("res/entity/particle/health.cfg");
            }
            else if (to_spawn.type == Type::hit) {
                s_particle_entities.emplace_back(new entity::Particle);
                s_particle_entities.back()->load_config("res/entity/particle/hit.cfg");
                velocity = {};
            }
            else if (to_spawn.type == Type::interact) {                
                s_particle_entities.emplace_back(new entity::ParticleInteract);
                s_particle_entities.back()->load_config("res/entity/particle/interact_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
            }
            else if (to_spawn.type == Type::melee) {
                s_particle_entities.emplace_back(new entity::ParticleMelee);
                s_particle_entities.back()->load_config("res/entity/particle/melee_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
                //console::log("time left dead: ", s_particle_entities.back()->time_left_dead(), "\n");
                console::log("particle::check_to_spawn Melee velocity: ", velocity.x, " ", velocity.y, "\n");
            }
            else if (to_spawn.type == Type::sense) {
                //console::log("spawn sense ", "res/entity/particle/sense_" + entity::to_string(to_spawn.parent->type()) + ".cfg", "\n");
                s_particle_entities.emplace_back(new entity::ParticleSense);
                s_particle_entities.back()->load_config("res/entity/particle/sense_" + entity::to_string(to_spawn.parent->type()) + ".cfg");
                s_particle_entities.back()->start_layer(NUM_VISIBLE_LAYERS - 1);
            }
            else if (to_spawn.type == Type::shot) {
                s_particle_entities.emplace_back(new entity::ParticleShot);
                s_particle_entities.back()->load_config("res/entity/particle/shot_" + entity::to_string(to_spawn.parent->type()) + ".cfg");                                
            }
            else if (to_spawn.type == Type::brick       ||
                     to_spawn.type == Type::down_thrust ||
                     to_spawn.type == Type::drop        ||
                     to_spawn.type == Type::drop_blood  ||
                     to_spawn.type == Type::dust        ||
                     to_spawn.type == Type::dust_L      ||
                     to_spawn.type == Type::dust_R) {
                if (to_spawn.type == Type::brick) {
                    //state = entity::State::idle;
                    s_particle_entities.emplace_back(new entity::ParticleBrick);
                }
                else if (to_spawn.type == Type::down_thrust) {
                    s_particle_entities.emplace_back(new entity::ParticleDownThrust);
                }
                else if (to_spawn.type == Type::drop_blood || to_spawn.type == Type::drop) {
                    s_particle_entities.emplace_back(new entity::ParticleDrop);
                }
                else {
                    s_particle_entities.emplace_back(new entity::Particle);
                }
                s_particle_entities.back()->load_config("res/entity/particle/" + particle::to_string(to_spawn.type) + ".cfg");
            }

            if (s_particle_entities.back()->transform_id() != -1) {
                transform::position(s_particle_entities.back()->transform_id(), position);
                transform::velocity(s_particle_entities.back()->transform_id(), velocity);
            }

            if (to_spawn.parent) {
                s_particle_entities.back()->parent(to_spawn.parent);
                sprite::layer(s_particle_entities.back()->sprite_id(), NUM_VISIBLE_LAYERS - 1);
                s_particle_entities.back()->start_layer(NUM_VISIBLE_LAYERS - 1);

                if (to_spawn.parent->prev_state() == entity::State::swim) {
                    s_particle_entities.back()->acceleration(to_spawn.parent->acceleration());
                    s_particle_entities.back()->max_velocity(to_spawn.parent->max_velocity());
                }
            }

            s_particle_entities.back()->next_state(state);

            //s_particle_entities.back()->update();
        }
        s_to_spawn.clear();
    }    
}
