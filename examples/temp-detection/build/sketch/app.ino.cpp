#include <Arduino.h>
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"

#define DHT_DEBUG

#include <DHT.h>
#include <pins_arduino.h>


#define DHTPIN D11
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#line 13 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"
void setup();
#line 21 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"
void loop();
#line 13 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"
void setup()
{
  Serial.begin(115200);
  pinMode(DHTPIN, INPUT);

  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.printf("H: %f\r\n", h);
  Serial.printf("T: %f\r\n", t);

  delay(1000);
}
