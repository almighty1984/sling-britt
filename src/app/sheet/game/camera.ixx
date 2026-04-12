export module camera;
import console;
import random;
import transform;
import types;
import std;

static inline std::set<I32> s_transforms;

static inline Vec2F s_shake_min = {},
                    s_shake_max = {};
static inline F32 s_shake_dec     = 0.0F,
                  s_sum_shake_dec = 0.0F;

static inline U16 s_time_left_shaking = 0,
                  s_time_to_shake     = 0;
static inline U8 s_shake_time_spacing = 0;

static inline U16 s_time_focus_idle   = 0,
                  s_time_focus_idle_until_recenter = 100;


export namespace camera {
    I32   focus_transform = -1;
    Vec2F focal_point      = { 160.0F, 90.0F };
    Vec2F speed            = { 0.0625F, 0.0625F };
    Vec2F position         = { 0.0F, 0.0F };
    Vec2F difference       = { 0.0F, 0.0F };

    Vec2I level_scroll_limit = { U16_MAX, U16_MAX };

    Vec2F focus_offset = { 0.0F, 0.0F };

    F32  direction_offset_amount = 16.0F;

    bool is_idle_recentered = true;
    bool is_offset_with_direction = false;

    void add_transform(cI32 id) {
        //console::log("camera::add_transform: ", id, "\n");
        s_transforms.insert(id);
    }
    void remove_transform(cI32 id) {
        s_transforms.erase(id);
    }
    void clear() {
        focus_transform = -1;
        s_transforms.clear();
        position = { 0.0F, 0.0F };
        difference = { 0.0F, 0.0F };
        level_scroll_limit = { U16_MAX, U16_MAX };
    }
    void set_position(cVec2F p) {
        position = p;
        if (position.x < 0.0F) {
            position.x = 0.0F;
        }
        else if (position.x > level_scroll_limit.x) {
            position.x = level_scroll_limit.x;
        }
        if (position.y < 0.0F) {
            position.y = 0.0F;
        }
        else if (position.y > level_scroll_limit.y) {
            position.y = level_scroll_limit.y;
        }

        for (auto& i : s_transforms) {
            transform::position(i, transform::position(i) - position);
            transform::start_position(i, transform::start_position(i) - position);
        }
    }
    void init_shake(cU16 duration, cU8 time_spacing, cVec2F min, cVec2F max, cF32 dec) {
        s_time_left_shaking = duration;
        s_time_to_shake = duration;
        s_shake_min = min;
        s_shake_max = max;
        s_shake_dec = dec;
        s_sum_shake_dec = 0.0F;
        s_shake_time_spacing = time_spacing;
    }

