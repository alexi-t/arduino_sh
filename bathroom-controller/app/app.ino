// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>

#include "config.h"

#include <pins_arduino.h>

static bool messagePending = false;
static bool messageSending = true;

static char *connectionString;
static char *ssid;
static char *pass;

static int interval = INTERVAL;

void blinkYLED()
{
    digitalWrite(Y_LED_PIN, HIGH);
    delay(500);
    digitalWrite(Y_LED_PIN, LOW);
}

void blinkGLED()
{
    digitalWrite(Y_LED_PIN, HIGH);
    delay(500);
    digitalWrite(Y_LED_PIN, LOW);
}


static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
void setup()
{
    pinMode(Y_LED_PIN, OUTPUT);
    pinMode(G_LED_PIN, OUTPUT);

    pinMode(SR_PIN, INPUT);
    pinMode(DHT_PIN, INPUT);

    initSerial();
    delay(2000);
    readCredentials();

    initWifi();
    initTime();

    /*
     * AzureIotHub library remove AzureIoTHubClient class in 1.0.34, so we remove the code below to avoid
     *    compile error
    */

    // initIoThubClient();
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
        while (1);
    } 
    else 
    {
        Serial.printf("Created client from cs %s\r\n", connectionString);
    }


    IoTHubClient_LL_SetOption(iotHubClientHandle, "product_info", "bathroom-controller");
    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
    IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
    //IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);
}

static int messageCount = 1;
void loop()
{
    // if (!messagePending && messageSending)
    // {
    //     char messagePayload[MESSAGE_MAX_LEN];
    //     bool temperatureAlert = readMessage(messageCount, messagePayload);
    //     sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
    //     messageCount++;
    //     delay(interval);
    // }

    int srValue = digitalRead(SR_PIN);
    Serial.printf("%d\r\n", srValue);
    delay(2000);

    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(10);
}