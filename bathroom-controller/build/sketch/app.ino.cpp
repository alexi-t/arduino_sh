#include <Arduino.h>
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
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

#line 27 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void blinkYLED();
#line 34 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void blinkGLED();
#line 43 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void setup();
#line 86 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void onMoveStart();
#line 96 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void onMoveEnd();
#line 106 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void onTempMeasure(float temp, float humidity);
#line 115 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
void loop();
#line 4 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
void readCredentials();
#line 36 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
bool needEraseEEPROM();
#line 49 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
void clearParam();
#line 58 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
void EEPROMWrite(int addr, char *data, int size);
#line 76 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
int EEPROMread(int addr, char *buf);
#line 8 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
void initIO();
#line 14 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
void enableLight();
#line 22 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
void disableLight();
#line 32 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
void overrideLight();
#line 38 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
void clearLightOverride();
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\init.ino"
void initWifi();
#line 23 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\init.ino"
void initTime();
#line 6 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback);
#line 20 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *buffer);
#line 47 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
IOTHUBMESSAGE_DISPOSITION_RESULT receiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback);
#line 75 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
int deviceMethodCallback( const char *methodName, const unsigned char *payload, size_t size, unsigned char **response, size_t *response_size, void *userContextCallback);
#line 119 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
void twinCallback( DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, size_t size, void *userContextCallback);
#line 3 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\message.ino"
void createLightMessage(int messageId, bool lightState, char *buffer);
#line 16 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\message.ino"
void createTempMessage(int messageId, float temp, float humidity, char *buffer);
#line 14 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\sensor.ino"
void initSensors(void (*moveStart)(), void (*moveEnd)());
#line 32 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\sensor.ino"
void checkSensors(void (*onTempMeasure)(float temp, float humidity));
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\serialReader.ino"
void initSerial();
#line 17 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\serialReader.ino"
bool readFromSerial(char * prompt, char * buf, int maxLen, int timeout);
#line 27 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
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

    initSerial();
    delay(2000);
    
    initIO();

    initSensors(onMoveStart, onMoveEnd);

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
        while (1)
            ;
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

void onMoveStart()
{
    char messagePayload[MESSAGE_MAX_LEN];
    createLightMessage(messageCount, true, messagePayload);
    sendMessage(iotHubClientHandle, messagePayload);
    messageCount++;
    enableLight();
    delay(interval);
}

void onMoveEnd()
{
    char messagePayload[MESSAGE_MAX_LEN];
    createLightMessage(messageCount, false, messagePayload);
    sendMessage(iotHubClientHandle, messagePayload);
    messageCount++;
    disableLight();
    delay(interval);
}

void onTempMeasure(float temp, float humidity)
{
    char messagePayload[MESSAGE_MAX_LEN];
    createTempMessage(messageCount, temp, humidity, messagePayload);
    sendMessage(iotHubClientHandle, messagePayload);
    messageCount++;
    delay(interval);
}

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

    checkSensors(onTempMeasure);

    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(10);
}
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
#include <EEPROM.h>

// Read parameters from EEPROM or Serial
void readCredentials()
{
    int ssidAddr = 0;
    int passAddr = ssidAddr + SSID_LEN;
    int connectionStringAddr = passAddr + SSID_LEN;

    // malloc for parameters
    ssid = (char *)malloc(SSID_LEN);
    pass = (char *)malloc(PASS_LEN);
    connectionString = (char *)malloc(CONNECTION_STRING_LEN);

    // try to read out the credential information, if failed, the length should be 0.
    int ssidLength = EEPROMread(ssidAddr, ssid);
    int passLength = EEPROMread(passAddr, pass);
    int connectionStringLength = EEPROMread(connectionStringAddr, connectionString);

    if (ssidLength > 0 && passLength > 0 && connectionStringLength > 0 && !needEraseEEPROM())
    {
        return;
    }

    // read from Serial and save to EEPROM
    readFromSerial("Input your Wi-Fi SSID: ", ssid, SSID_LEN, 0);
    EEPROMWrite(ssidAddr, ssid, strlen(ssid));

    readFromSerial("Input your Wi-Fi password: ", pass, PASS_LEN, 0);
    EEPROMWrite(passAddr, pass, strlen(pass));

    readFromSerial("Input your Azure IoT hub device connection string: ", connectionString, CONNECTION_STRING_LEN, 0);
    EEPROMWrite(connectionStringAddr, connectionString, strlen(connectionString));
}

