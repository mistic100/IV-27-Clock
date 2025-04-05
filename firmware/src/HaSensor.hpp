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
    const String getMessage()
    {
        ESP_LOGI(TAG_HA, "Update message");

        http.useHTTP10(true);
        http.begin(client, HA_URL + HA_SENSOR_MESSAGE);
        http.addHeader("Authorization", HA_TOKEN);
        http.GET();

        doc.clear();
        deserializeJson(doc, http.getStream());

        String message = doc["state"].as<String>();

        http.end();

        ESP_LOGI(TAG_HA, "Message: %s", message.c_str());

        return message;
    }

    const bool getOccupancy() {
        ESP_LOGI(TAG_HA, "Update occupancy");

        http.useHTTP10(true);
        http.begin(client, HA_URL + HA_SENSOR_ZONE);
        http.addHeader("Authorization", HA_TOKEN);
        http.GET();

        doc.clear();
        deserializeJson(doc, http.getStream());

        uint8_t occupancy = doc["state"].as<uint8_t>();

        http.end();

        ESP_LOGI(TAG_HA, "Occupancy: %d", occupancy);

        return occupancy > 0;
    }
};
