#pragma once

#ifndef __HAL_H__
#define __HAL_H__
#include "button_event.h"
#include "HAL_Def.h"
#include <Arduino.h>
#include "config.h"
#include "CommonMacro.h"

typedef enum
{
    SUPER_DIAL_NULL = 0,
    SUPER_DIAL_LEFT = 1,
    SUPER_DIAL_RIGHT = 2,
} SuperDialMotion;

typedef enum
{
    HASS_LEFT = 1,
    HASS_RIGHT = 2,
    HASS_PUSH = 3,
    HASS_MAX,
} HassMotion;

namespace HAL
{
    void Init();
    void Update();

    void knob_init();
    void knob_update(void);
    bool encoder_is_pushed(void);

    // void TaskMotorUpdate(void *pvParameters);
    int get_motor_position(void);
    void update_motor_mode(int mode);

    void surface_dial_init(void);
    void surface_dial_update(SuperDialMotion direction);
    void surface_dial_release(void);
    void surface_dial_press(void);
    bool surface_dial_is_connected(void);

    void wifi_init(void* parameter);
    bool wifi_is_connected(void);
    void wifi_disconnect(void);
    void wifi_dial_update(SuperDialMotion direction);
    void wifi_dial_press(void);
    void wifi_dial_release(void);
    String return_IP(void);
    bool return_UDP(void);
    int updateTime(void);
    int return_hour(void);
    int return_minute(void);
    
    void OTA_setup(void* parameter);
    void OTAloop(void* parameter);
    String updateInfo(void);

    void power_init(void);
    void power_off(void);
    int32_t LightMode(void);

    bool is_encoder_enabled(void);
    void encoder_disable(void);
    void encoder_enable(void);

    void mqtt_init(void);
    int mqtt_publish(const char *topic, const char *playload);
    int mqtt_subscribe(const char *topic);
    void mqtt_deinit(void);
}

#endif