bool needEraseEEPROM()
{
    char result = 'n';
    readFromSerial("Do you need re-input your credential information?(Auto skip this after 5 seconds)[Y/n]", &result, 1, 5000);
    if (result == 'Y' || result == 'y')
    {
        clearParam();
        return true;
    }
    return false;
}

// reset the EEPROM
void clearParam()
{
    char data[EEPROM_SIZE];
    memset(data, '\0', EEPROM_SIZE);
    EEPROMWrite(0, data, EEPROM_SIZE);
}

#define EEPROM_END 0
#define EEPROM_START 1
void EEPROMWrite(int addr, char *data, int size)
{
    EEPROM.begin(EEPROM_SIZE);
    // write the start marker
    EEPROM.write(addr, EEPROM_START);
    addr++;
    for (int i = 0; i < size; i++)
    {
        EEPROM.write(addr, data[i]);
        addr++;
    }
    EEPROM.write(addr, EEPROM_END);
    EEPROM.commit();
    EEPROM.end();
}

// read bytes from addr util '\0'
// return the length of read out.
int EEPROMread(int addr, char *buf)
{
    EEPROM.begin(EEPROM_SIZE);
    int count = -1;
    char c = EEPROM.read(addr);
    addr++;
    if (c != EEPROM_START)
    {
        return 0;
    }
    while (c != EEPROM_END && count < EEPROM_SIZE)
    {
        c = (char)EEPROM.read(addr);
        count++;
        addr++;
        buf[count] = c;
    }
    EEPROM.end();
    return count;
}

#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\gpio.ino"
#include <pins_arduino.h>

static boolean lightOverride = false;
static boolean disableLightAfterOverrideClear = true;

const int lightPin = D7;

void initIO()
{
    pinMode(lightPin, OUTPUT);
    digitalWrite(lightPin, LOW);
}

void enableLight()
{
    digitalWrite(lightPin, HIGH);

    if (lightOverride)
        disableLightAfterOverrideClear = false;
}

void disableLight()
{
    if (lightOverride)
    {
        disableLightAfterOverrideClear = true;
        return;
    }
    digitalWrite(lightPin, LOW);
}

void overrideLight()
{
    lightOverride = true;
    digitalWrite(lightPin, HIGH);
}

void clearLightOverride()
{
    lightOverride = false;
    if (disableLightAfterOverrideClear)
        digitalWrite(lightPin, LOW);
}
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\init.ino"
void initWifi()
{
    // Attempt to connect to Wifi network:
    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, pass);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void initTime()
{
    time_t epochTime;
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
            break;
        }
    }
}
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
static WiFiClientSecure sslClient; // for ESP8266

const char *onSuccess = "\"Successfully invoke device method\"";
const char *notFound = "\"No method found\"";

static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        Serial.println("Message sent to Azure IoT Hub");
        blinkGLED();
    }
    else
    {
        Serial.println("Failed to send message to Azure IoT Hub");
    }
    messagePending = false;
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *buffer)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char *)buffer, strlen(buffer));
    if (messageHandle == NULL)
    {
        Serial.println("Unable to create a new IoTHubMessage.");
    }
    else
    {
        //MAP_HANDLE properties = IoTHubMessage_Properties(messageHandle);
        //Map_Add(properties, "temperatureAlert", temperatureAlert ? "true" : "false");
        Serial.printf("Sending message: %s.\r\n", buffer);
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            Serial.println("Failed to hand over the message to IoTHubClient.");
        }
        else
        {
            blinkYLED();
            messagePending = true;
            Serial.println("IoTHubClient accepted the message for delivery.");
        }

        IoTHubMessage_Destroy(messageHandle);
    }
}

IOTHUBMESSAGE_DISPOSITION_RESULT receiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void *userContextCallback)
{
    IOTHUBMESSAGE_DISPOSITION_RESULT result;
    const unsigned char *buffer;
    size_t size;
    if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
    {
        Serial.println("Unable to IoTHubMessage_GetByteArray.");
        result = IOTHUBMESSAGE_REJECTED;
    }
    else
    {
        /*buffer is not zero terminated*/
        char *temp = (char *)malloc(size + 1);

        if (temp == NULL)
        {
            return IOTHUBMESSAGE_ABANDONED;
        }

        strncpy(temp, (const char *)buffer, size);
        temp[size] = '\0';
        Serial.printf("Receive C2D message: %s.\r\n", temp);
        free(temp);
    }
    return IOTHUBMESSAGE_ACCEPTED;
}

