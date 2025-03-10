idf_component_register(
    SRCS "./sources/main.c" "./sources/WifiHandler.c" "./sources/ledHandler.c" 
    "./sources/BuzzerHandler.c" "./sources/mpuHandler.c" "./sources/oledHandler.c" "./sources/peripheralsList.c"
    "./sources/rtcHandler.c" "./sources/sprites.c"
    INCLUDE_DIRS "./include/"
    REQUIRES esp_http_client esp_wifi nvs_flash json driver)