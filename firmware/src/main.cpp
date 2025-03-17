#include <Arduino.h>
#include <ESPRotary.h>
#include <Button2.h>
#include <Wire.h>
#include "Display.hpp"
#include "Controller.hpp"

Display display;
ESPRotary rotaryEncoder;
Button2 button;
Controller ctrl(&display);

void setup()
{
    Serial.begin(9600);
    
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
    button.loop();
    rotaryEncoder.loop();
    ctrl.loop();
    display.loop();
}
