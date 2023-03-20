#ifndef __OTA_VIEW_H
#define __OTA_VIEW_H
#include "Arduino.h"
#include "app/app.h"
#include "../Page.h"
#include "PlaygroundView.h"


namespace Page
{

class OTAView: public PlaygroundView
{
public:
    OTAView(){}
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
