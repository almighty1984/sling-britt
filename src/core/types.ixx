module;
#include <concepts>
#include <string>
#include <map>

export module types;

export using   U8 =       unsigned char;
export using  cU8 = const unsigned char;
export using  U16 =       unsigned short;
export using cU16 = const unsigned short;
export using  U32 =       unsigned int;
export using cU32 = const unsigned int;
export using  U64 =       unsigned long;
export using cU64 = const unsigned long;

export using   I8 =       char;
export using  cI8 = const char;
export using  I16 =       short;
export using cI16 = const short;
export using  I32 =       int;
export using cI32 = const int;
export using  I64 =       long;
export using cI64 = const long;

export using  F32 =       float;
export using cF32 = const float;
export using  F64 =       double;
export using cF64 = const double;

export constexpr U16 WINDOW_W = 320;
export constexpr U16 WINDOW_H = 180;

export constexpr U16 U16_MAX = -1;
//export constexpr cU8 NUM_LAYERS = 2;

export constexpr U8 NUM_VISIBLE_LAYERS = 15;
export constexpr U8 NUM_LEVEL_LAYERS = 10;

export namespace aabb {
    enum class Name {
        none = 0,
        one, two, three, four, five, six, seven, eight, nine,
        up, down, left, right,
        body, body_swim,
        bone,
        hit_ground,
        track
    };
    using cName = const Name;
    const std::string to_string(cName n) {
        switch (n) {
        case Name::one:        return "one";        break;
        case Name::two:        return "two";        break;
        case Name::three:      return "three";      break;
        case Name::four:       return "four";       break;
        case Name::five:       return "five";       break;
        case Name::six:        return "six";        break;
        case Name::seven:      return "seven";      break;
        case Name::eight:      return "eight";      break;
        case Name::nine:       return "nine";       break;
        case Name::up:         return "up";         break;
        case Name::down:       return "down";       break;
        case Name::left:       return "left";       break;
        case Name::right:      return "right";      break;
        case Name::body:       return "body";       break;
        case Name::body_swim:  return "body_swim";  break;
        case Name::bone:       return "bone";       break;
        case Name::hit_ground: return "hit_ground"; break;
        case Name::track:      return "track";      break;
        default:               return "";           break;
        }
    }
    const std::map<std::string, Name> string_to_aabb_name_map{
        { "one"        , Name::one        },
        { "two"        , Name::two        },
        { "three"      , Name::three      },
        { "four"       , Name::four       },
        { "five"       , Name::five       },
        { "six"        , Name::six        },
        { "seven"      , Name::seven      },
        { "eight"      , Name::eight      },
        { "nine"       , Name::nine       },        
        { "up"         , Name::up         },
        { "down"       , Name::down       },
        { "left"       , Name::left       },
        { "right"      , Name::right      },
        { "body"       , Name::body       },
        { "body_swim"  , Name::body_swim  },
        { "bone"       , Name::bone       },
        { "hit_ground" , Name::hit_ground },
        { "track"      , Name::track      }        
    };
    cName string_to_name(const std::string& s) {        
        return string_to_aabb_name_map.find(s) == string_to_aabb_name_map.end() ? Name::none : string_to_aabb_name_map.at(s);
    }
}

export namespace anim {
    enum class Type {
        none = 0,
        bounce,
        crawl,
        dead, down_thrust, duck,
        fall,
        hurt, hit_ground, hover,
        idle,
        jump, jump_spin, jump_skid,
        ledge_grab, ledge_climb, lever,
        melee,
        rise, run,
        skid, slide_ground, slide_wall, stunned, swim, sling,
        walk,
        upended
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {
        case Type::bounce:       return "bounce";
        case Type::crawl:        return "crawl";
        case Type::dead:         return "dead";
        case Type::down_thrust:  return "down_thrust";
        case Type::duck:         return "duck";
        case Type::fall:         return "fall";
        case Type::hurt:         return "hurt";
        case Type::hit_ground:   return "hit_ground";
        case Type::hover:        return "hover";
        case Type::idle:         return "idle";
        case Type::jump:         return "jump";
        case Type::jump_spin:    return "jump_spin";
        case Type::jump_skid:    return "jump_skid";
        case Type::ledge_grab:   return "ledge_grab";
        case Type::ledge_climb:  return "ledge_climb";
        case Type::lever:        return "lever";
        case Type::melee:        return "melee";
        case Type::rise:         return "rise";
        case Type::run:          return "run";
        case Type::skid:         return "skid";
        case Type::slide_ground: return "slide_ground";
        case Type::slide_wall:   return "slide_wall";
        case Type::stunned:      return "stunned";
        case Type::swim:         return "swim";
        case Type::sling:        return "sling";
        case Type::walk:         return "walk";
        case Type::upended:      return "upended";
        default:                 return "";
        }
    }
    const std::map<std::string, Type> string_to_anim_type_map{
        { "bounce"       , Type::bounce       },
        { "crawl"        , Type::crawl        },
        { "dead"         , Type::dead         },
        { "down_thrust"  , Type::down_thrust  },
        { "duck"         , Type::duck         },
        { "fall"         , Type::fall         },
        { "hurt"         , Type::hurt         },
        { "hit_ground"   , Type::hit_ground   },
        { "hover"        , Type::hover        },
        { "idle"         , Type::idle         },
        { "jump"         , Type::jump         },
        { "jump_spin"    , Type::jump_spin    },
        { "jump_skid"    , Type::jump_skid    },
        { "ledge_grab"   , Type::ledge_grab   },
        { "ledge_climb"  , Type::ledge_climb  },
        { "lever"        , Type::lever        },
        { "melee"        , Type::melee        },
        { "rise"         , Type::rise         },
        { "run"          , Type::run          },
        { "skid"         , Type::skid         },
        { "slide_ground" , Type::slide_ground },
        { "slide_wall"   , Type::slide_wall   },
        { "stunned"      , Type::stunned      },
        { "swim"         , Type::swim         },
        { "sling"        , Type::sling        },
        { "walk"         , Type::walk         },
        { "upended"      , Type::upended      }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_anim_type_map.find(s) != string_to_anim_type_map.end()) {
            return string_to_anim_type_map.at(s);
        }
        return Type::none;
    }
}

