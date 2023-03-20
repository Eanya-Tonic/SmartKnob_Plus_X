#include "OTAModel.h"
#include "app/Accounts/Account_Master.h"
#include <Arduino.h>
#include "hal/hal.h"

using namespace Page;

OTAModel::OTAModel()
{
    app = APP_MODE_OTA;
}

void OTAModel::GetKnobStatus(PlaygroundInfo *info)
{
    PlaygroundModel::GetKnobStatus(info);
    info->is_wifi_connected = HAL::wifi_is_connected();
}

void OTAModel::SetPlaygroundMode(int16_t mode)
{
}

void OTAModel::Init()
{
    PlaygroundModel::Init();
}

void OTAModel::Deinit()
{
}