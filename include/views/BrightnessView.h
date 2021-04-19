#pragma once

#include <M5StickCPlus.h>
#include "View.h"

#define grey 0x65DB

class BrightnessView : public View
{
public:
    BrightnessView();
    ~BrightnessView();

    void render();
    bool receive_event(EVENTS::event event);

private:
    TFT_eSprite *disp_buffer;
    uint8_t brightness;
    float rendering_brightness_value;
    String header_message;
    unsigned long last_rendered;
};
