#ifndef __WIFI_DIAL_VIEW_H
#define __WIFI_DIAL_VIEW_H
#include "Arduino.h"
#include "app/app.h"
#include "../Page.h"
#include "PlaygroundView.h"


namespace Page
{

class wifiDialView: public PlaygroundView
{
public:
    wifiDialView(){}
    void Create(lv_obj_t* root);
    void Delete();
    void UpdateView(PlaygroundInfo *info);
    void SetPlaygroundMode(int16_t mode);


public:
    struct {
        lv_obj_t* img_wifi_dial;
        lv_obj_t *label;
    }m_ui;

private:
};

}

#endif // !__VIEW_H
