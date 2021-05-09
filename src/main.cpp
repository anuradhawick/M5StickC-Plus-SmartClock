#include <Arduino.h>
#include <M5StickCPlus.h>
#include "views/View.h"
#include "views/TimeView.h"
#include "views/BTView.h"
#include "views/AnalogClock.h"
#include "views/BrightnessView.h"
#include "tasks.h"

static const char *TAG = "MAIN";

View *active_view;
uint8_t state = 1;
unsigned long rst_clicked_time = millis();
unsigned long home_clicked_time = millis();
unsigned long active_time = millis();

TASKS::Dimmer dimmer;
TASKS::IMUManager imumgr;
int32_t flags = 0;

#define M5_BUTTON_RESET_PRESSED 0x01
#define M5_BUTTON_HOME_PRESSED 0x02
#define M5_BUTTON_POWER_PRESSED 0x04

void event_loop();

void reset_buttton_pressed()
{
    if (!(rst_clicked_time + 500 > millis()))
    {
        ESP_LOGD(TAG, "Reset Buttion Pressed");

        rst_clicked_time = millis();
        flags = flags | M5_BUTTON_RESET_PRESSED;
    }
}

void home_button_pressed()
{
    if (!(home_clicked_time + 500 > millis()))
    {
        ESP_LOGD(TAG, "Home Button Pressed");
        home_clicked_time = millis();
        flags = flags | M5_BUTTON_HOME_PRESSED;
    }
}

void power_button_pressed()
{
    ESP_LOGD(TAG, "Power Button Pressed");
    flags = flags | M5_BUTTON_POWER_PRESSED;
}

void setup()
{
    M5.begin();
    M5.Imu.Init();
    M5.Axp.ScreenBreath(9);

    ESP_LOGD(TAG, "%d", M5.Axp.Read8bit(0x28) >> 4);

    active_view = new AnalogClock();

    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
}

void loop()
{
    // render view
    active_view->render();

    event_loop();

    // no dimming as long as in shake
    if (imumgr.is_moved())
    {
        dimmer.recover();
        active_time = millis();
    }

    // auto dimming timed
    if (active_time + 5000 < millis())
    {
        dimmer.go_dim();
    }

    // auto screenoff timed
    if (active_time + 10000 < millis())
    {
        dimmer.go_dark();
    }

    // capture events
    if (digitalRead(M5_BUTTON_RST) == LOW)
    {
        reset_buttton_pressed();
    }

    if (M5.Axp.GetBtnPress() == 0x02)
    {
        power_button_pressed();
    }

    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {
        home_button_pressed();
    }

    // propagate events to view
    if (flags > 0)
    {
        if (dimmer.is_dim())
        {
            dimmer.recover();
            active_time = millis();
            flags = 0;
        }
        else if(!dimmer.dim_exitting())
        {
            event_loop();
            flags = 0;
        }
        else
        {
            flags = 0;
        }
    }
}

void event_loop()
{
    if (flags && M5_BUTTON_RESET_PRESSED)
    {
        active_view->receive_event(EVENTS::RESET_PRESSED);
    }

    if (flags && M5_BUTTON_POWER_PRESSED)
    {
        active_view->receive_event(EVENTS::POWER_PRESSED);
    }

    if (flags && M5_BUTTON_HOME_PRESSED)
    {
        state++;

        if (state > 4)
        {
            state = 1;
        }

        delete active_view;
        active_view = NULL;

        switch (state)
        {
        case 1:
            active_view = new AnalogClock();
            break;
        case 2:
            active_view = new TimeView();
            break;
        case 3:
            active_view = new BrightnessView();
            break;
        case 4:
            active_view = new BTView();
            break;

        default:
            break;
        }
    }
}