export namespace entity {
    enum class Type {
        none = 0,
        arch_L_1x1, arch_R_1x1, arch_L_2x1_0, arch_L_2x1_1, arch_R_2x1_0, arch_R_2x1_1,
        brick, bridge, bug,
        clip, clip_D, clip_LD, clip_RD, clip_L, clip_R, clip_ledge, clip_U,
        coin_0,
        conduit_UD, conduit_LR,
        conduit_corner_UL, conduit_corner_UR, conduit_corner_DL, conduit_corner_DR,
        conduit_L_1x1_0, conduit_L_1x1_1, conduit_R_1x1_0, conduit_R_1x1_1,
        conduit_L_1x2_0, conduit_L_1x2_1, conduit_R_1x2_0, conduit_R_1x2_1,
        conduit_L_2x1_0, conduit_L_2x1_1, conduit_R_2x1_0, conduit_R_2x1_1,        
        frog,
        grass_0, grass_0_L_1x1, grass_0_R_1x1, grass_0_L_2x1_0, grass_0_L_2x1_1, grass_0_R_2x1_0, grass_0_R_2x1_1,
        grass_1, grass_1_L_1x1, grass_1_R_1x1, grass_1_L_2x1_0, grass_1_L_2x1_1, grass_1_R_2x1_0, grass_1_R_2x1_1,
        grass_2, grass_2_L_1x1, grass_2_R_1x1, grass_2_L_2x1_0, grass_2_L_2x1_1, grass_2_R_2x1_0, grass_2_R_2x1_1,
        level_center, level_L_0, level_R_0,
        logic_and_UD, logic_and_LR,
        logic_not_U, logic_not_D, logic_not_L, logic_not_R,
        logic_or_UD, logic_or_LR,
        mole,
        particle_brick, particle_bubble,
        particle_down_thrust, particle_drop,
        particle_health,
        particle_interact,
        particle_melee,
        particle_sense, particle_shot,
        player,
        slope_L_1x1, slope_R_1x1, slope_L_2x1_0, slope_L_2x1_1, slope_R_2x1_0, slope_R_2x1_1, slope_U,
        spring_U,
        sling,        
        track_UD, track_LR,
        track_corner_UL, track_corner_UR, track_corner_DL, track_corner_DR,
        track_L_1x1_0, track_L_1x1_1, track_R_1x1_0, track_R_1x1_1,
        track_L_1x2_0, track_L_1x2_1, track_R_1x2_0, track_R_1x2_1,
        track_L_2x1_0, track_L_2x1_1, track_R_2x1_0, track_R_2x1_1,
        train,
        trigger, conduit_trigger_UL, conduit_trigger_UR, track_trigger_UL, track_trigger_UR,
        water_line, water_line_L, water_line_R
    };
    using cType = const Type;

    bool is_arch(cType t) {
        return t == Type::arch_L_1x1 || t == Type::arch_L_2x1_0 || t == Type::arch_L_2x1_1 ||
            t == Type::arch_R_1x1 || t == Type::arch_R_2x1_0 || t == Type::arch_R_2x1_1;
    }
    bool is_slope(cType t) {
        return t == Type::slope_L_1x1 || t == Type::slope_L_2x1_0 || t == Type::slope_L_2x1_1 ||
            t == Type::slope_R_1x1 || t == Type::slope_R_2x1_0 || t == Type::slope_R_2x1_1 ||
            t == Type::slope_U;
    }
    bool is_clip(cType t) {
        return t == Type::clip || t == Type::clip_U || t == Type::clip_D ||
            t == Type::clip_L || t == Type::clip_R || t == Type::clip_LD || t == Type::clip_RD ||
            t == Type::clip_ledge;
    }
    bool is_conduit(cType t) {
        return t == Type::conduit_trigger_UL || t == Type::conduit_trigger_UR ||
            t == Type::conduit_UD || t == Type::conduit_LR ||            
            t == Type::conduit_corner_UL || t == Type::conduit_corner_UR || t == Type::conduit_corner_DL || t == Type::conduit_corner_DR ||
            t == Type::conduit_L_1x1_0 || t == Type::conduit_L_1x1_1 || t == Type::conduit_R_1x1_0 || t == Type::conduit_R_1x1_1 ||
            t == Type::conduit_L_1x2_0 || t == Type::conduit_L_1x2_1 || t == Type::conduit_R_1x2_0 || t == Type::conduit_R_1x2_1 ||
            t == Type::conduit_L_2x1_0 || t == Type::conduit_L_2x1_1 || t == Type::conduit_R_2x1_0 || t == Type::conduit_R_2x1_1
            ;
    }
    bool is_coin(cType t) {
        return t == Type::coin_0;
    }

