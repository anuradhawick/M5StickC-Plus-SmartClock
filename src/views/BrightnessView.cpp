#include <M5StickCPlus.h>
#include "views/View.h"
#include "event.h"
#include "views/BrightnessView.h"

static const char *TAG = "BRIGHTNESS_VIEW";

BrightnessView::BrightnessView()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);

    brightness = M5.Axp.Read8bit(0x28) >> 4;
    rendering_brightness_value = (float)brightness;
    last_rendered = millis();

    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextFont(4);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    header_message = "Adjust Brightness";

    disp_buffer->drawString(header_message,
                            M5.Lcd.width() / 2 - M5.Lcd.textWidth(header_message, 4) / 2,
                            5,
                            4);
    disp_buffer->fillTriangle(0,
                              M5.Lcd.height() - 25,
                              40 * (brightness - 6),
                              M5.Lcd.height() - 25,
                              40 * (brightness - 6),
                              (double)M5.Lcd.height() - 25 - 40.0 * (double)(brightness - 6) * tan(PI / 10),
                              WHITE);
    disp_buffer->pushSprite(0, 0);
}

BrightnessView::~BrightnessView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
}

void BrightnessView::render()
{
    if (abs(rendering_brightness_value - brightness) < 0.001)
    {
        rendering_brightness_value = (float)brightness;
    }

    if (rendering_brightness_value < brightness)
    {
        rendering_brightness_value += 0.005;
    }
    else if (rendering_brightness_value > brightness)
    {
        rendering_brightness_value -= 0.005;
    }
    else
    {
        return;
    }

    if (last_rendered + 10 < millis() && rendering_brightness_value != brightness)
    {
        disp_buffer->fillRect(0, 0, 240, 135, BLACK);
        disp_buffer->drawString(header_message,
                                M5.Lcd.width() / 2 - M5.Lcd.textWidth(header_message, 4) / 2,
                                5,
                                4);
        disp_buffer->fillTriangle(0,
                                  M5.Lcd.height() - 25,
                                  40 * (rendering_brightness_value - 6),
                                  M5.Lcd.height() - 25,
                                  40 * (rendering_brightness_value - 6),
                                  (double)M5.Lcd.height() - 25 - 40.0 * (double)(rendering_brightness_value - 6) * tan(PI / 10),
                                  WHITE);
        disp_buffer->pushSprite(0, 0);
        last_rendered = millis();
    }
}

bool BrightnessView::receive_event(EVENTS::event event)
{
    switch (event)
    {
    case EVENTS::RESET_PRESSED:
        brightness--;

        if (brightness < 7)
        {
            brightness = 7;
        }
        M5.Axp.ScreenBreath(brightness);
        return true;
    case EVENTS::POWER_PRESSED:
        brightness++;

        if (brightness > 12)
        {
            brightness = 12;
        }
        M5.Axp.ScreenBreath(brightness);
        return true;
    default:
        break;
    }
    return false;
}
