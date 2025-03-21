#include <Arduino.h>
#include <ESPRotary.h>
#include <Button2.h>
#include <Wire.h>
#include "Display.hpp"
#include "Controller.hpp"
#include "constants.hpp"
#include "secrets.hpp"

#ifdef WIFI_OTA
#include <WiFi.h>
#include <ArduinoOTA.h>
#endif

Display display;
ESPRotary rotaryEncoder;
Button2 button;
Controller ctrl(&display);

void setup()
{
    Serial.begin(9600);

#ifdef WIFI_OTA
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Wait Wifi");
        delay(500);
    }

    Serial.println(WiFi.localIP());

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
    ctrl.loop();
    display.loop();
}
