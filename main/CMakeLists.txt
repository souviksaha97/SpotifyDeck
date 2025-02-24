idf_component_register(SRCS "./sources/main.c" "./sources/WifiHandler.c" "./sources/ledHandler.c" "./sources/BuzzerHandler.c"
                    INCLUDE_DIRS "./include/"
                    REQUIRES esp_http_client esp_wifi nvs_flash json driver)