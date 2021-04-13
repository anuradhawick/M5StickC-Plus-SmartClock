#include <Arduino.h>
#include <M5StickCPlus.h>
#include "views/View.h"
#include "views/TimeView.h"
#include "views/BTView.h"
#include "views/AnalogClock.h"

static const char *TAG = "MAIN";

View *active_view;
volatile uint8_t state = 1;
volatile uint8_t brightness = 9;
volatile bool brightness_changed = false;
volatile unsigned long brightness_changed_time = millis();
volatile bool state_changed = false;
volatile unsigned long state_changed_time = millis();

// int freq = 1800;
// int channel = 0;
// int resolution_bits = 8;
// int buzzer = 2;

void brightness_change_clicked()
{
    if (brightness_changed_time + 500 > millis())
    {
        return;
    }

    ESP_LOGD(TAG, "Brightness Change Clicked");

    brightness++;

    if (brightness > 12)
    {
        brightness = 7;
    }
    brightness_changed = true;
    brightness_changed_time = millis();
}

void state_change_clicked()
{
    if (state_changed_time + 500 > millis())
    {
        return;
    }

    ESP_LOGD(TAG, "State Change Clicked");
    state++;

    if (state > 3)
    {
        state = 1;
    }
    state_changed = true;
    state_changed_time = millis();
}

void setup()
{
    M5.begin();
    M5.Axp.ScreenBreath(brightness);
    // put your setup code here, to run once:
    // active_view = new TimeView();
    active_view = new AnalogClock();
    pinMode(M5_BUTTON_RST, INPUT_PULLUP);
    pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
    // active_view = new BTView();

    // ledcSetup(channel, freq, resolution_bits);
    // ledcAttachPin(buzzer, channel);
    // attachInterrupt(digitalPinToInterrupt(M5_BUTTON_RST), brightness_change_clicked, RISING);
    attachInterrupt(digitalPinToInterrupt(M5_BUTTON_HOME), state_change_clicked, FALLING);
}

void loop()
{
    // put your main code here, to run repeatedly:
    active_view->render();

    if (brightness_changed)
    {
        M5.Axp.ScreenBreath(brightness);
        brightness_changed = false;
    }

    if (state_changed)
    {
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
            active_view = new BTView();
            break;
        
        default:
            break;
        }

        state_changed = false;
    }

    if (digitalRead(M5_BUTTON_RST) ==LOW)
    {
        brightness_change_clicked();
    }

    if (digitalRead(M5_BUTTON_HOME) ==LOW)
    {
        state_change_clicked();
    }
}