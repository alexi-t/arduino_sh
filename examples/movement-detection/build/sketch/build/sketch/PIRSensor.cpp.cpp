#include <Arduino.h>
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\movement-detection\\PIRSensor.cpp"
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\movement-detection\\PIRSensor.cpp"
#include "PIRSensor.h"

PIRSensor::PIRSensor(int sensorPin)
{
  pirPin = sensorPin;            //вывод подключения PIR датчика
  calibrationTime = 20000; //Время калибровки датчика (10-60 сек. по даташиту)
  isInitializing = false;
  initStart = 0;
  lowIn;          //Время, в которое был принят сигнал отсутствия движения(LOW)
  pause = 5000;   //Пауза, после которой движение считается оконченным
  lockLow = true; //Флаг. false = значит движение уже обнаружено, true - уже известно, что движения нет
  takeLowTime;
}

void PIRSensor::init(void (*onMoveStartCallback)(), void (*onMoveEndCallback)())
{
  pinMode(pirPin, INPUT);

  isInitializing = true;
  Serial.println("Start init");

  _onMoveStartCallback = onMoveStartCallback;
  _onMoveEndCallback = onMoveEndCallback;
}

void PIRSensor::check()
{
  if (isInitializing)
  {
    if (initStart == 0)
    {
      initStart = millis();
      Serial.print("Calibrating");
    }
    else
    {
      if (millis() - initStart > calibrationTime)
      {
        isInitializing = false;
        Serial.println("SENSOR ACTIVE");
      }
    }
    return;
  }

  //Если обнаружено движение
  if (digitalRead(pirPin) == HIGH)
  {
    Serial.println("SENSOR HIGH");
    if (lockLow)
    {
      lockLow = false;
      _onMoveStartCallback();
      delay(50);
    }
    takeLowTime = true;
  }

  //Ели движения нет
  if (digitalRead(pirPin) == LOW)
  {
    Serial.println("SENSOR LOW");
    if (takeLowTime)
    {
      lowIn = millis();    //Сохраним время окончания движения
      takeLowTime = false; //Изменим значения флага, чтобы больше не брать время, пока не будет нового движения
    }
    //Если время без движение превышает паузу => движение окончено
    if (!lockLow && millis() - lowIn > pause)
    {
      //Изменяем значение флага, чтобы эта часть кода исполнилась лишь раз, до нового движения
      lockLow = true;
      _onMoveEndCallback();
      delay(50);
    }
  }
}
#line 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\movement-detection\\app.ino"
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
