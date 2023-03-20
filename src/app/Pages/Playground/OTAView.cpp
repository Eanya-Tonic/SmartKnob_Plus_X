#include "OTAView.h"

using namespace Page;
static uint32_t last_publish = 0;
static unsigned long updateTime = 0;
static int tmpSecond = 0;

/*
 * 默认视图显示： 圆点，原点所在位置 label_value
 * 此函数根据需要增加或 hidden 对象
 */
void Page::OTAView::SetPlaygroundMode(int16_t mode)
{
	lv_obj_add_flag(ui.lable_value, LV_OBJ_FLAG_HIDDEN);
	lv_meter_set_scale_ticks(ui.meter, ui.scale_pot, 73, 2, 0, lv_color_make(0xff, 0x00, 0x00));
	lv_meter_set_scale_range(ui.meter, ui.scale_pot, 0, 72, 360, 270);
}

void Page::OTAView::UpdateView(PlaygroundInfo *info)
{

	int32_t motor_pos = info->motor_pos;
	motor_pos = motor_pos % (360 / 5); // 5 为surface dial 电机模式最小角度
	if (motor_pos < 0)
	{
		motor_pos = 360 / 5 + motor_pos;
	}
	lv_meter_set_indicator_value(ui.meter, ui.nd_img_circle, motor_pos);
	lv_color_t color;
	if (millis() - last_publish > 500)
	{
		if (HAL::LightMode())
		{
			color = info->is_wifi_connected ? lv_color_make(0x00, 0x82, 0xfc) : lv_color_black();
		}
		else
		{
			color = info->is_wifi_connected ? lv_color_make(0x00, 0x82, 0xfc) : lv_color_white();
		}
		lv_obj_set_style_img_recolor_opa(m_ui.img_wifi_dial, LV_OPA_COVER, 0);
		lv_obj_set_style_img_recolor(m_ui.img_wifi_dial, color, 0);
		lv_label_set_text(m_ui.label, HAL::updateInfo().c_str());
	}
	vTaskDelay(1);
	rtc_wdt_feed();
}

void OTAView::Create(lv_obj_t *root)
{
	PlaygroundView::Create(root);

	m_ui.img_wifi_dial = lv_img_create(root);
	lv_img_set_src(m_ui.img_wifi_dial, Resource.GetImage("wifipad"));
	lv_obj_align(m_ui.img_wifi_dial, LV_ALIGN_CENTER, 0, 0);

	m_ui.label = lv_label_create(root);
	lv_obj_set_style_text_font(m_ui.label, Resource.GetFont("HarmonyOS_Sans_SC_Regular_16"), 0);
	if (HAL::LightMode() == 1)
	{
		lv_obj_set_style_text_color(m_ui.label, lv_color_black(), 0);
	}
	else
	{
		lv_obj_set_style_text_color(m_ui.label, lv_color_white(), 0);
	}
	lv_label_set_text(m_ui.label, "UPDATE");
	lv_obj_align(m_ui.label, LV_ALIGN_BOTTOM_MID, 0, -55);
}

void OTAView::Delete()
{

PlaygroundView:
	Delete();
}