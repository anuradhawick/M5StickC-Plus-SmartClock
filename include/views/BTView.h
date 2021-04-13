#pragma once

#include <M5StickCPlus.h>
#include <BluetoothSerial.h>
#include "View.h"

class BTView : public View
{
public:
    BTView();
    ~BTView();

    void render();

private:
    void bt_loop();
    void render_loader();
    void render_text();

    unsigned long loader_last_rendered;
    unsigned long text_last_rendered;

    uint16_t loader_frame;
    String bt_incoming;
    String text;

    TFT_eSprite *disp_buffer;
    uint8_t main_menu = 0;

    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    bool bt_started;
    unsigned long inited_time;
};
