#include <Arduino.h>
#include "app/app.h"
#include "app/ui/ui.h"
#include "hal/hal.h"
#include <esp32-hal-psram.h>

#include "interface_task.h"
#include "motor_task.h"

static MotorTask motor_task(1);
InterfaceTask interface_task(0, motor_task);

void HAL::update_motor_mode(int mode)
{
    interface_task.motor_task_.setConfig(configs[mode]);
}


void setup()
{

    // 启动串口
    Serial.begin(115200);
    // 启动 SPIRAM
    heap_caps_malloc_extmem_enable(heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    HAL::Init();
    display_init();
    App_Init();

    // 为 motor_task 设置一个日志记录器
    motor_task.setLogger(&interface_task);
    // 启动 motor_task
    motor_task.begin();
    // 启动 interface_task
    interface_task.begin();
}

void loop()
{
    HAL::Update();
    delay(10);
}
