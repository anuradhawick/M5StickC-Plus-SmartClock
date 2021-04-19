#pragma once

#include <M5StickCPlus.h>
#include "View.h"
#include "fonts/7seg20.h"
#include "fonts/7seg70.h"
#include "data.h"

#define grey 0x65DB

class TimeView : public View
{
public:
    TimeView();
    ~TimeView();

    void render();
    bool receive_event(EVENTS::event event){ return false; };

private:
    uint8_t h_cache = 25;
    uint8_t m_cache = 60;
    uint8_t s_cache = 60;
    uint8_t d_cache = 10;
    uint8_t dt_cache = 35;
    uint8_t mn_cache = 35;
    uint16_t yr_cache = 0;

    char timeHHMM[5];
    char seconds[2];
    char day[10];

    TFT_eSprite *disp_buffer;
};
