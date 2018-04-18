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