idf_component_register(SRCS "src/tas5805m.c"
                            "src/tas5805m-math.c"
                        INCLUDE_DIRS "include"
                        REQUIRES driver esp_driver_i2c)
