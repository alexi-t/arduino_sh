#include <pins_arduino.h>
// Physical device information for board and sensor
#define DEVICE_ID "Wemos D1"

// Pin layout configuration
#define Y_LED_PIN D14
#define G_LED_PIN D14

// Interval time(ms) for sending message to IoT Hub
#define INTERVAL 2000

// EEPROM address configuration
#define EEPROM_SIZE 512

// SSID and SSID password's length should < 32 bytes
// http://serverfault.com/a/45509
#define SSID_LEN 32
#define PASS_LEN 32
#define CONNECTION_STRING_LEN 256

#define MESSAGE_MAX_LEN 256
