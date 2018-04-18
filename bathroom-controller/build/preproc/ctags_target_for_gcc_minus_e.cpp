# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

# 7 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2
# 8 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2
# 9 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2

# 11 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2
# 12 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2
# 13 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2

# 15 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino" 2

static bool messagePending = false;
static bool messageSending = true;

static char *connectionString;
static char *ssid;
static char *pass;

static int interval = 2000;

void blinkYLED()
{
    digitalWrite(13, 0x1);
    delay(500);
    digitalWrite(13, 0x0);
}

void blinkGLED()
{
    digitalWrite(13, 0x1);
    delay(500);
    digitalWrite(13, 0x0);
}


static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
void setup()
{
    pinMode(13, 0x01);
    pinMode(12, 0x01);

    pinMode(10, 0x00);
    pinMode(9, 0x00);

    initSerial();
    delay(2000);
    readCredentials();

    initWifi();
    initTime();

    /*

     * AzureIotHub library remove AzureIoTHubClient class in 1.0.34, so we remove the code below to avoid

     *    compile error

    */
# 61 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\app.ino"
    // initIoThubClient();
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == __null)
    {
        Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
        while (1);
    }
    else
    {
        Serial.printf("Created client from cs %s\r\n", connectionString);
    }


    IoTHubClient_LL_SetOption(iotHubClientHandle, "product_info", "bathroom-controller");
    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, __null);
    IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, __null);
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

    int srValue = digitalRead(10);
    Serial.printf("%d\r\n", srValue);
    delay(2000);

    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(10);
}
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino"
# 2 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\EEPROM.ino" 2

// Read parameters from EEPROM or Serial
void readCredentials()
{
    int ssidAddr = 0;
    int passAddr = ssidAddr + 32;
    int connectionStringAddr = passAddr + 32;

    // malloc for parameters
    ssid = (char *)malloc(32);
    pass = (char *)malloc(32);
    connectionString = (char *)malloc(256);

    // try to read out the credential information, if failed, the length should be 0.
    int ssidLength = EEPROMread(ssidAddr, ssid);
    int passLength = EEPROMread(passAddr, pass);
    int connectionStringLength = EEPROMread(connectionStringAddr, connectionString);

    if (ssidLength > 0 && passLength > 0 && connectionStringLength > 0 && !needEraseEEPROM())
    {
        return;
    }

    // read from Serial and save to EEPROM
    readFromSerial("Input your Wi-Fi SSID: ", ssid, 32, 0);
    EEPROMWrite(ssidAddr, ssid, strlen(ssid));

    readFromSerial("Input your Wi-Fi password: ", pass, 32, 0);
    EEPROMWrite(passAddr, pass, strlen(pass));

    readFromSerial("Input your Azure IoT hub device connection string: ", connectionString, 256, 0);
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
    char data[512];
    memset(data, '\0', 512);
    EEPROMWrite(0, data, 512);
}



void EEPROMWrite(int addr, char *data, int size)
{
    EEPROM.begin(512);
    // write the start marker
    EEPROM.write(addr, 1);
    addr++;
    for (int i = 0; i < size; i++)
    {
        EEPROM.write(addr, data[i]);
        addr++;
    }
    EEPROM.write(addr, 0);
    EEPROM.commit();
    EEPROM.end();
}

// read bytes from addr util '\0'
// return the length of read out.
int EEPROMread(int addr, char *buf)
{
    EEPROM.begin(512);
    int count = -1;
    char c = EEPROM.read(addr);
    addr++;
    if (c != 1)
    {
        return 0;
    }
    while (c != 0 && count < 512)
    {
        c = (char)EEPROM.read(addr);
        count++;
        addr++;
        buf[count] = c;
    }
    EEPROM.end();
    return count;
}
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\init.ino"
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
        epochTime = time(__null);

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
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\iothubClient.ino"
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

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *buffer, bool temperatureAlert)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char *)buffer, strlen(buffer));
    if (messageHandle == __null)
    {
        Serial.println("Unable to create a new IoTHubMessage.");
    }
    else
    {
        //MAP_HANDLE properties = IoTHubMessage_Properties(messageHandle);
        //Map_Add(properties, "temperatureAlert", temperatureAlert ? "true" : "false");
        Serial.printf("Sending message: %s.\r\n", buffer);
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, __null) != IOTHUB_CLIENT_OK)
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

        if (temp == __null)
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
        blinkGLED();
        //start();
        Serial.println("Enable light");
    }
    else if (strcmp(methodName, "disableLight") == 0)
    {
        blinkYLED();
        //stop();
        Serial.println("Disable light");
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
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\message.ino"
# 2 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\message.ino" 2

void createLightMessage(int messageId, bool lightState, char *buffer)
{
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    root["deviceId"] = "Wemos D1";
    root["messageId"] = messageId;

    root["lightState"] = lightState;

    root.printTo(buffer, 256);
}
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\sensor.ino"
// #include <DHT.h>

// static DHT dht(DHT_PIN, DHT_TYPE);

// void initSensor()
// {
//     dht.begin();
// }

// float readTemperature()
// {
//     return dht.readTemperature();
// }

// float readHumidity()
// {
//     return dht.readHumidity();
// }
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\serialReader.ino"
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
# 17 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\bathroom-controller\\app\\serialReader.ino"
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
