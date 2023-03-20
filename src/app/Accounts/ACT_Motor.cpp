#include "Account_Master.h"
#include "HAL/HAL.h"
#include "app/Configs/Config.h"

using namespace AccountSystem;

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(AccountSystem::Motor_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    AccountSystem::Motor_Info_t* info = (AccountSystem::Motor_Info_t*)param->data_p;
    Serial.printf("Motor: OnEvent");
    switch (info->cmd)
    {
    case MOTOR_CMD_CHANGE_MODE:
        /* code */
        HAL::update_motor_mode(info->motor_mode);
        break;
    case MOTOR_CMD_CHECKOUT_PAGE:
        break;
    default:
        break;
    }
    

    return 0;
}

ACCOUNT_INIT_DEF(Motor)
{
    account->SetEventCallback(onEvent);
}