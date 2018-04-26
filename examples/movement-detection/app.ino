//Henry's Bench
// HC-SR501 Motion Detector
// Sample Sketch

#include <pins_arduino.h>

//Тестировалось на Arduino IDE 1.0.1
//Время калибровки датчика (10-60 сек. по даташиту)
int calibrationTime = 30;
//Время, в которое был принят сигнал отсутствия движения(LOW)
long unsigned int lowIn;
//Пауза, после которой движение считается оконченным
long unsigned int pause = 5000;
//Флаг. false = значит движение уже обнаружено, true - уже известно, что движения нет
boolean lockLow = true;
//Флаг. Сигнализирует о необходимости запомнить время начала отсутствия движения
boolean takeLowTime;
int pirPin = D12; //вывод подключения PIR датчика
int ledPin = D14; //вывод сигнального диода

void setup()
{
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  //дадим датчику время на калибровку
  Serial.print("Calibrating");
  for(int i = 0; i < calibrationTime; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop()
{
  //Если обнаружено движение
  if(digitalRead(pirPin) == HIGH)
  {
    Serial.println("PIN HIGH");
    //Если еще не вывели информацию об обнаружении
    if(lockLow)
    {
      lockLow = false;
      Serial.println("Motion detected");
      digitalWrite(ledPin, HIGH);
      delay(50);
    }
    takeLowTime = true;
  }

  //Ели движения нет
  if(digitalRead(pirPin) == LOW)
  {
    Serial.println("PIN LOW");
    //Если время окончания движения еще не записано
    if(takeLowTime)
    {
      lowIn = millis();          //Сохраним время окончания движения
      takeLowTime = false;       //Изменим значения флага, чтобы больше не брать время, пока не будет нового движения
    }
    //Если время без движение превышает паузу => движение окончено
    if(!lockLow && millis() - lowIn > pause)
    {
      //Изменяем значение флага, чтобы эта часть кода исполнилась лишь раз, до нового движения
      lockLow = true;
      Serial.println("Motion finished");
      digitalWrite(ledPin, LOW);
      delay(50);
    }
  }
}