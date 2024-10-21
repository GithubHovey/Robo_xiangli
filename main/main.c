#include <stdio.h>
#include "Applications/include/sys_init.h"
#include "Middlewares/include/myGUI.h"
void app_main(void)
{
    vTaskDelay(1000);
    printf("start memory:\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);
    Module_init();
    AppInit();   
}

