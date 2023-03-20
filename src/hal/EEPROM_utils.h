#pragma once

#include <EEPROM.h>


// EEPROM存储WiFi信息
static int32_t SISSD_ADDR = 0;
static int32_t PASSWORD_ADDR = 100;
static int32_t SISSD_NUM = 0;
static int32_t PASSWORD_NUM = 0;

// EEPROM储存系统设置信息
static int32_t SYSTEM_ADDR = 200;

// EEPROM存储系统信息
static int32_t DEVICE_NAME_ADDR = 300;
static int32_t DEVICE_NAME_NUM = 0;
static int32_t DEVICE_VERSION_ADDR = 400;
static int32_t DEVICE_VERSION_NUM = 0;
static int32_t DEVICE_USER_ADDR = 500;
static int32_t DEVICE_USER_NUM = 0;

// EEPROM储存MQTT信息
static int32_t MQTT_IP_ADDR = 600;
static int32_t MQTT_IP_NUM = 0;
static int32_t MQTT_PORT_ADDR = 700;
static int32_t MQTT_PORT_NUM = 0;
static int32_t MQTT_USER_ADDR = 800;
static int32_t MQTT_USER_NUM = 0;
static int32_t MQTT_PASSWORD_ADDR = 900;
static int32_t MQTT_PASSWORD_NUM = 0;

void set_String(int a, int b, String str);
String get_String(int a, int b);
void clean_EEPROM(void);