int deviceMethodCallback(
    const char *methodName,
    const unsigned char *payload,
    size_t size,
    unsigned char **response,
    size_t *response_size,
    void *userContextCallback)
{
    Serial.printf("Try to invoke method %s.\r\n", methodName);
    const char *responseMessage = onSuccess;
    int result = 200;

    if (strcmp(methodName, "enableLight") == 0)
    {
        overrideLight();
        Serial.println("Enable light");
    }
    else if (strcmp(methodName, "disableLight") == 0)
    {
        clearLightOverride();
        Serial.println("Disable light");
    }
    else if (strcmp(methodName, "enableVent") == 0)
    {
        Serial.println("Enable vent");
    }
    else if (strcmp(methodName, "disableVent") == 0)
    {
        Serial.println("Disable vent");
    }
    else
    {
        Serial.printf("No method %s found.\r\n", methodName);
        responseMessage = notFound;
        result = 404;
    }

    *response_size = strlen(responseMessage);
    *response = (unsigned char *)malloc(*response_size);
    strncpy((char *)(*response), responseMessage, *response_size);

    return result;
}

void twinCallback(
    DEVICE_TWIN_UPDATE_STATE updateState,
    const unsigned char *payLoad,
    size_t size,
    void *userContextCallback)
{
    char *temp = (char *)malloc(size + 1);
    for (int i = 0; i < size; i++)
    {
        temp[i] = (char)(payLoad[i]);
    }
    temp[size] = '\0';
    //parseTwinMessage(temp);
    free(temp);
}

#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\message.ino"
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
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\sensor.ino"
#include <DHT.h>
#include "PIRSensor.h"

int pirPin = D12;
int lastTempMeasureTime;
int dhtPeriod = 20000;

#define DHTPIN D11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
PIRSensor pir(pirPin);

void initSensors(void (*moveStart)(), void (*moveEnd)())
{
  Serial.println("Start init sensors");
  //init PIR
  pinMode(pirPin, INPUT);
  pir.init(moveStart, moveEnd);
  Serial.println("PIR init OK");

  //init DHT
  pinMode(DHTPIN, INPUT);
  dht.begin();
  Serial.println("DHT init OK");

  Serial.println("End init sensors");

  lastTempMeasureTime = millis();
}

void checkSensors(void (*onTempMeasure)(float temp, float humidity))
{
  pir.check();

  if (millis() - lastTempMeasureTime > dhtPeriod)
  {
    lastTempMeasureTime = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    onTempMeasure(t, h);
  }
}
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\serialReader.ino"
void initSerial()
{
    // Start serial and initialize stdout
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("Serial successfully inited.");
}

/* Read a string whose length should in (0, lengthLimit) from Serial and save it into buf.
 *
 *        prompt   - the interact message and buf should be allocaled already and return true.
 *        buf      - a part of memory which is already allocated to save string read from serial
 *        maxLen   - the buf's len, which should be upper bound of the read-in string's length, this should > 0
 *        timeout  - If after timeout(ms), return false with nothing saved to buf.
 *                   If no timeout <= 0, this function will not return until there is something read.
 */
bool readFromSerial(char * prompt, char * buf, int maxLen, int timeout)
{
    int timer = 0, delayTime = 1000;
    String input = "";
    if(maxLen <= 0)
    {
        // nothing can be read
        return false;
    }

    Serial.println(prompt);
    while(1)
    {
        input = Serial.readString();
        int len = input.length();
        if(len > maxLen)
        {
            Serial.printf("Your input should less than %d character(s), now you input %d characters.\r\n", maxLen, len);
        }
        else if (len > 0)
        {
            // save the input into the buf
            sprintf(buf, "%s", input.c_str());
            return true;
        }

        // if timeout, return false directly
        timer += delayTime;
        if(timeout > 0 && timer >= timeout)
        {
            Serial.println("You input nothing, skip...");
            return false;
        }
        // delay a time before next read
        delay(delayTime);
    }
}

