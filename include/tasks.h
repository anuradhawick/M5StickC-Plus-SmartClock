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

    class IMUManager
    {
    public:
        IMUManager();
        bool is_moved();

    private:
        float aX = 0.0F, aY = 0.0F, aZ = 0.0F, next_aX = 0.0F, next_aY = 0.0F, next_aZ = 0.0F;
    };

    int8_t get_batt_percentage();
    void update_imu();
    bool is_moved();
};
