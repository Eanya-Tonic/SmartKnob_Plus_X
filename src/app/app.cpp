/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "App.h"
#include "hal/hal.h"
#include "Utils/PageManager/PageManager.h"
#include "Accounts/Account_Master.h"
#include "Resources/ResourcePool.h"
#include "Pages/AppFactory.h"
void App_Init()
{
    static AppFactory factory;
    static PageManager manager(&factory);

    Accounts_Init();
    Resource.Init();

    // /*----------------------- Pages Init -----------------------*/
    // StatusBar::Init(lv_layer_top());

    manager.Install("Template", "Pages/Template");
    manager.Install("Menu", "Pages/Menu");
    manager.Install("Startup", "Pages/Startup");
    manager.Install("Playground", "Pages/Playground");

    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP, 500);
    manager.Push("Pages/Startup");
    INIT_DONE();

}

void App_UnInit()
{

}
