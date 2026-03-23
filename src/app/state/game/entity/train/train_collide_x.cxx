module;
#include <cmath>
#include <sstream>

module entity.train;

namespace entity {
    void Train::collide_x(aabb::cInfo our, aabb::cInfo other) {
        aabb::cName our_name = aabb::name(our.id);

        if (our_name != aabb::Name::bone) {
            return;
        }

        aabb::Name other_name = aabb::name(other.id);

        //console::log("aabb name: ", aabb::to_string(our_name), "\n");

        cRectF our_rect = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_rect = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();

        if (!is_track(other_type) &&
            //other_type != Type::player &&
            other_type != Type::trigger &&
            other_type != Type::track_trigger_UL) return;


        /*if (std::abs(velocity().y) > std::abs(velocity().x)) {
            collide_y(our, other);
            return;
        }*/

        cF32 diff_x = other_rect.x - our_rect.x;
        cF32 diff_y = other_rect.y - our_rect.y;

        cF32 overlap_x = our_rect.x < other_rect.x ? our_rect.w - other_rect.x : -(other_rect.w - our_rect.x);


        const bool is_larger_vel_x = std::abs(velocity().x) > std::abs(velocity().y);

        if (is_track(other_type) || other_type == Type::trigger) {
            m_is_powered = other.owner->is_powered();
            m_time_left_alive = other.owner->time_left_alive();
            //m_speed = m_start_speed;
            // 
            //console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
            //console::log("is_powered: ", m_is_powered, "\n");
        }

        if (other_type == Type::track_UD) {
            if (other_name == aabb::Name::track) {
                position_add_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
                m_direction.x = 0.0F;
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::track_LR || other_type == Type::track_trigger_UL || other_type == Type::track_trigger_UR) {
            if (other_name == aabb::Name::track) {
                position_add_y(other_rect.y - our_rect.y);
                velocity_y(0.0F);
                m_direction.y = 0.0F;
                if (m_direction.x < 0.0F) {
                    velocity_x(-std::abs(m_speed));
                } else if (m_direction.x > 0.0F) {
                    velocity_x(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::track_corner_UL) {
            if (other_name == aabb::Name::up) {
                if (m_direction.x < 0.0F) {
                    if (our_rect.x < other_rect.x) {
                        velocity_x(0.0F);
                        velocity_y(std::abs(m_speed));
                        position_add_x(other_rect.x - our_rect.x);
                    }
                }
            } else if (other_name == aabb::Name::down) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::track_corner_UR) {
            if (other_name == aabb::Name::up) {
                if (m_direction.x > 0.0F) {
                    if (our_rect.w > other_rect.w) {
                        velocity_x(0.0F);           
                        velocity_y(std::abs(m_speed));
                        position_add_x(other_rect.w - our_rect.w);
                    }
                }
            } else if (other_name == aabb::Name::down) {
                if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                } else if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                }
                position_add_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::track_corner_DL) {
            if (other_name == aabb::Name::up) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::down) {
                if (m_direction.x < 0.0F) {
                    if (our_rect.x < other_rect.x) {
                        velocity_x(0.0F);
                        velocity_y(-std::abs(m_speed));
                        position_add_x(other_rect.x - our_rect.x);                        
                    }
                }
            }
        } else if (other_type == Type::track_corner_DR) {
            if (other_name == aabb::Name::up) {
                if (m_direction.y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (m_direction.y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_rect.x - our_rect.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::down) {
                if (m_direction.x > 0.0F) {
                    if (our_rect.w > other_rect.w) {
                        velocity_x(0.0F);
                        velocity_y(-std::abs(m_speed));
                        position_add_x(other_rect.w - our_rect.w);
                    }
                }
            }
        } else if (other_type == Type::track_L_1x1_0 || other_type == Type::track_L_1x1_1) {
            if (other_name == aabb::Name::track) {
                if (m_direction.y < 0.0F && m_direction.x == 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity({ -std::abs(m_speed) * 0.71F, -std::abs(m_speed) * 0.71F });
                    }
                } else if (m_direction.x > 0.0F && m_direction.y < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity({ std::abs(m_speed) * 0.71F, std::abs(m_speed) * 0.71F });
                    }
                }
            }
        } else if (other_type == Type::track_R_1x1_0 || other_type == Type::track_R_1x1_1) {
            if (other_name == aabb::Name::track) {
                if (m_direction.y < 0.0F && m_direction.x >= 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_x( std::abs(m_speed) * 0.71F);
                        velocity_y(-std::abs(m_speed) * 0.71F);
                    }
                } else if (m_direction.x < 0.0F && m_direction.y < 0.0F) {
                    if (our_rect.y < other_rect.y) {
                        velocity_x(-std::abs(m_speed) * 0.71F);
                        velocity_y( std::abs(m_speed) * 0.71F);
                    }
                }
            }
        } else if (other_type == Type::track_L_2x1_0 || other_type == Type::track_L_2x1_1) {
            if (m_direction.x < 0.0F && m_direction.y <= 0.0F) {
                if (our_rect.x < other_rect.x) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);                    
                }
            } else if (m_direction.x > 0.0F && m_direction.y <= 0.0F) {
                if (our_rect.x > other_rect.x) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);                    
                }
            }
        } else if (other_type == Type::track_R_2x1_0 || other_type == Type::track_R_2x1_1) {
            if (m_direction.x < 0.0F && m_direction.y <= 0.0F) {
                if (our_rect.x < other_rect.x) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);
                    //position_add_y(other_rect.y - our_rect.y);
                }
            } else if (m_direction.x > 0.0F && m_direction.y >= 0.0F) {
                if (our_rect.x > other_rect.x) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);
                    //position_add_y(other_rect.y - our_rect.y);
                }
            }
        } else if (other_type == Type::track_L_1x2_0 || other_type == Type::track_L_1x2_1) {
            if (other_name == aabb::Name::track) {
                if (m_direction.x < 0.0F) {
                    if (our_rect.x < other_rect.x) {
                        velocity_x(-std::abs(m_speed) * 0.5F);
                        velocity_y(-std::abs(m_speed) * 0.9F);
                    }
                } else if (m_direction.x > 0.0F && m_direction.y >= 0.0F) {
                    if (our_rect.x > other_rect.x) {
                        velocity_x(std::abs(m_speed) * 0.5F);
                        velocity_y(std::abs(m_speed) * 0.9F);
                    }
                } else if (m_direction.x > 0.0F && m_direction.y < 0.0F) {
                    if (our_rect.x > other_rect.x) {
                        velocity_x(-std::abs(m_speed) * 0.5F);
                        velocity_y(-std::abs(m_speed) * 0.9F);
                    }
                }
            }
        } else if (other_type == Type::track_R_1x2_0 || other_type == Type::track_R_1x2_1) {
            if (m_direction.x < 0.0F && m_direction.y > 0.0F) {
                if (our_rect.x < other_rect.x) {
                    velocity_y(std::abs(m_speed) * 0.9F);
                    velocity_x(-std::abs(m_speed) * 0.5F);
                }
            } else if (m_direction.x > 0.0F && m_direction.y < 0.0F) {
                if (our_rect.x > other_rect.x) {
                    velocity_x(std::abs(m_speed) * 0.5F);
                    velocity_y(-std::abs(m_speed) * 0.9F);
                }
            } else if (m_direction.x > 0.0F && m_direction.y > 0.0F) {
                if (our_rect.x > other_rect.x) {
                    velocity_x(-std::abs(m_speed) * 0.5F);
                    velocity_y(std::abs(m_speed) * 0.9F);
                }
            }

        }
    }
}