#include "MenuView.h"

using namespace Page;

#define ITEM_HEIGHT_MIN 100
#define ITEM_PAD ((LV_VER_RES - ITEM_HEIGHT_MIN) / 2)

void MenuView::Create(lv_obj_t *root)
{
	lv_obj_remove_style_all(root);
	lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
	if (HAL::LightMode() == 1)
	{
		lv_obj_set_style_bg_color(root, lv_color_white(), 0);
	}
	else
	{
		lv_obj_set_style_bg_color(root, lv_color_black(), 0);
	}
	lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
	lv_obj_set_style_pad_ver(root, ITEM_PAD, 0);

	lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(
		root,
		LV_FLEX_ALIGN_START,
		LV_FLEX_ALIGN_START,
		LV_FLEX_ALIGN_CENTER);

	Style_Init();

	/* Item  */
	Item_Create(
		&ui.update,
		root,
		"在线更新",
		"update",

		"连接无线局域网后\n"
		"通过GitHub\n"
		"在线更新\n"
		"至最新系统\n");
	Item_Create(
		&ui.dialpad,
		root,
		"蓝牙控制",
		"dialpad",

		"Surface Dial"
		"模式\n"
		"通过蓝牙连接\n"
		"控制电脑\n"
		"需要系统 Windows10+\n");
	Item_Create(
		&ui.wifipad,
		root,
		"无线控制",
		"wifipad",

		"无线局域网"
		"模式\n"
		"通过无线局域网连接\n"
		"控制电脑\n"
		"需要安装电脑端软件\n");

	Item_Create(
		&ui.switches,
		root,
		"游乐场",
		"switches",

		"体验\n"
		"无阻尼无限制模式,\n"
		"有阻尼无限制模式,\n"
		"无阻尼有限制模式,\n"
		"开关模式\n"

	);
	Item_Create(
		&ui.hass,
		root,
		"智能家居",
		"home",

		"通过MQTT\n"
		"和无线局域网\n"
		"控制您的各类 \n"
		"智能家居设备");
	// /* Item System */
	Item_Create(
		&ui.system,
		root,
		"关于系统",
		"system_info",

		"设备名\n"
		"当前版本\n"
		"用户\n"
		"LVGL\n"
		"编译器\n\n"
		"编译时间\n");

	// /* Item Battery */
	// Item_Create(
	// 	&ui.battery,
	// 	root,
	// 	"Battery",
	// 	"battery_info",

	// 	"Usage\n"
	// 	"Voltage\n"
	// 	"Status"
	// );

	// /* Item Storage */
	// Item_Create(
	// 	&ui.storage,
	// 	root,
	// 	"Storage",
	// 	"storage",

	// 	"Detect\n"
	// 	"Size\n"
	// 	"Version"
	// );

	Group_Init();
}

void MenuView::Group_Init()
{
	ui.group = lv_group_create();
	lv_group_set_focus_cb(ui.group, onFocus);
	lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), ui.group);

	lv_group_add_obj(ui.group, ui.update.icon);
	lv_group_add_obj(ui.group, ui.dialpad.icon);
	lv_group_add_obj(ui.group, ui.wifipad.icon);
	lv_group_add_obj(ui.group, ui.switches.icon);
	lv_group_add_obj(ui.group, ui.hass.icon);
	lv_group_add_obj(ui.group, ui.system.icon);
	// lv_group_add_obj(ui.group, ui.battery.icon);
	// lv_group_add_obj(ui.group, ui.storage.icon);

	lv_group_focus_obj(ui.switches.icon);
}

void MenuView::Delete()
{
	lv_group_del(ui.group);
	Style_Reset();
}

void MenuView::SetScrollToY(lv_obj_t *obj, lv_coord_t y, lv_anim_enable_t en)
{
	lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
	lv_coord_t diff = -y + scroll_y;

	lv_obj_scroll_by(obj, 0, diff, en);
}

void MenuView::onFocus(lv_group_t *g)
{
	lv_obj_t *icon = lv_group_get_focused(g);
	lv_obj_t *cont = lv_obj_get_parent(icon);
	lv_coord_t y = lv_obj_get_y(cont);
	lv_obj_scroll_to_y(lv_obj_get_parent(cont), y, LV_ANIM_ON);
}

