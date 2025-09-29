#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.hpp"
#include "model.hpp"
#include "secrets.hpp"

static const char *TAG_HA = "HA";

class HaSensor
{
private:
    WiFiClient client;
    HTTPClient http;
    JsonDocument doc;

public:
    const void update(HaData &data)
    {
        ESP_LOGI(TAG_HA, "Update message");

        http.useHTTP10(true);
        http.begin(client, HA_URL + HA_SENSOR);
        http.addHeader("Authorization", HA_TOKEN);
        http.GET();

        doc.clear();
        deserializeJson(doc, http.getStream());

        #ifdef USE_HA_OCCUPANCY
        data.atHome = doc["attributes"]["at_home"].as<bool>();
        #endif
        #ifdef USE_HA_MESSAGE
        data.message = doc["attributes"]["message"].as<String>();
        #endif

        http.end();

        #ifdef USE_HA_OCCUPANCY
        ESP_LOGI(TAG_HA, "At home: %d", data.atHome);
        #endif
        #ifdef USE_HA_MESSAGE
        ESP_LOGI(TAG_HA, "Message: %s", data.message.c_str());
        #endif
    }
};
