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
        ESP_LOGI(TAG_HA, "Update");

        http.useHTTP10(true);
        http.begin(client, HA_URL);
        http.addHeader("Authorization", HA_TOKEN);
        http.GET();

        doc.clear();
        deserializeJson(doc, http.getStream());

        String message = doc["state"].as<String>();

        http.end();

        ESP_LOGI(TAG_HA, "Message: %s", message.c_str());

        return message;
    }
};
