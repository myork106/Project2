//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "servo.h"

//=====[Declaration of private defines]========================================

#define DUTY_MIN        0.025       //sets minimum duty cycle to 0 degrees
#define DUTY_MAX        0.075       //sets maximum duty cycle to 90 degrees
#define PERIOD          0.02        //sets period to 20ms

//=====[Declaration of public global objects]=====================================

PwmOut servo(PF_9);                 //Declares PF_9 pin to a PWM output called servo

//=====[Implementations of public functions]========================

//initializes servo to 20ms period at position 0 degrees
void servoInit() {
    servo.period(PERIOD);
    servo.write(DUTY_MIN);
}

//writes servo position to 90 degrees
void servoOpen() {
    servo.write(DUTY_MAX);
}

//writes servo position to 0 degrees
void servoClose() {
    servo.write(DUTY_MIN);
}
