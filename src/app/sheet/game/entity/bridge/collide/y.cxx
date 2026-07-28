module entity.bridge;

namespace entity {
    void Bridge::collide_y(aabb::cInfo our, aabb::cInfo other) {
        cType other_type = other.owner->type();

        cVec2F our_UL = aabb::UL(our.id);
        cVec2F our_DR = aabb::DR(our.id);
        cVec2F other_UL = aabb::UL(other.id);
        cVec2F other_DR = aabb::DR(other.id);

        cF32 overlap_y = other_DR.y - our_UL.y;

        cVec2F our_velocity = velocity() + move_velocity();
        cVec2F other_velocity = other.owner->velocity() + other.owner->move_velocity();

        switch (other_type) {
            case Type::player:
            case Type::brick:
            case Type::bug:
            case Type::frog: {
                if (other_UL.y + 8.0F > our_UL.y or other_velocity.y < 0.0F) return;

                add_inputs_from(other.owner);
                add_input(other.owner);
                //return;
                //m_force.y += other.owner->weight();
                //return;
                
                //console::log("offset from root: ", m_offset_from_L, " ", m_offset_from_R, "\n");

                //F32 up_force = (m_offset_from_L * m_offset_from_R) / 1000.0F;

                //console::log("up force: ", up_force, "\n");

                if (other.owner->time_left_colliding_with(m_type) > 0) return;

                //if (other_velocity.y >= 0.0F) {

                    //console::log("start position: ", start_position_on_level().y, "\n");
                    //console::log("input position: ", inputs().front()->start_position_on_level().x, " ", inputs().back()->start_position_on_level().x, "\n");

                    //m_time_in_state = 0;

                    //if (other_type == Type::player) {
                        //console::log("inputs: ", m_inputs.size(), " near R: ", m_is_near_wall_R, "\n");
                        if (m_roots.size() == 2) {
                        //    //console::log(m_roots.front()->position_on_level().x, " ", m_roots.back()->position_on_level().x, "\n");

                            //F32 offset_from_L = position_on_level().x - m_roots.front()->position_on_level().x;
                            //F32 offset_from_R = m_roots.back()->position_on_level().x - position_on_level().x;
                        //    //console::log(offset_from_L, " ", offset_from_R, "\n");

                            //F32 up_force = (start_position_on_level().y - position_on_level().y) * 0.1F;

                            //cF32 factor = 0.001F * other.owner->weight();

                            //m_force.y += (offset_from_L * offset_from_R) / 1000.0F;
                        //    return;

                            //F32 input_0_offset_from_L = m_children.front()->position_on_level().x - m_roots.front()->position_on_level().x;
                            //F32 input_1_offset_from_R = m_roots.back()->position_on_level().x - m_children.back()->position_on_level().x;


                            //F32 input_0_input_0_offset_from_L = m_children.front()->children_front()->position_on_level().x - m_roots.front()->position_on_level().x;
                            //F32 input_1_input_1_offset_from_R = m_roots.back()->position_on_level().x - m_children.back()->children_back()->position_on_level().x;


                        //    //console::log("up_force: ", up_force, "\n");


                            //console::log("weight: ", other.owner->weight(), "\n");
                        
                            //m_force.y = offset_from_L * offset_from_R * factor
                            //            +
                                        //input_0_offset_from_L * factor + input_1_offset_from_R * factor
                                        //+
                                        //input_0_input_0_offset_from_L * factor + input_1_input_1_offset_from_R * factor
                                        //+
                              //          up_force
                                //        ;

                        //    //m_force.y = offset_from_L * offset_from_R / 400.0F;


                        }


                        if (m_inputs.size() == 2) {
                        

                            //velocity_y((offset_from_L + offset_from_R) / 100.0F);
                        }
                    //}

                    //m_start_offset.y = 4.0F;

                    /*if (m_is_near_wall_L or m_is_near_wall_R) {
                        m_start_offset.y *= 0.5F;
                    }
                    else {
                        for (auto& i : m_inputs) {
                            if (!i or i->is_near_wall_L() or i->is_near_wall_R()) continue;
                            i->start_offset({ 0.0F, m_start_offset.y * 1.0F });
                            i->time_in_state(0);

                            for (auto& ii : i->m_inputs) {
                                if (!ii or ii->is_near_wall_L() or ii->is_near_wall_R()) continue;
                                ii->start_offset({ 0.0F, m_start_offset.y * 0.9F });
                                ii->time_in_state(0);

                                for (auto& iii : ii->m_inputs) {
                                    if (!iii or iii->is_near_wall_L() or iii->is_near_wall_R()) continue;
                                    iii->start_offset({ 0.0F, m_start_offset.y * 0.8F });
                                    iii->time_in_state(0);
                                }
                            }

                        }
                    }*/
                    /*if (other_DR.y + 2.0F < our_UL.y + m_start_offset.y) {
                        return;
                    }*/
                    //m_force.y = other_velocity.y * 4.0F;
                    
                    //other.owner->position_add_y(-overlap_y);
                    //
                    //other.owner->velocity_y(0.0F);
                    ////other.owner->velocity_y(0.0F);
                    //other.owner->move_velocity_x(0.0F);
                    //other.owner->move_velocity_y(0.0F);
                    //other.owner->is_on_ground(true);

                    //console::log("inputs: ", m_inputs.size(), "\n");

                //}
                break;
            }
        }
    }
}