#include "SurfaceDialView.h"

using namespace Page;
static uint32_t last_publish = 0;

/*
 * 默认视图显示： 圆点，原点所在位置 label_value
 * 此函数根据需要增加或 hidden 对象
 */
void Page::SurfaceDialView::SetPlaygroundMode(int16_t mode)
{
	lv_obj_add_flag(ui.lable_value, LV_OBJ_FLAG_HIDDEN);
	lv_meter_set_scale_ticks(ui.meter, ui.scale_pot, 73, 2, 0, lv_color_make(0xff, 0x00, 0x00));
	lv_meter_set_scale_range(ui.meter, ui.scale_pot, 0, 72, 360, 270);
}

void Page::SurfaceDialView::UpdateView(PlaygroundInfo *info)
{

	int32_t motor_pos = info->motor_pos;
	motor_pos = motor_pos % (360 / 5); // 5 为surface dial 电机模式最小角度
	if (motor_pos < 0)
	{
		motor_pos = 360 / 5 + motor_pos;
	}
	lv_meter_set_indicator_value(ui.meter, ui.nd_img_circle, motor_pos);
	lv_color_t color;
	if (millis() - last_publish > 5)
	{
		if (HAL::LightMode())
		{
			color = info->is_ble_connected ? lv_color_make(0x00, 0x82, 0xfc) : lv_color_black();
		}
		else
		{
			color = info->is_ble_connected ? lv_color_make(0x00, 0x82, 0xfc) : lv_color_white();
		}
		lv_obj_set_style_img_recolor_opa(m_ui.img_super_dial, LV_OPA_COVER, 0);
		lv_obj_set_style_img_recolor(m_ui.img_super_dial, color, 0);
		last_publish = millis();
	}
}

void SurfaceDialView::Create(lv_obj_t *root)
{
	PlaygroundView::Create(root);

	m_ui.img_super_dial = lv_img_create(root);
	lv_img_set_src(m_ui.img_super_dial, Resource.GetImage("dialpad"));
	lv_obj_align(m_ui.img_super_dial, LV_ALIGN_CENTER, 0, 0);
}

void SurfaceDialView::Delete()
{

PlaygroundView:
	Delete();
}