    bool is_grass_0(cType t) { return t == Type::grass_0 || t == Type::grass_0_L_1x1 || t == Type::grass_0_R_1x1 || t == Type::grass_0_L_2x1_0 || t == Type::grass_0_L_2x1_1 || t == Type::grass_0_R_2x1_0 || t == Type::grass_0_R_2x1_1; }
    bool is_grass_1(cType t) { return t == Type::grass_1 || t == Type::grass_1_L_1x1 || t == Type::grass_1_R_1x1 || t == Type::grass_1_L_2x1_0 || t == Type::grass_1_L_2x1_1 || t == Type::grass_1_R_2x1_0 || t == Type::grass_1_R_2x1_1; }
    bool is_grass_2(cType t) { return t == Type::grass_2 || t == Type::grass_2_L_1x1 || t == Type::grass_2_R_1x1 || t == Type::grass_2_L_2x1_0 || t == Type::grass_2_L_2x1_1 || t == Type::grass_2_R_2x1_0 || t == Type::grass_2_R_2x1_1; }
    bool is_grass(cType t) { return is_grass_0(t) || is_grass_1(t) || is_grass_2(t); }
    bool is_logic_and(cType t) { return t == Type::logic_and_UD || t == Type::logic_and_LR; }
    bool is_logic_not(cType t) { return t == Type::logic_not_U || t == Type::logic_not_D || t == Type::logic_not_L || t == Type::logic_not_R; }
    bool is_logic_or(cType t) { return t == Type::logic_or_UD || t == Type::logic_or_LR; }
    bool is_logic(cType t) { return is_logic_and(t) || is_logic_not(t) || is_logic_or(t); }
    bool is_particle(cType t) { return t == Type::particle_sense; }
    bool is_track(cType t) {
        return t == Type::track_trigger_UL || t == Type::track_trigger_UR ||
               t == Type::track_UD || t == Type::track_LR ||
               t == Type::track_corner_UL || t == Type::track_corner_UR || t == Type::track_corner_DL || t == Type::track_corner_DR ||
               t == Type::track_L_1x1_0 || t == Type::track_L_1x1_1 || t == Type::track_R_1x1_0 || t == Type::track_R_1x1_1 ||
               t == Type::track_L_1x2_0 || t == Type::track_L_1x2_1 || t == Type::track_R_1x2_0 || t == Type::track_R_1x2_1 ||
               t == Type::track_L_2x1_0 || t == Type::track_L_2x1_1 || t == Type::track_R_2x1_0 || t == Type::track_R_2x1_1;
    }
    bool is_water_line(cType t) { return t == Type::water_line || t == Type::water_line_L || t == Type::water_line_R; }

