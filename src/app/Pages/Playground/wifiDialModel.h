#ifndef __WIFI_DIAL_MODEL_H
#define __WIFI_DIAL_MODEL_H
#include "app/Utils/AccountSystem/Account.h"
#include "lvgl.h"
#include "PlaygroundView.h"
#include "PlaygroundModel.h"

namespace Page
{

class wifiDialModel: public PlaygroundModel
{
public:
    wifiDialModel();
    void Init();
    void Deinit();
    void Update(void* pg_ui);
    void GetKnobStatus(PlaygroundInfo *info);
    void ChangeMotorMode(int mode);
    void SetPlaygroundMode(int16_t mode);

    void onEvent(Account* account, Account::EventParam_t* param);
private:

};

}

#endif
