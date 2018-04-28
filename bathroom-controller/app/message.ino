#include <ArduinoJson.h>

void createLightMessage(int messageId, bool lightState, char *buffer)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;

    root["lightState"] = lightState;

    root.printTo(buffer, MESSAGE_MAX_LEN);
}

void createTempMessage(int messageId, float temp, float humidity, char *buffer)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;

    root["temp"] = temp;
    root["humidity"] = humidity;

    root.printTo(buffer, MESSAGE_MAX_LEN);
}