    const std::map<std::string, Type> string_to_entity_type_map{
        { "arch_L_1x1"           , Type::arch_L_1x1           },
        { "arch_R_1x1"           , Type::arch_R_1x1           },
        { "arch_L_2x1_0"         , Type::arch_L_2x1_0         },
        { "arch_L_2x1_1"         , Type::arch_L_2x1_1         },
        { "arch_R_2x1_0"         , Type::arch_R_2x1_0         },
        { "arch_R_2x1_1"         , Type::arch_R_2x1_1         },
        { "brick"                , Type::brick                },
        { "bridge"               , Type::bridge               },
        { "bug"                  , Type::bug                  },
        { "clip"                 , Type::clip                 },
        { "clip_L"               , Type::clip_L               },
        { "clip_R"               , Type::clip_R               },
        { "clip_D"               , Type::clip_D               },
        { "clip_LD"              , Type::clip_LD              },
        { "clip_RD"              , Type::clip_RD              },
        { "clip_ledge"           , Type::clip_ledge           },
        { "clip_U"               , Type::clip_U               },
        { "coin_0"               , Type::coin_0               },
        { "conduit_trigger_UL"   , Type::conduit_trigger_UL   },
        { "conduit_trigger_UR"   , Type::conduit_trigger_UR   },
        { "conduit_UD"           , Type::conduit_UD           },
        { "conduit_LR"           , Type::conduit_LR           },
        { "conduit_corner_UL"    , Type::conduit_corner_UL    },
        { "conduit_corner_UR"    , Type::conduit_corner_UR    },
        { "conduit_corner_DL"    , Type::conduit_corner_DL    },
        { "conduit_corner_DR"    , Type::conduit_corner_DR    },
        { "conduit_L_1x1_0"      , Type::conduit_L_1x1_0      },
        { "conduit_L_1x1_1"      , Type::conduit_L_1x1_1      },
        { "conduit_R_1x1_0"      , Type::conduit_R_1x1_0      },
        { "conduit_R_1x1_1"      , Type::conduit_R_1x1_1      },
        { "conduit_L_1x2_0"      , Type::conduit_L_1x2_0      },
        { "conduit_L_1x2_1"      , Type::conduit_L_1x2_1      },
        { "conduit_R_1x2_0"      , Type::conduit_R_1x2_0      },
        { "conduit_R_1x2_1"      , Type::conduit_R_1x2_1      },
        { "conduit_L_2x1_0"      , Type::conduit_L_2x1_0      },
        { "conduit_L_2x1_1"      , Type::conduit_L_2x1_1      },
        { "conduit_R_2x1_0"      , Type::conduit_R_2x1_0      },
        { "conduit_R_2x1_1"      , Type::conduit_R_2x1_1      },
        { "frog"                 , Type::frog                 },
        { "grass_0"              , Type::grass_0              },
        { "grass_0_L_1x1"        , Type::grass_0_L_1x1        },
        { "grass_0_R_1x1"        , Type::grass_0_R_1x1        },
        { "grass_0_L_2x1_0"      , Type::grass_0_L_2x1_0      },
        { "grass_0_L_2x1_1"      , Type::grass_0_L_2x1_1      },
        { "grass_0_R_2x1_0"      , Type::grass_0_R_2x1_0      },
        { "grass_0_R_2x1_1"      , Type::grass_0_R_2x1_1      },
        { "grass_1"              , Type::grass_1              },
        { "grass_1_L_1x1"        , Type::grass_1_L_1x1        },
        { "grass_1_R_1x1"        , Type::grass_1_R_1x1        },
        { "grass_1_L_2x1_0"      , Type::grass_1_L_2x1_0      },
        { "grass_1_L_2x1_1"      , Type::grass_1_L_2x1_1      },
        { "grass_1_R_2x1_0"      , Type::grass_1_R_2x1_0      },
        { "grass_1_R_2x1_1"      , Type::grass_1_R_2x1_1      },
        { "grass_2"              , Type::grass_2              },
        { "grass_2_L_1x1"        , Type::grass_2_L_1x1        },
        { "grass_2_R_1x1"        , Type::grass_2_R_1x1        },
        { "grass_2_L_2x1_0"      , Type::grass_2_L_2x1_0      },
        { "grass_2_L_2x1_1"      , Type::grass_2_L_2x1_1      },
        { "grass_2_R_2x1_0"      , Type::grass_2_R_2x1_0      },
        { "grass_2_R_2x1_1"      , Type::grass_2_R_2x1_1      },
        { "level_center"         , Type::level_center         },
        { "level_L_0"            , Type::level_L_0            },
        { "level_R_0"            , Type::level_R_0            },
        { "logic_and_UD"         , Type::logic_and_UD         },
        { "logic_and_LR"         , Type::logic_and_LR         },
        { "logic_not_U"          , Type::logic_not_U          },
        { "logic_not_D"          , Type::logic_not_D          },
        { "logic_not_L"          , Type::logic_not_L          },
        { "logic_not_R"          , Type::logic_not_R          },
        { "logic_or_UD"          , Type::logic_or_UD          },
        { "logic_or_LR"          , Type::logic_or_LR          },
        { "mole"                 , Type::mole                 },
        { "particle_brick"       , Type::particle_brick       },
        { "particle_bubble"      , Type::particle_bubble      },
        { "particle_down_thrust" , Type::particle_down_thrust },
        { "particle_drop"        , Type::particle_drop        },
        { "particle_health"      , Type::particle_health      },
        { "particle_interact"    , Type::particle_interact    },
        { "particle_melee"       , Type::particle_melee       },
        { "particle_sense"       , Type::particle_sense       },
        { "particle_shot"        , Type::particle_shot        },
        { "player"               , Type::player               },
        { "slope_L_1x1"          , Type::slope_L_1x1          },
        { "slope_L_2x1_0"        , Type::slope_L_2x1_0        },
        { "slope_L_2x1_1"        , Type::slope_L_2x1_1        },
        { "slope_R_1x1"          , Type::slope_R_1x1          },
        { "slope_R_2x1_0"        , Type::slope_R_2x1_0        },
        { "slope_R_2x1_1"        , Type::slope_R_2x1_1        },
        { "spring_U"             , Type::spring_U             },
        { "slope_U"              , Type::slope_U              },
        { "sling"                , Type::sling                },
        { "train"                , Type::train                },
        { "track_trigger_UL"     , Type::track_trigger_UL     },
        { "track_trigger_UR"     , Type::track_trigger_UR     },        
        { "track_UD"             , Type::track_UD             },
        { "track_LR"             , Type::track_LR             },
        { "track_corner_UL"      , Type::track_corner_UL      },
        { "track_corner_UR"      , Type::track_corner_UR      },
        { "track_corner_DL"      , Type::track_corner_DL      },
        { "track_corner_DR"      , Type::track_corner_DR      },
        { "track_L_1x1_0"        , Type::track_L_1x1_0        },
        { "track_L_1x1_1"        , Type::track_L_1x1_1        },
        { "track_R_1x1_0"        , Type::track_R_1x1_0        },
        { "track_R_1x1_1"        , Type::track_R_1x1_1        },
        { "track_L_1x2_0"        , Type::track_L_1x2_0        },
        { "track_L_1x2_1"        , Type::track_L_1x2_1        },
        { "track_R_1x2_0"        , Type::track_R_1x2_0        },
        { "track_R_1x2_1"        , Type::track_R_1x2_1        },
        { "track_L_2x1_0"        , Type::track_L_2x1_0        },
        { "track_L_2x1_1"        , Type::track_L_2x1_1        },
        { "track_R_2x1_0"        , Type::track_R_2x1_0        },
        { "track_R_2x1_1"        , Type::track_R_2x1_1        },
        { "trigger"              , Type::trigger              },
        { "water_line"           , Type::water_line           },
        { "water_line_L"         , Type::water_line_L         },
        { "water_line_R"         , Type::water_line_R         }
    };
    cType string_to_type(const std::string& s) {
        if (string_to_entity_type_map.find(s) != string_to_entity_type_map.end()) {
            return string_to_entity_type_map.at(s);
        }
        return Type::none;
    }
    const std::string to_string(cType type) {        
        switch (type) {
            case Type::arch_L_1x1:           return "arch_L_1x1";
            case Type::arch_R_1x1:           return "arch_R_1x1";
            case Type::arch_L_2x1_0:         return "arch_L_2x1_0";
            case Type::arch_L_2x1_1:         return "arch_L_2x1_1";
            case Type::arch_R_2x1_0:         return "arch_R_2x1_0";
            case Type::arch_R_2x1_1:         return "arch_R_2x1_1";
            case Type::brick:                return "brick";
            case Type::bridge:               return "bridge";
            case Type::bug:                  return "bug";
            case Type::clip:                 return "clip";
            case Type::clip_L:               return "clip_L";
            case Type::clip_R:               return "clip_R";
            case Type::clip_D:               return "clip_D";
            case Type::clip_LD:              return "clip_LD";
            case Type::clip_RD:              return "clip_RD";
            case Type::clip_ledge:           return "clip_ledge";
            case Type::clip_U:               return "clip_U";
            case Type::coin_0:               return "coin_0";
            case Type::conduit_trigger_UL:   return "conduit_trigger_UL";
            case Type::conduit_trigger_UR:   return "conduit_trigger_UR";
            case Type::conduit_UD:           return "conduit_UD";
            case Type::conduit_LR:           return "conduit_LR";
            case Type::conduit_corner_UL:    return "conduit_corner_UL";
            case Type::conduit_corner_UR:    return "conduit_corner_UR";
            case Type::conduit_corner_DL:    return "conduit_corner_DL";
            case Type::conduit_corner_DR:    return "conduit_corner_DR";
            case Type::conduit_L_1x1_0:      return "conduit_L_1x1_0";
            case Type::conduit_L_1x1_1:      return "conduit_L_1x1_1";
            case Type::conduit_R_1x1_0:      return "conduit_R_1x1_0";
            case Type::conduit_R_1x1_1:      return "conduit_R_1x1_1";
            case Type::conduit_L_1x2_0:      return "conduit_L_1x2_0";
            case Type::conduit_L_1x2_1:      return "conduit_L_1x2_1";
            case Type::conduit_R_1x2_0:      return "conduit_R_1x2_0";
            case Type::conduit_R_1x2_1:      return "conduit_R_1x2_1";
            case Type::conduit_L_2x1_0:      return "conduit_L_2x1_0";
            case Type::conduit_L_2x1_1:      return "conduit_L_2x1_1";
            case Type::conduit_R_2x1_0:      return "conduit_R_2x1_0";
            case Type::conduit_R_2x1_1:      return "conduit_R_2x1_1";
            case Type::frog:                 return "frog";
            case Type::grass_0:              return "grass_0";
            case Type::grass_0_L_1x1:        return "grass_0_L_1x1";
            case Type::grass_0_R_1x1:        return "grass_0_R_1x1";
            case Type::grass_0_L_2x1_0:      return "grass_0_L_2x1_0";
            case Type::grass_0_L_2x1_1:      return "grass_0_L_2x1_1";
            case Type::grass_0_R_2x1_0:      return "grass_0_R_2x1_0";
            case Type::grass_0_R_2x1_1:      return "grass_0_R_2x1_1";
            case Type::grass_1:              return "grass_1";
            case Type::grass_1_L_1x1:        return "grass_1_L_1x1";
            case Type::grass_1_R_1x1:        return "grass_1_R_1x1";
            case Type::grass_1_L_2x1_0:      return "grass_1_L_2x1_0";
            case Type::grass_1_L_2x1_1:      return "grass_1_L_2x1_1";
            case Type::grass_1_R_2x1_0:      return "grass_1_R_2x1_0";
            case Type::grass_1_R_2x1_1:      return "grass_1_R_2x1_1";
            case Type::grass_2:              return "grass_2";
            case Type::grass_2_L_1x1:        return "grass_2_L_1x1";
            case Type::grass_2_R_1x1:        return "grass_2_R_1x1";
            case Type::grass_2_L_2x1_0:      return "grass_2_L_2x1_0";
            case Type::grass_2_L_2x1_1:      return "grass_2_L_2x1_1";
            case Type::grass_2_R_2x1_0:      return "grass_2_R_2x1_0";
            case Type::grass_2_R_2x1_1:      return "grass_2_R_2x1_1";
            case Type::level_center:         return "level_center";
            case Type::level_L_0:            return "level_L_0";
            case Type::level_R_0:            return "level_R_0";
            case Type::logic_and_UD:         return "logic_and_UD";
            case Type::logic_and_LR:         return "logic_and_LR";
            case Type::logic_not_U:          return "logic_not_U";
            case Type::logic_not_D:          return "logic_not_D";
            case Type::logic_not_L:          return "logic_not_L";
            case Type::logic_not_R:          return "logic_not_R";
            case Type::logic_or_UD:          return "logic_or_UD";
            case Type::logic_or_LR:          return "logic_or_LR";
            case Type::mole:                 return "mole";
            case Type::particle_brick:       return "particle_brick";
            case Type::particle_bubble:      return "particle_bubble";
            case Type::particle_down_thrust: return "particle_down_thrust";
            case Type::particle_drop:        return "particle_drop";
            case Type::particle_health:      return "particle_health";
            case Type::particle_interact:    return "particle_interact";
            case Type::particle_melee:       return "particle_melee";
            case Type::particle_sense:       return "particle_sense";
            case Type::particle_shot:        return "particle_shot";
            case Type::player:               return "player";
            case Type::slope_L_1x1:          return "slope_L_1x1";
            case Type::slope_R_1x1:          return "slope_R_1x1";
            case Type::slope_L_2x1_0:        return "slope_L_2x1_0";
            case Type::slope_L_2x1_1:        return "slope_L_2x1_1";
            case Type::slope_R_2x1_0:        return "slope_R_2x1_0";
            case Type::slope_R_2x1_1:        return "slope_R_2x1_1";
            case Type::slope_U:              return "slope_U";
            case Type::spring_U:             return "spring_U";
            case Type::sling:                return "sling";
            case Type::track_trigger_UL:     return "track_trigger_UL";
            case Type::track_trigger_UR:     return "track_trigger_UR";
            case Type::track_UD:             return "track_UD";
            case Type::track_LR:             return "track_LR";
            case Type::track_corner_UL:      return "track_corner_UL";
            case Type::track_corner_UR:      return "track_corner_UR";
            case Type::track_corner_DL:      return "track_corner_DL";
            case Type::track_corner_DR:      return "track_corner_DR";
            case Type::track_L_1x1_0:        return "track_L_1x1_0";
            case Type::track_L_1x1_1:        return "track_L_1x1_1";
            case Type::track_R_1x1_0:        return "track_R_1x1_0";
            case Type::track_R_1x1_1:        return "track_R_1x1_1";
            case Type::track_L_1x2_0:        return "track_L_1x2_0";
            case Type::track_L_1x2_1:        return "track_L_1x2_1";
            case Type::track_R_1x2_0:        return "track_R_1x2_0";
            case Type::track_R_1x2_1:        return "track_R_1x2_1";
            case Type::track_L_2x1_0:        return "track_L_2x1_0";
            case Type::track_L_2x1_1:        return "track_L_2x1_1";
            case Type::track_R_2x1_0:        return "track_R_2x1_0";
            case Type::track_R_2x1_1:        return "track_R_2x1_1";
            case Type::train:                return "train";
            case Type::trigger:              return "trigger";
            case Type::water_line:           return "water_line";
            case Type::water_line_L:         return "water_line_L";
            case Type::water_line_R:         return "water_line_R";
            default:                         return "";
        }
    }
    enum class State {
        none = 0,
        blocked, bounce,
        carried,
        dead, dive,
        enter, exit,
        heal, hurt,
        idle,
        jump,
        ledge,
        melee,
        run,
        shoot, stunned, swim, sling,
        tossed,
        upended,
        walk
    };
    using cState = const State;
    const std::string to_string(cState s) {
        switch (s) {
        case State::blocked:    return "blocked";
        case State::bounce:     return "bounce";
        case State::carried:    return "carried";
        case State::dead:       return "dead";
        case State::dive:       return "dive";
        case State::enter:      return "enter";
        case State::exit:       return "exit";
        case State::heal:       return "heal";
        case State::hurt:       return "hurt";
        case State::idle:       return "idle";
        case State::jump:       return "jump";
        case State::ledge:      return "ledge";
        case State::melee:      return "melee";
        case State::run:        return "run";
        case State::shoot:      return "shoot";
        case State::stunned:    return "stunned";
        case State::swim:       return "swim";
        case State::sling:      return "sling";
        case State::tossed:     return "tossed";
        case State::upended:    return "upended";
        case State::walk:       return "walk";
        default:                return "";
        }
    }
}

