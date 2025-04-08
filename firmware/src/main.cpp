#include <Arduino.h>
#include <ESPRotary.h>
#include <Button2.h>
#include <Wire.h>
#include "Settings.hpp"
#include "Display.hpp"
#include "Light.hpp"
#include "Ui.hpp"
#include "Controller.hpp"
#include "constants.hpp"

#ifdef USE_WIFI
#include <WiFi.h>
static const char *TAG_WIFI = "WIFI";
#endif
#ifdef USE_WIFI_OTA
#include <ArduinoOTA.h>
#endif

ESPRotary rotaryEncoder;
Button2 button;

void setup()
{
    Serial.begin(115200);

    SETTINGS.begin();

#ifdef USE_WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGI(TAG_WIFI, "Wait Wifi");
        delay(500);
    }

    ESP_LOGI(TAG_WIFI, "%s", WiFi.localIP().toString());
#endif

#ifdef USE_WIFI_OTA
    ArduinoOTA.setRebootOnSuccess(true);
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setMdnsEnabled(true);
    ArduinoOTA.setPassword(OTA_PASS.c_str());
    ArduinoOTA.begin();

    ArduinoOTA.onStart([]() {
        CTRL.off(true);
    });
#endif

    Wire.begin();
    DISP.begin();
    LIGHT.begin();
    CTRL.begin();
    button.begin(ENCODER_SW);
    rotaryEncoder.begin(ENCODER_A, ENCODER_B, 4);

    button.setClickHandler([](Button2 &btn) { 
        UI.click();
    });
    button.setLongClickHandler([](Button2 &btn) { 
        UI.longClick();
    });
    rotaryEncoder.setLeftRotationHandler([](ESPRotary &r) { 
        UI.down();
    });
    rotaryEncoder.setRightRotationHandler([](ESPRotary &r) {
        UI.up();
    });
}

void loop()
{
#ifdef USE_WIFI_OTA
    ArduinoOTA.handle();
#endif
    button.loop();
    rotaryEncoder.loop();
    CTRL.loop();
    LIGHT.loop();
    DISP.loop();
}
