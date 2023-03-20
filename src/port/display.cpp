#include "display.h"
#include "config.h"
#include "hal/hal.h"
#include "soc/rtc_wdt.h"

TaskHandle_t handleTaskLvgl;
void TaskLvglUpdate(void* parameter)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    for (;;)
    {
        lv_task_handler();

        vTaskDelay(5);

        rtc_wdt_feed();
    }
}

void display_init(void)
{
    static SCREEN_CLASS screen;
    screen.begin();        /* TFT init */
    screen.setRotation(0); /* Landscape orientation */
    
    //背光控制
    static const uint8_t LEDC_CHANNEL_LCE_BACKLIGHT = 0;
    ledcSetup(LEDC_CHANNEL_LCE_BACKLIGHT, 5000, 16);
    ledcAttachPin(PIN_LCD_BACKLIGHT, LEDC_CHANNEL_LCE_BACKLIGHT);
    ledcWrite(LEDC_CHANNEL_LCE_BACKLIGHT, UINT16_MAX);

    screen.fillScreen(TFT_BLACK);

    lv_init();
    lv_port_disp_init(&screen);
    lv_port_indev_init();
    // Update display in parallel thread.
    xTaskCreatePinnedToCore(
        TaskLvglUpdate,
        "LvglThread",
        8192,
        nullptr,
        configMAX_PRIORITIES,
        &handleTaskLvgl,
        LVGL_RUNNING_CORE);

}