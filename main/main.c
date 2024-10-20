#include <stdio.h>
#include "Applications/include/sys_init.h"
#include "Middlewares/include/myGUI.h"
void app_main(void)
{
    vTaskDelay(1000);
    Module_init();
    AppInit();    
}

