#define START_APP (1)

class StartAppLayout {
    public:

        void show(WifiService _wifiService) {
            wifiService = _wifiService;

            showSelectBtns();
        }

        void clickSwitchModeSetting() {
            //отключает режим настройки и начинает искать сеть
        }

        void clickSwitchModeWifi() {
            //останавливает поиск и переключает в режим настройки
        }
        
        void update() {
        }

        void click1() {
            Serial.println("click 1");
        }

        void click2() {
            if (isActive) {
                isActive = false;
                showSearchWifi();
            }
        }

        void click3() {
            if (isActive) {
                isActive = false;
                showAdminWeb();
            }
        }
    
    private: 
        char buf[128];

        uint8_t iconWifi;
        Rect_t area_zone_text_center;
        Rect_t area_zone_text_botton_center;
        Rect_t area_zone_text_botton_center2;

        bool isActive = true;

        WifiService wifiService;

        void setTextBottomCenter(char* text, char* text2 = nullptr) {
            if (&area_zone_text_botton_center != nullptr) {
                epd_clear_area(area_zone_text_botton_center);
            }
            if (&area_zone_text_botton_center2 != nullptr) {
                epd_clear_area(area_zone_text_botton_center2);
                area_zone_text_botton_center2 = {};
            }

            int x = (960 / 2) - (String(text).length() * 4);
            int y = 540 - 35;

            area_zone_text_botton_center = {
                .x = x - 14,
                .y = y - 34,
                .width = (int32_t) (x + ((String(text).length() * 7))),
                .height = 40,
            };

            writeln((GFXfont *)&osans12b, text, &x, &y, NULL);

            if (text2 != nullptr) {
                int y2 = 540 - 6;
                int x2 = (960 / 2) - (String(text2).length() * 4);

                area_zone_text_botton_center2 = {
                    .x = x2 - 14,
                    .y = y2 - 30,
                    .width = (int32_t) (x2 + ((String(text2).length() * 7))),
                    .height = 40,
                };

                writeln((GFXfont *)&osans12b, text2, &x2, &y2, NULL);
                
            }

        }

        void showAdminWeb() {
            free(framebuffer);
            
            epd_poweron();
            epd_clear();

            epd_draw_grayscale_image(epd_full_screen(), framebuffer);
            epd_poweroff();
        }

        void showSelectBtns() {
            free(framebuffer);

            epd_poweron();
            epd_clear();

            const char *overview[] = {
                "[1] Режим поиска сети WIFI и отображение погоды\n"\
                "[2] Режим админ панели, будет сгенерирован вход \n     в админ панель"
            };

            int top = -15;

            cursor_x = 440;
            cursor_y = 50 + top;

            writeln((GFXfont *)&osans12b, "1", &cursor_x, &cursor_y, framebuffer);

            cursor_x += 50;

            writeln((GFXfont *)&osans12b, "2", &cursor_x, &cursor_y, framebuffer);

            cursor_x = 430;
            cursor_y = 25 + top;
            
            epd_draw_rect(cursor_x, cursor_y, 30, 30, 0, framebuffer);

            cursor_x += 67;
            
            epd_draw_rect(cursor_x, cursor_y, 30, 30, 0, framebuffer);

            // выводим информацию...

            cursor_x = 50;
            cursor_y = 150;

            write_string((GFXfont *)&osans16b, overview[0], &cursor_x, &cursor_y, framebuffer);

            // cursor_x = 50;
            // cursor_y += 50;

            // writeln((GFXfont *)&osans16b, , &cursor_x, &cursor_y, framebuffer);

            epd_draw_grayscale_image(epd_full_screen(), framebuffer);

            epd_poweroff();

        }

        void showSearchWifi() {
            epd_poweron();
            epd_clear();

            setTextBottomCenter("Ищем подключение к сети...");

            showWifiIcon();

            if (connectWifi()) {
                setTextBottomCenter("Соединение установлено!");
                epd_poweroff();
                return;
            }

            setTextBottomCenter("Не удалось подключиться к сети", "Устройство переключилась в режим настройки!");
            epd_poweroff();
        }
        
        void setTextCenter(const char* text) {
            if (&area_zone_text_center != nullptr) {
                epd_clear_area(area_zone_text_center);
            }

            int y = 150 + IconWifiSmall_height + 35;
            int x = (960 / 2) - (String(text).length() * 11);

            area_zone_text_center = {
                .x = x - 14,
                .y = y - 34,
                .width = (int32_t) (x + ((String(text).length()) - 11)),
                .height = 40,
            };

            writeln((GFXfont *)&osans16b, text, &x, &y, NULL);
        }

        void showWifiIcon() {
            Rect_t area = {
                .x = 480 - (IconWifiSmall_width / 2),
                .y = 150,
                .width = IconWifiSmall_width,
                .height = IconWifiSmall_height,
            };

            epd_draw_image(area, (uint8_t *)IconWifiSmall_data, BLACK_ON_WHITE);
        }

        String stringS = "[";

        void hideProcessBar() {
            int cursor_x = 15;
            int cursor_y = 35;

            Rect_t area_zone = {
                .x = cursor_x,
                .y = cursor_y - 28,
                .width = 940,
                .height = 32,
            };

            epd_clear_area(area_zone);
            stringS = "[";
        }

        void showProcessBar() {
            hideProcessBar();

            int cursor_x = 20;
            int cursor_y = 35;

            const char *string2 = "[                                                                                                                                ]\n";
            
            writeln((GFXfont *)&osans12b, string2 , &cursor_x, &cursor_y, NULL);
        }


        bool setProcessBar(int i = 0) {

            int cursor_x = 20;
            int cursor_y = 35;

            if (i == 0) {
                stringS += "==============";
            } else {
                stringS += "==========";
            } 

            writeln((GFXfont *)&osans12b, stringS.c_str() , &cursor_x, &cursor_y, NULL);

            return i == 5;
        }

        bool connectWifi() {
            showProcessBar();

            if (!wifiService.nextConnect()) {
                return false;
            }

            WifiData wifi = wifiService.getCurrentWifi();
            
            Serial.println("search ... : " + wifi.ssid);

            setTextCenter(wifi.ssid.c_str());

            int step = 0;

            while(!setProcessBar(step)) {
                step++;
                
                if (wifiService.statusConnect()) {
                    hideProcessBar();
                    return true;
                }

                delay(3000);
            }

            hideProcessBar();

            return connectWifi();
        }
};