    //bool dir_x = false;
    //bool dir_y = false;
    void shake() {
        //console::log("camera::shake()\n");
        //if (shake_dec >= 1.0F) return;
        
        /*cF32 random_x = random::number(-shake_max.x, shake_max.x);
        cF32 random_y = random::number(-shake_max.y, shake_max.y);

        focus_offset.x = dir_x ? random_x : -random_x;
        focus_offset.y = dir_y ? random_y : -random_y;*/

        //dir_x = !dir_x;
        //dir_y = !dir_y;
        
        
        cI32 random_x = random::number((U16)s_shake_min.x, (U16)s_shake_max.x);
        cI32 random_y = random::number((U16)s_shake_min.y, (U16)s_shake_max.y);
        if (s_time_left_shaking == s_time_to_shake) {
            focus_offset.x = random::number(0, 1) ? random_x : -random_x;
            focus_offset.y = random::number(0, 1) ? random_y : -random_y;
        } else {
            focus_offset.x = random::number(0, 1) ? random_x : -random_x;
            focus_offset.y = random::number(0, 1) ? random_y : -random_y;
        }

        s_shake_min.x *= (1.0F - s_sum_shake_dec);
        s_shake_max.x *= (1.0F - s_sum_shake_dec);
        s_shake_min.y *= (1.0F - s_sum_shake_dec);
        s_shake_max.y *= (1.0F - s_sum_shake_dec);

        console::log("camera::shake() sum_shake_dec: ", s_sum_shake_dec, "\n");
        s_sum_shake_dec += s_shake_dec;
        if (s_sum_shake_dec > 1.0F) {
            s_sum_shake_dec = 1.0F;
        }

        //focus_offset.x = random::number(0, 1) ? -16.0F : 16.0F;
        //focus_offset.y = random::number(0, 1) ? -16.0F : 16.0F;
        //focus_offset.x = random::number(0, 1) ? focus_offset.x - 16.0F : focus_offset.x + 16.0F;
        //focus_offset.y = random::number(0, 1) ? focus_offset.y - 16.0F : focus_offset.y + 16.0F;
        //focus_offset.x = focus_offset.x > 0.0F ? -16.0F : 16.0F;
        //focus_offset.y = focus_offset.y > 0.0F ? -16.0F : 16.0F;
    }
    void update(cRectU view) {
        if (focus_transform == -1) return;
        
        cF32 direction_acc = 0.75F,
             recenter_acc  = 0.25F;

        // move camera left or right to see ahead
        if (transform::velocity(focus_transform).x < 0.0F or (is_offset_with_direction and transform::direction(focus_transform).x < 0.0F)) {
            s_time_focus_idle = 0;
            if (direction_offset_amount > 0.0F and focus_offset.x > -direction_offset_amount) {
                focus_offset.x -= direction_acc;
            }
        }
        else if (transform::velocity(focus_transform).x > 0.0F or (is_offset_with_direction and transform::direction(focus_transform).x > 0.0F)) {
            s_time_focus_idle = 0;
            if (direction_offset_amount > 0.0F and focus_offset.x < direction_offset_amount) {
                focus_offset.x += direction_acc;
            }
        }
        else if (is_idle_recentered) {
            ++s_time_focus_idle;
            if (s_time_focus_idle > s_time_focus_idle_until_recenter) {
                if (focus_offset.x < 0.0F) focus_offset.x += recenter_acc;
                else if (focus_offset.x > 0.0F) focus_offset.x -= recenter_acc;

                if (focus_offset.y < 0.0F) focus_offset.y += recenter_acc;
                else if (focus_offset.y > 0.0F) focus_offset.y -= recenter_acc;


                if (focus_offset.x > -recenter_acc and camera::focus_offset.x < recenter_acc and
                    focus_offset.y > -recenter_acc and camera::focus_offset.y < recenter_acc) {
                    focus_offset = {};
                    s_time_focus_idle = 0;
                }
            }
        }


        if (s_time_left_shaking > 0) {
            //console::log("camera::update() time left shaking: ", time_left_shaking, " ", shake_min.x, " ", shake_max.x, "\n");
            if (s_time_left_shaking % s_shake_time_spacing == 0) {
                shake();
            }
            --s_time_left_shaking;     
        }
        else {
            //focus_offset.y *= 0.5F;
        }

        if (focal_point.x < view.w / 2.0F - 80.0F) {
            focal_point.x = view.w / 2.0F - 80.0F;
        }
        else if (focal_point.x > view.w / 2.0F + 80.0F) {
            focal_point.x = view.w / 2.0F + 80.0F;
        }
        if (focal_point.y < view.h / 2.0F - 48.0F) {
            focal_point.y = view.h / 2.0F - 48.0F;
        }
        else if (focal_point.y > view.h / 2.0F + 48.0F) {
            focal_point.y = view.h / 2.0F + 48.0F;
        }

        //difference = { 0.0F, 0.0F };
        if (transform::position(focus_transform).x + focus_offset.x > focal_point.x or
            transform::position(focus_transform).x + focus_offset.x < focal_point.x) {
            difference.x = (transform::position(focus_transform).x + focus_offset.x - focal_point.x) * speed.x;

            if (difference.x < 0.0F and position.x <= 0.0F) {
                difference.x = -position.x;
            }
            else if (difference.x > 0.0F and position.x >= level_scroll_limit.x) {
                difference.x = level_scroll_limit.x - position.x;
            }
            else {
                // slow down at level edges
                if (difference.x < 0.0F and position.x < difference.x * -10.0F) {
                    difference.x = -position.x / 20.0F;
                }
                else if (difference.x > 0.0F and position.x > level_scroll_limit.x - difference.x * 10.0F) {
                    difference.x = -(position.x - level_scroll_limit.x) / 20.0F;
                }
            }
            for (auto& i : s_transforms) {
                transform::position(i, transform::position(i) - Vec2F{ difference.x, 0.0F });
                transform::start_position(i, transform::start_position(i) - Vec2F{ difference.x, 0.0F });
            }
            position.x += difference.x;
        }
        if (transform::position(focus_transform).y + focus_offset.y > focal_point.y or
            transform::position(focus_transform).y + focus_offset.y < focal_point.y) {
            difference.y = (transform::position(focus_transform).y + focus_offset.y - focal_point.y) * speed.y;

            if (difference.y < 0.0F and position.y <= 0.0F) {
                difference.y = -position.y;
            }
            else if (difference.y > 0.0F and position.y >= level_scroll_limit.y) {
                difference.y = level_scroll_limit.y - position.y;
            }
            else {
                // slow down at level edges
                if (difference.y < 0.0F and position.y < difference.y * -10.0F) {
                    difference.y = -position.y / 20.0F;
                }
                else if (difference.y > 0.0F and position.y > level_scroll_limit.y - difference.y * 10.0F) {
                    difference.y = -(position.y - level_scroll_limit.y) / 20.0F;
                }
            }
            for (auto& i : s_transforms) {
                transform::position(i, transform::position(i) - Vec2F{ 0.0F, difference.y });
                transform::start_position(i, transform::start_position(i) - Vec2F{ 0.0F, difference.y });
            }
            position.y += difference.y;
        }
    }
}