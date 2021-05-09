#include <M5StickCPlus.h>
#include "tasks.h"
#include <utility/MPU6886.h>

static const char *TAG = "TASKS_UTIL";

namespace TASKS
{

    Dimmer::Dimmer()
    {
        dimmed = false;
        asleep = false;
    }

    void Dimmer::go_dim()
    {
        if (dimmed)
        {
            return;
        }
        brightness = M5.Axp.Read8bit(0x28) >> 4;
        dimmed = true;
        M5.Axp.ScreenBreath(7);
    }

    void Dimmer::go_dark()
    {
        dimmed = true;
        asleep = true;
        M5.Axp.Write1Byte(0x12, 0b01001011);
    }

    void Dimmer::recover()
    {
        if (!(dimmed || asleep))
        {
            return;
        }
        if (asleep)
        {
            M5.Axp.Write1Byte(0x12, 0x4d);
        }
        M5.Axp.ScreenBreath(brightness);
        dimmed = false;
        asleep = false;
        dim_exit_time = millis();
    }

    bool Dimmer::dim_exitting()
    {
        return millis() - dim_exit_time < 500;
    }

    bool Dimmer::is_dim()
    {
        return dimmed || asleep;
    }

    int8_t get_batt_percentage()
    {
        float volt = M5.Axp.GetBatVoltage();

        if (volt < 3.20)
            return -1;
        if (volt < 3.27)
            return 0;
        if (volt < 3.61)
            return 5;
        if (volt < 3.69)
            return 10;
        if (volt < 3.71)
            return 15;
        if (volt < 3.73)
            return 20;
        if (volt < 3.75)
            return 25;
        if (volt < 3.77)
            return 30;
        if (volt < 3.79)
            return 35;
        if (volt < 3.80)
            return 40;
        if (volt < 3.82)
            return 45;
        if (volt < 3.84)
            return 50;
        if (volt < 3.85)
            return 55;
        if (volt < 3.87)
            return 60;
        if (volt < 3.91)
            return 65;
        if (volt < 3.95)
            return 70;
        if (volt < 3.98)
            return 75;
        if (volt < 4.02)
            return 80;
        if (volt < 4.08)
            return 85;
        if (volt < 4.11)
            return 90;
        if (volt < 4.15)
            return 95;
        if (volt < 4.20)
            return 100;
        if (volt >= 4.20)
            return 101;
        return 0;
    }

    IMUManager::IMUManager()
    {
        M5.Imu.getAccelData(&aX, &aY, &aZ);
        M5.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);
    }

    bool IMUManager::is_moved()
    {
        M5.Imu.getAccelData(&next_aX, &next_aY, &next_aZ);

        if (abs(next_aX - aX) > 0.5 || abs(next_aY - aY) > 0.5 || abs(next_aZ - aZ) > 0.5)
        {
            aX = next_aX;
            aY = next_aY;
            aZ = next_aZ;
            return true;
        }
        return false;
    }
};