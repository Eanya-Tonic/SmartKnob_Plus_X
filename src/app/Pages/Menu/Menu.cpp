#include "Menu.h"
#include "app/Configs/Version.h"
#include "app/app.h"
#include "hal/EEPROM_utils.h"
using namespace Page;

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::onCustomAttrConfig()
{
}

void Menu::onViewLoad()
{
	Model.Init();
	View.Create(root);
	AttachEvent(root, onPlaygroundEvent);
	AttachEvent(View.ui.dialpad.icon, onSuperDialEvent);
	AttachEvent(View.ui.wifipad.icon, onwifiDialEvent);
	AttachEvent(View.ui.update.icon, onOTAEvent);
	AttachEvent(View.ui.switches.icon, onPlaygroundEvent);
	AttachEvent(View.ui.hass.icon, onHassEvent);
	AttachEvent(View.ui.system.icon, onSystemEvent);
}

void Menu::onViewDidLoad()
{
}

void Menu::onViewWillAppear()
{
	lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), View.ui.group);
	// StatusBar::SetStyle(StatusBar::STYLE_BLACK);

	timer = lv_timer_create(onTimerUpdate, 100, this);
	lv_timer_ready(timer);

	View.SetScrollToY(root, -LV_VER_RES, LV_ANIM_OFF);
	lv_obj_fade_in(root, 300, 0);
}

void Menu::onViewDidAppear()
{
	View.onFocus(View.ui.group);
}

void Menu::onViewWillDisappear()
{
	lv_obj_fade_out(root, 300, 0);
}

void Menu::onViewDidDisappear()
{
	lv_timer_del(timer);
}

void Menu::onViewDidUnload()
{
	View.Delete();
	Model.Deinit();
}

void Menu::AttachEvent(lv_obj_t *obj, lv_event_cb_t event_cb)
{
	lv_obj_set_user_data(obj, this);
	lv_obj_add_event_cb(obj, event_cb, LV_EVENT_ALL, this);
}

void Menu::Update()
{
	char buf[64];

	// 设置默认
	String GET_DEVICE_NAME = "SmartKnob X";
	String GET_DEVICE_VERSION = "1.0.0";
	String GET_DEVICE_USER = "EanyaTonic";
	if (EEPROM.read(DEVICE_NAME_ADDR) != 0)
	{
		GET_DEVICE_NAME = get_String(EEPROM.read(DEVICE_NAME_ADDR), DEVICE_NAME_ADDR + 1);
		GET_DEVICE_USER = get_String(EEPROM.read(DEVICE_USER_ADDR), DEVICE_USER_ADDR + 1);
	}
	else
	{
		DEVICE_NAME_NUM = GET_DEVICE_NAME.length();
		DEVICE_USER_NUM = GET_DEVICE_USER.length();
		EEPROM.write(DEVICE_NAME_ADDR, DEVICE_NAME_NUM);
		EEPROM.write(DEVICE_USER_ADDR, DEVICE_USER_NUM);
		set_String(DEVICE_NAME_NUM, DEVICE_NAME_ADDR + 1, GET_DEVICE_NAME);
		set_String(DEVICE_USER_NUM, DEVICE_USER_ADDR + 1, GET_DEVICE_USER);
	}

	/* System */
	View.SetSystem(
		GET_DEVICE_NAME.c_str(),
		GET_DEVICE_VERSION.c_str(),
		GET_DEVICE_USER.c_str(),
		VERSION_LVGL,
		VERSION_COMPILER,
		VERSION_BUILD_TIME);
}

void Menu::onTimerUpdate(lv_timer_t *timer)
{
	Menu *instance = (Menu *)timer->user_data;

	instance->Update();
}

void Menu::onPlaygroundEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		// instance->Model.ChangeMotorMode(MOTOR_FINE_DETENTS);
		instance->Manager->Push("Pages/Playground");
	}
}

void Menu::onSystemEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		Serial.printf("Power off...\n");
		HAL::power_off();
	}
}

void Menu::onSuperDialEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		// instance->Model.ChangeMotorMode(MOTOR_FINE_DETENTS);
		int16_t mode = APP_MODE_SUPER_DIAL;
		Stash_t stash;
		stash.ptr = &mode;
		stash.size = sizeof(int16_t);
		instance->Manager->Push("Pages/Playground", &stash);
	}
}

void Menu::onwifiDialEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		// instance->Model.ChangeMotorMode(MOTOR_FINE_DETENTS);
		int16_t mode = APP_MODE_WIFI_DIAL;
		Stash_t stash;
		stash.ptr = &mode;
		stash.size = sizeof(int16_t);
		instance->Manager->Push("Pages/Playground", &stash);
	}
}

void Menu::onOTAEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		// instance->Model.ChangeMotorMode(MOTOR_FINE_DETENTS);
		int16_t mode = APP_MODE_OTA;
		Stash_t stash;
		stash.ptr = &mode;
		stash.size = sizeof(int16_t);
		instance->Manager->Push("Pages/Playground", &stash);
	}
}

void Menu::onHassEvent(lv_event_t *event)
{
	lv_obj_t *obj = lv_event_get_target(event);
	lv_event_code_t code = lv_event_get_code(event);
	auto *instance = (Menu *)lv_obj_get_user_data(obj);

	if (code == LV_EVENT_PRESSED)
	{
		// instance->Model.ChangeMotorMode(MOTOR_FINE_DETENTS);
		int16_t mode = APP_MODE_HOME_ASSISTANT;
		Stash_t stash;
		stash.ptr = &mode;
		stash.size = sizeof(int16_t);
		instance->Manager->Push("Pages/Playground", &stash);
	}
}
