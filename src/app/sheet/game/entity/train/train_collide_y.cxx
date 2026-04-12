module entity.train;

namespace entity {
    void Train::collide_y(aabb::cInfo our, aabb::cInfo other) {
        aabb::cName our_name = aabb::name(our.id);

        if (our_name != aabb::Name::bone) {
            return;
        }

        cRectF our_points = { aabb::point(our.id, 0).x, aabb::point(our.id, 0).y,
                            aabb::point(our.id, 3).x, aabb::point(our.id, 3).y };
        cRectF other_points = { aabb::point(other.id, 0).x, aabb::point(other.id, 0).y,
                              aabb::point(other.id, 3).x, aabb::point(other.id, 3).y };

        cType other_type = other.owner->type();

        aabb::cName other_name = aabb::name(other.id);

        if (!is_track(other_type) and
            //other_type != Type::player and
            other_type != Type::trigger and
            other_type != Type::track_trigger_UL) return;


        /*if (std::abs(velocity().x) > std::abs(velocity().y)) {
            collide_x(our, other);
            return;
        }*/


        cF32 diff_x = other_points.x - our_points.x;
        cF32 diff_y = other_points.y - our_points.y;

        cF32 overlap_y = our_points.y < other_points.y ? our_points.h - other_points.y : -(other_points.h - our_points.y);

        bool is_larger_vel_y = std::abs(velocity().y) > std::abs(velocity().x);

        //m_speed = 0.0F;
        if (is_track(other_type) or other_type == Type::trigger) {
            m_is_powered = other.owner->is_powered();
            m_time_left_alive = other.owner->time_left_alive();
            //m_speed = m_start_speed;
            //console::log("time_left_alive: ", other.owner->time_left_alive(), "\n");
            //console::log("is_powered: ", m_is_powered, "\n");
        }

        if (other_type == Type::track_UD) {
            if (other_name == aabb::Name::track) {
                position_add_x(other_points.x - our_points.x);
                velocity_x(0.0F);
                direction_x(0.0F);
                if (direction().y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                } else if (direction().y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::track_LR or other_type == Type::track_trigger_UL or other_type == Type::track_trigger_UR) {
            if (other_name == aabb::Name::track) {
                position_add_y(other_points.y - our_points.y);
                velocity_y(0.0F);
                direction_y(0.0F);
                if (direction().x < 0.0F) {
                    velocity_x(-std::abs(m_speed));
                } else if (direction().x > 0.0F) {
                    velocity_x(std::abs(m_speed));
                }
            }
        } else if (other_type == Type::track_trigger_UL or other_type == Type::track_LR) {
            if (other_name == aabb::Name::up) {
                if (our_points.y < other_points.h) {
                    velocity_y(0.0F);
                    position_add_y(-overlap_y);
                }
            } else if (other_name == aabb::Name::down) {
                if (our_points.h > other_points.y) {
                    velocity_y(0.0F);
                    position_add_y(-overlap_y);
                }
            } else if (other_name == aabb::Name::track) {
                if (direction().x < 0.0F) {
                    velocity_x(-std::abs(m_speed));
                }
                else if (direction().x > 0.0F) {
                    velocity_x(std::abs(m_speed));
                }
                velocity_y(diff_y / 16.0F * std::abs(m_speed));
            }
        } else if (other_type == Type::track_corner_UL) {
            if (other_name == aabb::Name::up) {
                if (direction().y < 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity_y(0.0F);
                        velocity_x(std::abs(m_speed));
                        position_add_y(other_points.y - our_points.y);
                    }
                }
                else if (direction().y > 0.0F and direction().x < 0.0F) {
                    position_add_y(other_points.y - our_points.y);
                    velocity_y(0.0F);                    
                    velocity_x(-std::abs(m_speed));
                }
            } else if (other_name == aabb::Name::down) {
                if (direction().y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                }
                else if (direction().y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_points.x - our_points.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::track_corner_UR) {
            if (other_name == aabb::Name::up) {
                if (direction().y < 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                        position_add_y(other_points.y - our_points.y);
                    }
                }
            } else if (other_name == aabb::Name::down) {
                if (direction().y < 0.0F) {
                    velocity_y(-std::abs(m_speed));                    
                }
                else if (direction().y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_points.x - our_points.x);
                velocity_x(0.0F);
            }
        } else if (other_type == Type::track_corner_DL) {
            if (other_name == aabb::Name::up) {
                if (direction().y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                }
                else if (direction().y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_points.x - our_points.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::down) {
                if (direction().y > 0.0F and direction().x >= 0.0F) {
                    if (our_points.y > other_points.y) {
                        velocity_y(0.0F);
                        velocity_x(std::abs(m_speed));
                        position_add_y(other_points.y - our_points.y);
                    }
                } else if (direction().y < 0.0F and direction().x < 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                        position_add_y(other_points.y - our_points.y);
                    }
                }
            }
        } else if (other_type == Type::track_corner_DR) {
            if (other_name == aabb::Name::up) {
                if (direction().y < 0.0F) {
                    velocity_y(-std::abs(m_speed));
                }
                else if (direction().y > 0.0F) {
                    velocity_y(std::abs(m_speed));
                }
                position_add_x(other_points.x - our_points.x);
                velocity_x(0.0F);
            } else if (other_name == aabb::Name::down) {
                if (direction().y > 0.0F) {
                    if (our_points.y > other_points.y) {
                        position_add_y(other_points.y - our_points.y);
                        velocity_y(0.0F);
                        velocity_x(-std::abs(m_speed));
                    }
                }
                else if (direction().x > 0.0F) {
                    if (our_points.y < other_points.y) {                        
                        position_add_y(other_points.y - our_points.y); 
                        velocity_x(std::abs(m_speed));
                    }
                }
            }
        } else if (other_type == Type::track_L_1x1_0 or other_type == Type::track_L_1x1_1) {
            if (other_name == aabb::Name::track) {
                if (direction().y > 0.0F and direction().x == 0.0F) {
                    if (our_points.y > other_points.y) {
                        velocity({ std::abs(m_speed) * 0.71F, std::abs(m_speed) * 0.71F });
                    }
                }
                else if (direction().x > 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity({ std::abs(m_speed) * 0.71F, std::abs(m_speed) * 0.71F });

                        //position_add_y(other_points.y - our_points.y);
                    }
                } else if (direction().y < 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity({ -std::abs(m_speed) * 0.71F, -std::abs(m_speed) * 0.71F });
                    }
                }
            }
        } else if (other_type == Type::track_R_1x1_0 or other_type == Type::track_R_1x1_1) {
            if (other_name == aabb::Name::track) {
                if (direction().y > 0.0F and direction().x <= 0.0F) {
                    if (our_points.y > other_points.y) {
                        velocity_x(-std::abs(m_speed) * 0.71F);
                        velocity_y( std::abs(m_speed) * 0.71F);
                    }
                }
                else if (direction().y < 0.0F and direction().x >= 0.0F) {
                    if (our_points.y < other_points.y) {
                        velocity_x( std::abs(m_speed) * 0.71F);
                        velocity_y(-std::abs(m_speed) * 0.71F);

                        //position_add_y(other_points.y - our_points.y);
                    }
                } else if (direction().y < 0.0F and direction().x <= 0.0F) {
                    position_add_y(other_points.y - our_points.y);
                    if (our_points.y < other_points.y) {
                        velocity_x(-std::abs(m_speed) * 0.71F);
                        velocity_y( std::abs(m_speed) * 0.71F);
                    }
                }
            }
        } else if (other_type == Type::track_L_1x2_0 or other_type == Type::track_L_1x2_1) {
            if (direction().y < 0.0F) {
                if (our_points.y < other_points.y) {
                    velocity_y(-std::abs(m_speed) * 0.75F);
                    velocity_x(-std::abs(m_speed) * 0.5F);
                }
            } else if (direction().y > 0.0F) {
                if (our_points.y > other_points.y) {
                    velocity_y(std::abs(m_speed) * 0.9F);
                    velocity_x(std::abs(m_speed) * 0.5F);
                }
            }
        }
        else if (other_type == Type::track_R_1x2_0 or other_type == Type::track_R_1x2_1) {
            if (direction().y < 0.0F/* and m_direction.x >= 0.0F*/) {
                if (our_points.y < other_points.y) {
                    velocity_y(-std::abs(m_speed) * 0.9F);
                    velocity_x(std::abs(m_speed) * 0.5F);
                }
            } else if (direction().y > 0.0F/* and m_direction.x <= 0.0F*/) {
                if (our_points.y > other_points.y) {
                    velocity_y(std::abs(m_speed) * 0.9F);
                    velocity_x(-std::abs(m_speed) * 0.5F);
                }
            }
        } else if (other_type == Type::track_L_2x1_0 or other_type == Type::track_L_2x1_1) {
            if (direction().y < 0.0F and direction().x <= 0.0F) {
                if (our_points.y < other_points.y) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);
                }
            } else if (direction().y > 0.0F and direction().x >= 0.0F) {
                if (our_points.y > other_points.y) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);
                }
            }
        } else if (other_type == Type::track_R_2x1_0 or other_type == Type::track_R_2x1_1) {
            if (direction().y < 0.0F and direction().x >= 0.0F) {
                if (our_points.y < other_points.y) {
                    velocity_x(std::abs(m_speed) * 0.9F);
                    velocity_y(-std::abs(m_speed) * 0.5F);
                }
            } else if (direction().y > 0.0F and direction().x <= 0.0F) {
                if (our_points.y > other_points.y) {
                    velocity_x(-std::abs(m_speed) * 0.9F);
                    velocity_y(std::abs(m_speed) * 0.5F);
                }
            }
        }
    }
}