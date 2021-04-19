#pragma once
#include <M5StickCPlus.h>

namespace TASKS
{
    class Dimmer
    {
    public:
        Dimmer();
        void go_dim();
        void go_dark();
        void recover();
        bool dim_exitting();
        bool is_dim();
    private:
        uint8_t brightness;
        bool dimmed, asleep;
        unsigned long dim_exit_time;
    };

    int8_t get_batt_percentage();
};
