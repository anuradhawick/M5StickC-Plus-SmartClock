#include <M5StickCPlus.h>
#include <BluetoothSerial.h>
#include "views/View.h"
#include "views/BTView.h"
#include "animations/loader.h"

static const char *TAG = "BT_VIEW";
static BluetoothSerial *btSerial = NULL;

static void bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_SRV_OPEN_EVT)
    {
        ESP_LOGD(TAG, "Client Connected");
        String text = "Send the option number..\
        \n1. WiFi Options\
        \n2. Set time\
        \n3. Set date\
        \n";

        for (size_t i = 0; i < text.length(); i++)
        {
            btSerial->write((uint8_t)text[i]);
        }
    }

    if (event == ESP_SPP_CLOSE_EVT)
    {
        ESP_LOGD(TAG, "Client disconnected");
    }
}

BTView::BTView()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);

    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(true);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    loader_last_rendered = 0;
    text_last_rendered = 0;
    loader_frame = 0;

    inited_time = millis();
    bt_started = false;

    render_text();
    render_loader();
}

BTView::~BTView()
{
    ESP_LOGD(TAG, "Destructor called");
    if (bt_started)
    {
        delete btSerial;
        btSerial = NULL;
    }

    disp_buffer->deleteSprite();
    delete disp_buffer;

    ESP_LOGD(TAG, "Destructor finished");
}

void BTView::render_loader()
{
    disp_buffer->pushImage(240 - loader_ani_width,
                           67 - loader_ani_height / 2,
                           loader_ani_width,
                           loader_ani_height,
                           loader_ani_imgs[loader_frame]);
    loader_frame++;

    if (loader_frame == loader_ani_frames)
    {
        loader_frame = 0;
    }
    disp_buffer->pushSprite(0, 0);
}

void BTView::render_text()
{
    if (bt_started)
    {
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Active", 10, 10, 4);
        disp_buffer->drawString("Connect with your", 10, 50, 2);
        disp_buffer->drawString("BT Serial App", 10, 70, 2);
        disp_buffer->drawString("to Continue", 10, 90, 2);
        disp_buffer->pushSprite(0, 0);
    }
    else
    {
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Loading", 10, 10, 4);
        disp_buffer->drawString("Wait " + String(5 - (millis() - inited_time) / 1000) + "s", 10, 50, 2);
        disp_buffer->drawString("to Continue", 10, 70, 2);
        disp_buffer->pushSprite(0, 0);
    }
}

void BTView::render()
{
    if (millis() > loader_last_rendered + 100)
    {
        loader_last_rendered = millis();
        render_loader();
    }

    if (millis() > text_last_rendered + 1000)
    {
        text_last_rendered = millis();
        render_text();
    }

    if (bt_started)
    {
        bt_loop();
    }
    else if (!bt_started && inited_time + 5000 < millis())
    {
        btSerial = new BluetoothSerial();
        btSerial->register_callback(bt_callback);
        btSerial->begin("M5Stick");
        ESP_LOGD(TAG, "BT Serial Started");
        bt_started = true;
    }
}

void BTView::bt_loop()
{
    if (btSerial->available())
    {
        bt_incoming = btSerial->readString();
        bt_incoming.trim();
        ESP_LOGD(TAG, "Incoming from BT Serial - %s", bt_incoming.c_str());

        switch (main_menu)
        {
        case 0:
            // select main menu
            if (1 <= (uint8_t)bt_incoming.toInt() && (uint8_t)bt_incoming.toInt() <= 3)
            {
                main_menu = (uint8_t)bt_incoming.toInt();

                switch (main_menu)
                {
                case 1:
                    text = "Not implemented\n";
                    break;
                case 2:
                    text = "Enter time as HH:MM\n";
                    break;
                case 3:
                    text = "Enter date as DD/MM/YYYY/D, D=1 for Sunday\n";
                    break;

                default:
                    break;
                }

                for (size_t i = 0; i < text.length(); i++)
                {
                    btSerial->write((uint8_t)text[i]);
                }
            }
            break;
        case 1:
            break;
        case 2:
            RTC_TimeStruct.Hours = (uint8_t)bt_incoming.substring(0, 2).toInt();
            RTC_TimeStruct.Minutes = (uint8_t)bt_incoming.substring(3, 5).toInt();

            ESP_LOGD(TAG, "Update time to %d:%d",
                     RTC_TimeStruct.Hours,
                     RTC_TimeStruct.Minutes,
                     bt_incoming.substring(0, 2),
                     bt_incoming.substring(3, 5));

            M5.Rtc.SetTime(&RTC_TimeStruct);

            text = "Time successfully updated\n";

            for (size_t i = 0; i < text.length(); i++)
            {
                btSerial->write((uint8_t)text[i]);
            }

            break;
        case 3:
            RTC_DateStruct.Date = (uint8_t)bt_incoming.substring(0, 2).toInt();
            RTC_DateStruct.Month = (uint8_t)bt_incoming.substring(3, 5).toInt();
            RTC_DateStruct.Year = (uint16_t)bt_incoming.substring(6, 10).toInt();
            RTC_DateStruct.WeekDay = (uint16_t)bt_incoming.substring(11, 12).toInt() - 1;

            ESP_LOGD(TAG, "Update date to %d/%d/%d/%d",
                     RTC_DateStruct.Date,
                     RTC_DateStruct.Month,
                     RTC_DateStruct.Year,
                     RTC_DateStruct.WeekDay);

            M5.Rtc.SetData(&RTC_DateStruct);

            text = "Date successfully updated\n";

            for (size_t i = 0; i < text.length(); i++)
            {
                btSerial->write((uint8_t)text[i]);
            }

            break;
        default:
            break;
        }
    }
}