#pragma once

#include "View.h"
#include "tasks.h"
#include "data.h"

class AnalogClock : public View
{
public:
    AnalogClock();
    ~AnalogClock();

    void render();
    bool receive_event(EVENTS::event event){ return false; };

private:
    unsigned long last_render;
    int16_t origin_x, origin_y;
    int16_t seconds_angle, minutes_angle, hours_angle;
    int16_t seconds_hand_width = 2, minutes_hand_width = 4, hours_hand_width = 8;
    int16_t seconds_hand_length = 63, minutes_hand_length = 58, hours_hand_length = 50;

    String bat_power;
    int8_t bat_cache = -1;
    uint8_t dt_cache = 35;
    uint8_t mn_cache = 35;
    uint16_t yr_cache = 0;
    uint8_t d_cache = 0;
    uint8_t h_cache = 25;
    uint8_t m_cache = 60;
    uint8_t s_cache = 60;
    char dateDDMMYYYY[10];
    char day[10];

    void render_clock_face();
    void render_date();
    void render_batt_power();

    TFT_eSprite *disp_buffer;
};
