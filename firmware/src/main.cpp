#include <Arduino.h>
#include <ESPRotary.h>
#include <Button2.h>
#include <Wire.h>
#include "Display.hpp"
#include "Light.hpp"
#include "Controller.hpp"
#include "constants.hpp"

#ifdef WIFI
#include <WiFi.h>
static const char *TAG_WIFI = "WIFI";
#endif
#ifdef USE_WIFI_OTA
#include <ArduinoOTA.h>
#endif

Display display;
Light light;
ESPRotary rotaryEncoder;
Button2 button;
Controller ctrl(&display, &light);

void setup()
{
    Serial.begin(115200);

#ifdef WIFI
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
        ctrl.off(true);
        display.on();
        display.print("Updating...");
    });
#endif

    Wire.begin();
    display.begin();
    light.begin();
    ctrl.begin();
    button.begin(ENCODER_SW);
    rotaryEncoder.begin(ENCODER_A, ENCODER_B, 4);

    button.setClickHandler([](Button2 &btn) { 
        ctrl.click();
    });
    button.setLongClickHandler([](Button2 &btn) { 
        ctrl.longClick();
    });
    rotaryEncoder.setLeftRotationHandler([](ESPRotary &r) { 
        ctrl.up();
    });
    rotaryEncoder.setRightRotationHandler([](ESPRotary &r) {
        ctrl.down();
    });
}

void loop()
{
#ifdef USE_WIFI_OTA
    ArduinoOTA.handle();
#endif
    button.loop();
    rotaryEncoder.loop();
    ctrl.loop();
    light.loop();
    display.loop();
}
