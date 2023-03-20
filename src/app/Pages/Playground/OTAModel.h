#ifndef __OTA_MODEL_H
#define __OTA_MODEL_H
#include "app/Utils/AccountSystem/Account.h"
#include "lvgl.h"
#include "PlaygroundView.h"
#include "PlaygroundModel.h"

namespace Page
{

class OTAModel: public PlaygroundModel
{
public:
    OTAModel();
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
