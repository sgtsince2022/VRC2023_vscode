#include "unistd.h"
#include <Arduino.h>
#include "esp32-hal-ledc.h"

#ifndef _ESP32_MOTOR_H_
#define _ESP32_MOTOR_H_

class esp32_motor
{
private:
    /* data */
public:
    uint8_t forward, backward;
    int maxPWM;
    esp32_motor(int8_t, int8_t, int8_t, int8_t, int, int);
    void Run(int PWM);
    void Stop();
};


#endif