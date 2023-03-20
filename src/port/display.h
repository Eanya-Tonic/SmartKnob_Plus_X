#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "lvgl.h"
#include <TFT_eSPI.h>

typedef TFT_eSPI SCREEN_CLASS;

void Port_Init();
void DisplayFault_Init(SCREEN_CLASS* scr);
void lv_port_disp_init(SCREEN_CLASS* scr);
void lv_fs_if_init();
void lv_port_indev_init();

extern TaskHandle_t handleTaskLvgl;
#endif /* __DISPLAY_H__ */