#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "constants.hpp"
#include "model.hpp"
#include "secrets.hpp"

static const char *TAG = "SENSORS";

class HaSensor
{
private:
    WiFiClient client;
    HTTPClient http;
    JsonDocument doc;

    String message;

public:
    void loop()
    {
        EVERY_N_MILLIS(HA_UPDATE_INTERVAL_MS)
        {
            updateData();
        }
    }

    const String &getMessage() const
    {
        return message;
    }

private:
    void updateData()
    {
        ESP_LOGI(TAG, "Update");

        http.useHTTP10(true);
        http.begin(client, HA_URL);
        http.addHeader("Authorization", HA_TOKEN);
        http.GET();

        doc.clear();
        deserializeJson(doc, http.getStream());

        message = doc["state"].as<String>();

        http.end();

        ESP_LOGI(TAG, "Message: %s", message.c_str());
    }
};