export namespace input {
    enum class Key : I32 {
        none = -1,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9, num_0,
        enter = 36, esc, backspace, tab, space, subtract, equal,
        grave = 48, comma, period, apostrophe, slash,
        f1 = 52, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12,
        del = 83,
        right = 86, left, down, up,
        ctrl = 127, shift, alt
    };
    enum class Button : I32 {
        none = -1,
        left, right, middle
    };
    I32 alphabet_begin() { return (I32)Key::a;         }
    I32 alphabet_end()   { return (I32)Key::z + 1;     }
    I32 numbers_begin()  { return (I32)Key::num_1;     }
    I32 numbers_end()    { return (I32)Key::num_0 + 1; }
    I32 inputs_begin()   { return (I32)Key::a;         }
    I32 inputs_end()     { return (I32)Key::alt + 1;   }

    const std::string string_from(Key key, bool is_shift) {
        switch (key) {
        case Key::a:          return is_shift ? "A" : "a";
        case Key::b:          return is_shift ? "B" : "b";
        case Key::c:          return is_shift ? "C" : "c";
        case Key::d:          return is_shift ? "D" : "d";
        case Key::e:          return is_shift ? "E" : "e";
        case Key::f:          return is_shift ? "F" : "f";
        case Key::g:          return is_shift ? "G" : "g";
        case Key::h:          return is_shift ? "H" : "h";
        case Key::i:          return is_shift ? "I" : "i";
        case Key::j:          return is_shift ? "J" : "j";
        case Key::k:          return is_shift ? "K" : "k";
        case Key::l:          return is_shift ? "L" : "l";
        case Key::m:          return is_shift ? "M" : "m";
        case Key::n:          return is_shift ? "N" : "n";
        case Key::o:          return is_shift ? "O" : "o";
        case Key::p:          return is_shift ? "P" : "p";
        case Key::q:          return is_shift ? "Q" : "q";
        case Key::r:          return is_shift ? "R" : "r";
        case Key::s:          return is_shift ? "S" : "s";
        case Key::t:          return is_shift ? "T" : "t";
        case Key::u:          return is_shift ? "U" : "u";
        case Key::v:          return is_shift ? "V" : "v";
        case Key::w:          return is_shift ? "W" : "w";
        case Key::x:          return is_shift ? "X" : "x";
        case Key::y:          return is_shift ? "Y" : "y";
        case Key::z:          return is_shift ? "Z" : "z";
        case Key::num_0:      return is_shift ? ")" : "0";
        case Key::num_1:      return is_shift ? "!" : "1";
        case Key::num_2:      return is_shift ? "@" : "2";
        case Key::num_3:      return is_shift ? "#" : "3";
        case Key::num_4:      return is_shift ? "$" : "4";
        case Key::num_5:      return is_shift ? "%" : "5";
        case Key::num_6:      return is_shift ? "^" : "6";
        case Key::num_7:      return is_shift ? "&" : "7";
        case Key::num_8:      return is_shift ? "*" : "8";
        case Key::num_9:      return is_shift ? "(" : "9";
        case Key::subtract:   return is_shift ? "_" : "-";
        case Key::equal:      return is_shift ? "+" : "=";
        case Key::grave:      return is_shift ? "~" : "`";
        case Key::comma:      return is_shift ? "<" : ",";
        case Key::period:     return is_shift ? ">" : ".";
        case Key::apostrophe: return is_shift ? "\"" : "'";
        case Key::slash:      return is_shift ? "?" : "/";
        default:              return "";
        }
    }
    const std::string string_from(Key key) {
        return string_from(key, false);
    }
}

