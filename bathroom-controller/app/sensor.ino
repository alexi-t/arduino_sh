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