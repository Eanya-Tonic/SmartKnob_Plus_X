#ifndef __CONFIG_H__
#define __CONFIG_H__


#define CONFIG_SCREEN_HOR_RES       240
#define CONFIG_SCREEN_VER_RES       240
#define CONFIG_SCREEN_BUFFER_SIZE   (CONFIG_SCREEN_HOR_RES * CONFIG_SCREEN_VER_RES / 1)

#define LVGL_RUNNING_CORE   0            
#define ESP32_RUNNING_CORE  1            

 // The owner of the X-Knob
#define MQTT_HOST               "SmartKnob"        

#endif 