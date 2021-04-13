#pragma once

#include "View.h"

class AnalogClock : public View
{
public:
    AnalogClock();
    ~AnalogClock();

    void render();

private:
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    unsigned long last_render;
    int16_t origin_x, origin_y;
    int16_t seconds_angle, minutes_angle, hours_angle;
    int16_t seconds_hand_width = 2, minutes_hand_width = 4, hours_hand_width = 8;
    int16_t seconds_hand_length = 63, minutes_hand_length = 58, hours_hand_length = 50;

    String days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    uint8_t dt_cache = 35;
    uint8_t mn_cache = 35;
    uint16_t yr_cache = 0;
    uint8_t d_cache = 10;
    uint8_t h_cache = 25;
    uint8_t m_cache = 60;
    uint8_t s_cache = 60;
    char dateDDMMYYYY[10];
    char day[10];

    void render_clock_face();
    void render_date();

    TFT_eSprite *disp_buffer;
};