void MenuView::Style_Init()
{
	lv_style_init(&style.icon);
	lv_style_set_width(&style.icon, 220);
	lv_style_set_bg_opa(&style.icon, LV_OPA_COVER);
	lv_style_set_text_font(&style.icon, Resource.GetFont("HarmonyOS_Sans_SC_Regular_16"));
	if (HAL::LightMode() == 1)
	{
		lv_style_set_text_color(&style.icon, lv_color_black());
		lv_style_set_bg_color(&style.icon, lv_color_white());
	}
	else
	{
		lv_style_set_text_color(&style.icon, lv_color_white());
		lv_style_set_bg_color(&style.icon, lv_color_black());
	}

	lv_style_init(&style.focus);
	lv_style_set_width(&style.focus, 70);
	lv_style_set_border_side(&style.focus, LV_BORDER_SIDE_RIGHT);
	lv_style_set_border_width(&style.focus, 2);
	lv_style_set_border_color(&style.focus, lv_color_hex(0xff0000));

	static const lv_style_prop_t style_prop[] =
		{
			LV_STYLE_WIDTH,
			LV_STYLE_PROP_INV};

	static lv_style_transition_dsc_t trans;
	lv_style_transition_dsc_init(
		&trans,
		style_prop,
		lv_anim_path_overshoot,
		200,
		0,
		nullptr);
	lv_style_set_transition(&style.focus, &trans);
	lv_style_set_transition(&style.icon, &trans);

	lv_style_init(&style.info);
	lv_style_set_text_font(&style.info, Resource.GetFont("HarmonyOS_Sans_SC_Regular_12"));
	if (HAL::LightMode() != 1)
	{
		lv_style_set_text_color(&style.info, lv_color_hex(0x999999));
	}
	else
	{
		lv_style_set_text_color(&style.info, lv_color_hex(0x666666));
	}

	lv_style_init(&style.data);
	lv_style_set_text_font(&style.data, Resource.GetFont("HarmonyOS_Sans_SC_Regular_12"));
	if (HAL::LightMode() != 1)
	{
		lv_style_set_text_color(&style.data, lv_color_white());
	}
	else
	{
		lv_style_set_text_color(&style.data, lv_color_black());
	}
}

void MenuView::Style_Reset()
{
	lv_style_reset(&style.icon);
	lv_style_reset(&style.info);
	lv_style_reset(&style.data);
	lv_style_reset(&style.focus);
}

void MenuView::Item_Create(
	item_t *item,
	lv_obj_t *par,
	const char *name,
	const char *img_src,
	const char *infos)
{
	lv_obj_t *cont = lv_obj_create(par);
	lv_obj_remove_style_all(cont);
	lv_obj_set_width(cont, 220);

	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	item->cont = cont;

	/* icon */
	lv_obj_t *icon = lv_obj_create(cont);
	lv_obj_remove_style_all(icon);
	lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_add_style(icon, &style.icon, 0);
	lv_obj_add_style(icon, &style.focus, LV_STATE_FOCUSED);
	lv_obj_set_style_align(icon, LV_ALIGN_LEFT_MID, 0);

	lv_obj_set_flex_flow(icon, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(
		icon,
		LV_FLEX_ALIGN_SPACE_AROUND,
		LV_FLEX_ALIGN_CENTER,
		LV_FLEX_ALIGN_CENTER);

	lv_obj_t *img = lv_img_create(icon);
	lv_img_set_src(img, Resource.GetImage(img_src));
	if (HAL::LightMode() == 1)
	{
		lv_obj_set_style_img_recolor_opa(img, LV_OPA_COVER, 0);
		lv_obj_set_style_img_recolor(img, lv_color_black(), 0);
	}

	lv_obj_t *label = lv_label_create(icon);
	lv_label_set_text(label, name);
	item->icon = icon;

	/* infos */
	label = lv_label_create(cont);
	lv_label_set_text(label, infos);
	lv_obj_add_style(label, &style.info, 0);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 75, 0);
	item->labelInfo = label;

	/* datas */
	label = lv_label_create(cont);
	lv_label_set_text(label, "");
	lv_obj_add_style(label, &style.data, 0);
	lv_obj_align(label, LV_ALIGN_CENTER, 60, 0);
	item->labelData = label;

	lv_obj_move_foreground(icon);

	/* get real max height */
	lv_obj_update_layout(item->labelInfo);
	lv_coord_t height = lv_obj_get_height(item->labelInfo);
	height = LV_MAX(height, ITEM_HEIGHT_MIN);
	lv_obj_set_height(cont, height);
	lv_obj_set_height(icon, height);
}

void MenuView::SetSystem(
	const char *firmVer,
	const char *authorName,
	const char *lvglVer,
	const char *bootTime,
	const char *compilerName,
	const char *bulidTime)
{
	lv_label_set_text_fmt(
		ui.system.labelData,
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s",
		firmVer,
		authorName,
		lvglVer,
		bootTime,
		compilerName,
		bulidTime);
}
