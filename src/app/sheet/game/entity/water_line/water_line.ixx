export module entity.water_line;
import anim;
import console;
import entity;
import aabb;
import line;
import random;
import std;

export namespace entity {
    class WaterLine : public Object {
        F32 m_line_offset_y   =  3.0F;
        I32 m_top_line     = -1,
            m_bottom_line  = -1;
        U8 m_wave_timer       =  0,
           m_top_line_size    =  3,
           m_bottom_line_size =  3;

    public:
        WaterLine() {
            m_input_limit = 2;
        }
        ~WaterLine() {            
            line::erase(m_top_line);
            line::erase(m_bottom_line);
        }

        const char* class_name() override { return "entity::WaterLine"; }

        void collide_x(aabb::cInfo our, aabb::cInfo other) override;
        void collide_y(aabb::cInfo our, aabb::cInfo other) override;

        virtual void draw(std::unique_ptr<Window>& window) override {
            line::draw(window, m_top_line);
            //line::draw(window, m_bottom_line);
        }

        void update() override;
    };
}