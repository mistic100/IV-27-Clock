#include <Arduino.h>
#include <ESPRotary.h>
#include <Button2.h>
#include <Wire.h>
#include "Display.hpp"
#include "Controller.hpp"
#include "constants.hpp"
#include "secrets.hpp"

#ifdef WIFI
#include <WiFi.h>
static const char *TAG_WIFI = "WIFI";
#endif
#ifdef WIFI_OTA
#include <ArduinoOTA.h>
#endif
#ifdef HA_MESSAGE
#include "HaSensor.hpp"
#endif

Display display;
ESPRotary rotaryEncoder;
Button2 button;
#ifdef HA_MESSAGE
HaSensor haSensor;
Controller ctrl(&display, &haSensor);
#else
Controller ctrl(&display);
#endif

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

#ifdef WIFI_OTA
    ArduinoOTA.setRebootOnSuccess(true);
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setMdnsEnabled(true);
    ArduinoOTA.setPassword(OTA_PASS.c_str());
    ArduinoOTA.begin();
#endif

    Wire.begin();

    display.begin();

    ctrl.begin();

    button.begin(D3);
    button.setClickHandler([](Button2 &btn)
                           { ctrl.click(); });
    button.setLongClickHandler([](Button2 &btn)
                               { ctrl.longClick(); });

    rotaryEncoder.begin(D1, D2, 4);
    rotaryEncoder.setLeftRotationHandler([](ESPRotary &r)
                                         { ctrl.up(); });
    rotaryEncoder.setRightRotationHandler([](ESPRotary &r)
                                          { ctrl.down(); });
}

void loop()
{
#ifdef WIFI_OTA
    ArduinoOTA.handle();
#endif

    button.loop();

    rotaryEncoder.loop();

#ifdef HA_MESSAGE
    haSensor.loop();
#endif

    ctrl.loop();

    display.loop();
}
