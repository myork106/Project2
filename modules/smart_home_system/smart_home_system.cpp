//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_home_system.h"

#include "siren.h"
#include "user_interface.h"
#include "fire_alarm.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "servo.h"          //included to initialize servo motor

//=====[Implementations of public functions]===================================

void smartHomeSystemInit()
{
    userInterfaceInit();
    fireAlarmInit();
    pcSerialComInit();
    servoInit();        //initialize servo with 20ms period at position 0 degrees
}

void smartHomeSystemUpdate()
{
    userInterfaceUpdate();
    fireAlarmUpdate();    
    pcSerialComUpdate();
    eventLogUpdate();
    delay(SYSTEM_TIME_INCREMENT_MS);
}