export namespace state {
    enum class Type {
        none = 0, edit, game, menu, overlay
    };
    using cType = const Type;
    Type type_from(const std::string_view& s) {
        if      (s == "edit")    return Type::edit;
        else if (s == "game")    return Type::game;
        else if (s == "menu")    return Type::menu;
        else if (s == "overlay") return Type::overlay;
        return Type::none;
    }
    const std::string to_string(cType t) {
        switch (t) {
        case Type::edit:    return "edit";    break;
        case Type::game:    return "game";    break;
        case Type::menu:    return "menu";    break;
        case Type::overlay: return "overlay"; break;
        default:            return "";        break;
        }
    }
}
export namespace start {
    enum class Type { none = 0, center, L_0, R_0 };
    using cType = const Type;
    struct Info {
        Type type   = Type::center;
        U8   number = 0;
    };
    using cInfo = const Info;
    Type from_string(const char* s) {
        if      (s == "center") return Type::center;
        else if (s == "L_0")    return Type::L_0;
        else if (s == "R_0")    return Type::R_0;
        else                    return Type::none;
    }
    const std::string to_string(cType type) {
        switch (type) {
        case Type::center:  return "center";
        case Type::L_0:     return "L_0";
        case Type::R_0:     return "R_0";
        default:            return ""; }
    }
}

