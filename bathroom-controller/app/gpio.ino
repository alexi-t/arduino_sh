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