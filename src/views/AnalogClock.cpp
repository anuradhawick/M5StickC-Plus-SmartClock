#include <M5StickCPlus.h>
#include "views/View.h"
#include "views/AnalogClock.h"

static const char *TAG = "AN_CLOCK";

AnalogClock::AnalogClock()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);

    disp_buffer = new TFT_eSprite(&M5.Lcd);
    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    last_render = 0;
    origin_x = M5.Lcd.height() / 2 + 5;
    origin_y = M5.Lcd.height() / 2;

    for (int32_t i = 0; i < 60; i++)
    {
        disp_buffer->fillCircle(origin_x + round((seconds_hand_length + 3) * cos(i * 6 * PI / 180)),
                            origin_y - round((seconds_hand_length + 3) * sin(i * 6 * PI / 180)),
                            3 ? i % 5 == 0 : 1,
                            WHITE);
    }

    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);

    render_clock_face();
    render_date();
}

AnalogClock::~AnalogClock()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
}

void AnalogClock::render_date()
{
    dt_cache = RTC_DateStruct.Date;
    mn_cache = RTC_DateStruct.Month;
    yr_cache = RTC_DateStruct.Year;

    sprintf(dateDDMMYYYY, "%02d/%02d/%04d", dt_cache, mn_cache+1, yr_cache);
    disp_buffer->drawString(dateDDMMYYYY,
                        140 + (100 - M5.Lcd.textWidth("10/01/2000")) / 2 - 5,
                        67 - disp_buffer->fontHeight(2),
                        2);
    d_cache = RTC_DateStruct.WeekDay;

    // clearing printed text
    disp_buffer->drawString("          ",
                        140 + (100 - M5.Lcd.textWidth(days[d_cache - 1].c_str())) / 2,
                        67,
                        2);
    disp_buffer->drawString(days[d_cache - 1].c_str(),
                        140 + (100 - M5.Lcd.textWidth(days[d_cache - 1].c_str())) / 2,
                        67,
                        2);

    disp_buffer->drawFastHLine(140, 67, 100, WHITE);

    disp_buffer->pushSprite(0, 0);
}

void AnalogClock::render_clock_face()
{
    s_cache = RTC_TimeStruct.Seconds;
    m_cache = RTC_TimeStruct.Minutes;
    h_cache = RTC_TimeStruct.Hours;

    disp_buffer->fillTriangle(origin_x - round(hours_hand_width * cos(hours_angle * PI / 180)),
                          origin_y - round(hours_hand_width * sin(hours_angle * PI / 180)),
                          origin_x + round(hours_hand_width * cos(hours_angle * PI / 180)),
                          origin_y + round(hours_hand_width * sin(hours_angle * PI / 180)),
                          origin_x + round(hours_hand_length * sin(hours_angle * PI / 180)),
                          origin_y - round(hours_hand_length * cos(hours_angle * PI / 180)),
                          BLACK);

    disp_buffer->fillTriangle(origin_x - round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                          origin_y - round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                          origin_x + round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                          origin_y + round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                          origin_x + round(minutes_hand_length * sin(minutes_angle * PI / 180)),
                          origin_y - round(minutes_hand_length * cos(minutes_angle * PI / 180)),
                          BLACK);

    disp_buffer->fillTriangle(origin_x - round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                          origin_y - round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                          origin_x + round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                          origin_y + round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                          origin_x + round(seconds_hand_length * sin(seconds_angle * PI / 180)),
                          origin_y - round(seconds_hand_length * cos(seconds_angle * PI / 180)),
                          BLACK);

    seconds_angle = s_cache * 6;
    minutes_angle = m_cache * 6;
    hours_angle = h_cache * 5 * 6 + m_cache * 0.5;

    disp_buffer->fillTriangle(origin_x - round(hours_hand_width * cos(hours_angle * PI / 180)),
                          origin_y - round(hours_hand_width * sin(hours_angle * PI / 180)),
                          origin_x + round(hours_hand_width * cos(hours_angle * PI / 180)),
                          origin_y + round(hours_hand_width * sin(hours_angle * PI / 180)),
                          origin_x + round(hours_hand_length * sin(hours_angle * PI / 180)),
                          origin_y - round(hours_hand_length * cos(hours_angle * PI / 180)),
                          0x67E0);

    disp_buffer->fillTriangle(origin_x - round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                          origin_y - round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                          origin_x + round(minutes_hand_width * cos(minutes_angle * PI / 180)),
                          origin_y + round(minutes_hand_width * sin(minutes_angle * PI / 180)),
                          origin_x + round(minutes_hand_length * sin(minutes_angle * PI / 180)),
                          origin_y - round(minutes_hand_length * cos(minutes_angle * PI / 180)),
                          0x031F);

    disp_buffer->fillTriangle(origin_x - round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                          origin_y - round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                          origin_x + round(seconds_hand_width * cos(seconds_angle * PI / 180)),
                          origin_y + round(seconds_hand_width * sin(seconds_angle * PI / 180)),
                          origin_x + round(seconds_hand_length * sin(seconds_angle * PI / 180)),
                          origin_y - round(seconds_hand_length * cos(seconds_angle * PI / 180)),
                          RED);
    disp_buffer->fillCircle(origin_x, origin_y, hours_hand_width + 5, WHITE);
    disp_buffer->fillCircle(origin_x, origin_y, hours_hand_width, BLACK);

    disp_buffer->pushSprite(0, 0);
}

void AnalogClock::render()
{
    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);

    if (s_cache != RTC_TimeStruct.Seconds)
    {
        render_clock_face();
    }

    // update date if its changed
    if (dt_cache != RTC_DateStruct.Date || mn_cache != RTC_DateStruct.Month || yr_cache != RTC_DateStruct.Year)
    {
        render_date();
    }
}
