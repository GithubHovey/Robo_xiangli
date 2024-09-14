file(GLOB DRIVER_SRCS "./Drivers/src/*.c")
file(GLOB MIDDLEWARE_SRCS "./Middlewares/src/*.c")
file(GLOB MODULE_SRCS "./Modules/src/*.c")
file(GLOB APPLICATION_SRCS "./Applications/src/*.c")
idf_component_register(SRCS "main.c" "test3.c"
                            ${DRIVER_SRCS}
                            ${MIDDLEWARE_SRCS}
                            ${MODULE_SRCS}
                            ${APPLICATION_SRCS}
                    INCLUDE_DIRS    "./Drivers/include"
                                    "./Middlewares/include"
                                    "./Modules/include"
                                    "./Applications/include"
                                    )
spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)
