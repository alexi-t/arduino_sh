//Henry's Bench
// HC-SR501 Motion Detector
// Sample Sketch

#include <pins_arduino.h>
#include "PIRSensor.h"

int ledPin = D14;
int pirPin = D12;      

PIRSensor pir(pirPin);

void enableLight()
{
  Serial.println("Motion detected");
  digitalWrite(ledPin, HIGH);
}
void disableLight()
{
  Serial.println("Motion finished");
  digitalWrite(ledPin, LOW);
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  pir.init(enableLight, disableLight);
}

void loop()
{
  pir.check();
}