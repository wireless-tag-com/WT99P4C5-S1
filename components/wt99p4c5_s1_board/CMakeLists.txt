
idf_component_register(
    SRCS "wt99p4c5_s1_board.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES driver vfs fatfs esp_eth esp_netif esp_event
    PRIV_REQUIRES esp_lcd usb spiffs
)