export namespace particle {
    enum class Type {
        none = 0,
        brick, bubble,
        down_thrust, drop, drop_blood, dust, dust_L, dust_R,
        health, hit,
        interact,
        melee,
        sense, shot,
        tile_line
    };
    using cType = const Type;
    const std::string to_string(cType t) {
        switch (t) {
        case Type::brick:       return "brick";
        case Type::bubble:      return "bubble";
        case Type::down_thrust: return "down_thrust";
        case Type::drop:        return "drop";
        case Type::drop_blood:  return "drop_blood";
        case Type::dust:        return "dust";
        case Type::dust_L:      return "dust_L";
        case Type::dust_R:      return "dust_R";
        case Type::health:      return "health";
        case Type::hit:         return "hit";
        case Type::interact:    return "interact";
        case Type::melee:       return "melee";
        case Type::sense:       return "sense";
        case Type::shot:        return "shot";
        case Type::tile_line:   return "tile_line";
        default:                return "";
        }
    }
}


export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Rect {
    T x = 0, y = 0, w = 0, h = 0;
    Rect() {}
    Rect(T in_x, T in_y, T in_w, T in_h) : x(in_x), y(in_y), w(in_w), h(in_h) {}
    Rect& operator =(const Rect& other) { x = other.x, y = other.y, w = other.w, h = other.h; return *this; }    
    template<typename T>
    Rect& operator *=(const T scalar) { x *= scalar, y *= scalar, w *= scalar, h *= scalar; return *this; }
    bool operator ==(const Rect& other) const { return (x == other.x && y == other.y && w == other.w && h == other.h); }
    bool operator !=(const Rect& other) const { return !operator==(other); }

    Rect operator +(const Rect& other) const { return { x + other.x, y + other.y, w + other.w, h + other.h }; }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec2 {
    T x = 0, y = 0;
    Vec2() {}
    Vec2(T in_x, T in_y) : x(in_x), y(in_y) {}
    Vec2& operator =(const Vec2& other) { x = other.x, y = other.y;   return *this; }
    Vec2& operator =(const T value) { x = value, y = value;   return *this; }
    Vec2& operator +=(const Vec2& other) { x += other.x, y += other.y; return *this; }
    Vec2& operator -=(const Vec2& other) { x -= other.x, y -= other.y; return *this; }
    Vec2& operator *=(const Vec2& other) { x *= other.x, y *= other.y; return *this; }
    Vec2& operator *=(const T value) { x *= value, y *= value; return *this; }
    Vec2 operator +(const Vec2& other)  const { return { x + other.x, y + other.y }; }
    Vec2 operator -(const Vec2& other)  const { return { x - other.x, y - other.y }; }
    Vec2 operator -()                   const { return { -x, -y }; }
    Vec2 operator *(const Vec2& other)  const { return { x * other.x, y * other.y }; }
    template<typename T>
    Vec2 operator *(const T scalar)     const { return { x * scalar, y * scalar }; }
    template<typename T>
    Vec2 operator /(const T scalar)     const { return { x / scalar, y / scalar }; }
    bool operator ==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator !=(const Vec2& other) const { return !operator==(other); }
    template<typename T>
    bool operator ==(const T value)     const { return x == value && y == value; }
    template<typename T>
    bool operator !=(const T value)     const { return x != value && y != value; }
    bool operator <(const Vec2& other)  const { return x < other.x && y < other.y; }
};

export template<typename T> requires std::integral<T> || std::floating_point<T>
struct Vec3 {
    T x = 0, y = 0, z = 0;
    Vec3() {}
    Vec3(T in_x, T in_y, T in_z) : x(in_x), y(in_y), z(in_z) {}
    Vec3& operator  =(const Vec3& other) { x = other.x, y = other.y, z = other.z;    return *this; }
    Vec3& operator  =(const T value) { x = value, y = value, z = value;          return *this; }
    Vec3& operator +=(const Vec3& other) { x += other.x, y += other.y, z += other.z; return *this; }
    Vec3& operator -=(const Vec3& other) { x -= other.x, y -= other.y, z -= other.z; return *this; }
    Vec3& operator *=(const Vec3& other) { x *= other.x, y *= other.y, z *= other.z; return *this; }
    Vec3  operator  +(const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vec3  operator  -(const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vec3  operator  -()                  const { return { -x, -y, -z }; }
    Vec3  operator  *(const Vec3& other) const { return { x * other.x, y * other.y, z * other.z }; }
    template<typename T>
    Vec3  operator  *(const T scalar)    const { return { x * scalar, y * scalar, z * scalar }; }
    template<typename T>
    Vec3  operator  /(const T scalar)    const { return { x / scalar, y / scalar, z / scalar }; }
    bool  operator ==(const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool  operator !=(const Vec3& other) const { return !operator==(other); }
    template<typename T>
    bool  operator ==(const T value)     const { return x == value && y == value && z == value; }
    template<typename T>
    bool  operator !=(const T value)     const { return !operator==(value); }
    bool  operator  <(const Vec3& other) const { return x < other.x && y < other.y && z < other.z; }
};

export using  Vec2I =       Vec2<I32>;
export using cVec2I = const Vec2<I32>;
export using  Vec2U =       Vec2<U32>;
export using cVec2U = const Vec2<U32>;
export using  Vec2F =       Vec2<F32>;
export using cVec2F = const Vec2<F32>;

export using  Vec3I =       Vec3<I32>;
export using cVec3I = const Vec3<I32>;
export using  Vec3U =       Vec3<U32>;
export using cVec3U = const Vec3<U32>;
export using  Vec3F =       Vec3<F32>;
export using cVec3F = const Vec3<F32>;

export using  RectI =       Rect<I32>;
export using cRectI = const Rect<I32>;
export using  RectU =       Rect<U32>;
export using cRectU = const Rect<U32>;
export using  RectF =       Rect<F32>;
export using cRectF = const Rect<F32>;


export struct Color {
    U8 r = 127, g = 127, b = 127;
    bool operator ==(const Color& other) { return (r == other.r && g == other.g && b == other.b); }
    bool operator !=(const Color& other) { return !operator==(other); }
    template<typename T>
    Color operator /(const T scalar) const {
        F32 float_scalar = scalar;
        Vec3F float_color = { r / float_scalar, g / float_scalar, b / float_scalar };
                
        return { (U8)float_color.x, (U8)float_color.y, (U8)float_color.z };
    }

    Color& operator =(const Color& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        return *this;
    }
};
using cColor = const Color;