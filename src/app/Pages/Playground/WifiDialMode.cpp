#include "wifiDialModel.h"
#include "app/Accounts/Account_Master.h"
#include <Arduino.h>
#include "hal/hal.h"

using namespace Page;

wifiDialModel::wifiDialModel()
{
    app = APP_MODE_WIFI_DIAL;
}

void wifiDialModel::GetKnobStatus(PlaygroundInfo *info)
{
    PlaygroundModel::GetKnobStatus(info);
    info->is_wifi_connected = HAL::wifi_is_connected();
}

void wifiDialModel::SetPlaygroundMode(int16_t mode)
{
}

void wifiDialModel::Init()
{
    PlaygroundModel::Init();
}

void wifiDialModel::Deinit()
{
}