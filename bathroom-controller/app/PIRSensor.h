#include <Arduino.h>

#ifndef PIRSensor_C
#define PIRSensor_C

class PIRSensor
{
  public:
    PIRSensor(int sensorPin);
    void init(void (*onMoveStartCallback)(), void (*onMoveEndCallback)());
    void check();

  private:
    int pirPin;          //вывод подключения PIR датчика
    int calibrationTime; //Время калибровки датчика (10-60 сек. по даташиту)
    boolean isInitializing;
    long unsigned int initStart;
    long unsigned int lowIn; //Время, в которое был принят сигнал отсутствия движения(LOW)
    long unsigned int pause; //Пауза, после которой движение считается оконченным
    boolean lockLow;         //Флаг. false = значит движение уже обнаружено, true - уже известно, что движения нет
    boolean takeLowTime;     //Флаг. Сигнализирует о необходимости запомнить время начала отсутствия движения

    void (*_onMoveStartCallback)();
    void (*_onMoveEndCallback)();
};

#endif