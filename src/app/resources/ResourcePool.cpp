#include "ResourcePool.h"

/* Global resource manager */
ResourcePool Resource;

extern "C"
{
#define IMPORT_FONT(name)                                        \
    do                                                           \
    {                                                            \
        LV_FONT_DECLARE(font_##name)                             \
        Resource.Font_.AddResource(#name, (void *)&font_##name); \
    } while (0)

#define IMPORT_IMG(name)                                             \
    do                                                               \
    {                                                                \
        LV_IMG_DECLARE(img_src_##name)                               \
        Resource.Image_.AddResource(#name, (void *)&img_src_##name); \
    } while (0)

    static void Resource_Init()
    {
        /* Import Fonts */
        IMPORT_FONT(HarmonyOS_Sans_SC_Regular_12);
        IMPORT_FONT(HarmonyOS_Sans_SC_Regular_26);
        IMPORT_FONT(HarmonyOS_Sans_SC_Regular_16);

        /* Import Images */
        IMPORT_IMG(system_info);

        // IMPORT_IMG(arm);
        IMPORT_IMG(bluetooth);
        IMPORT_IMG(switches);
        IMPORT_IMG(dialpad);
        IMPORT_IMG(wifipad);
        IMPORT_IMG(update);
        IMPORT_IMG(dot_blue);
        IMPORT_IMG(macos);

        // HASS
        IMPORT_IMG(home);
        IMPORT_IMG(home_fan);
        IMPORT_IMG(home_bulb);
        IMPORT_IMG(home_air_cond);
        IMPORT_IMG(home_wash_machine);
    }

} /* extern "C" */

void ResourcePool::Init()
{
    lv_obj_remove_style_all(lv_scr_act());
    if (HAL::LightMode() == 1)
    {
        lv_disp_set_bg_color(lv_disp_get_default(), lv_color_white());
    }
    else
    {
        lv_disp_set_bg_color(lv_disp_get_default(), lv_color_black());
    }

    Font_.SetDefault((void *)&lv_font_montserrat_14);

    Resource_Init();
}
