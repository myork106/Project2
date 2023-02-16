//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "code.h"
#include "siren.h"
#include "smart_home_system.h"
#include "fire_alarm.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"
#include "display.h"
#include "servo.h"          //included to write servo position for a correct code

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
float incorrectTries = 0.0;
//counts incorrect tries attempted on the gated entryway matrix keypad
static float count = 0.0;
//counts number of times the countdown has iterated to calculated time remaining in the countdown

//=====[Declaration and initialization of private global variables]============

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

static bool codeComplete = false;
static int numberOfCodeChars = 0;
static int accumulatedTime = 0;
//counts accumulated time for screens to delay for the proper amount of time without affecting system responsiveness

//=====[Declarations (prototypes) of private functions]========================

static void incorrectCodeIndicatorUpdate();
static void systemBlockedIndicatorUpdate();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
    userInterfaceDisplayUpdate();
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

bool userInterfaceCodeCompleteRead()
{
    return codeComplete;
}

void userInterfaceCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_CONNECTION_GPIO_4BITS );
    
    //if the system is blocked, change the initialized screen to 'system blocked'
    if (systemBlockedLed) {
        displayCharPositionWrite(0,0);
        displayStringWrite("     SYSTEM     ");

        displayCharPositionWrite(0,1);
        displayStringWrite("    BLOCKED     ");
    } 
    //otherwise display home screen to enter code
    else {
        displayCharPositionWrite ( 0,0 );
        displayStringWrite( "Enter Code to   " );

        displayCharPositionWrite ( 0,1 );
        displayStringWrite( "Open Gate:      " );
    }
}

static void userInterfaceDisplayUpdate()
{
    char codeString[1] = "";
    //stores single input from matrix keypad to write to codeSequenceFromUserInterface and LCD

    static int codeAttempt = 0;
    //true if 4 digit code is input, false if no valid code entered
    static bool closing = 0;
    //true if system is closing gate, false if gate is not closing
    static bool wrong = 0;
    //true if system is displaying the code is wrong, false if no wrong code is detected
    static int index = 0;
    //writes inputs to correct index of codeSequenceFromUserInterface, increments cursor one place to the right
    //after an input, checks if 4 inputs have been entered
    static float countDown = 0.0;
    //displays how many seconds the gate will remain open
    static float count;
    //counts number of times the countdown has iterated to calculated time remaining in the countdown
    static char str[2];
    //character used to convert float type to character type in sprintf() function
    static float triesLeft = 0.0;
    //displays how many tries remain before blocking the system

    codeString[0] = matrixKeypadUpdate();
    //stores matrix keypad input

    //if the keypad has inputs being entered, the system is not blocked, or the screen isn't displaying 
    //that the servo is closing or the code is wrong, then store and display matrix keypad input
    if ((codeString[0] != '\0') && (!systemBlockedLed) && (codeAttempt == 0) && (closing == 0) && (wrong == 0)) {
        userInterfaceCodeCompleteWrite(true);
        codeSequenceFromUserInterface[index] = codeString[0];
        //store input
        displayCharPositionWrite ( 11 + index , 1 );
        //increment cursor for each registered input
        displayStringWrite( codeString );
        //display input
        index++;
        //increment index
    }
    //if four buttons are pressed one after another, a code is being attempted
    if (index >= 4) {
        codeAttempt = 1;
        //indicate a code is attempted
        index = 0;
        //reset index
        countDown = 0.0;
        //reset countdown
    }

    //if a code is attempted, wait 1 second
    if ((codeAttempt == 1) && (accumulatedTime >= 1000)) {
        accumulatedTime = 0;
        //reset accumulated time

        //if the code is correct, open the gate and start the countdown
        if (codeMatchFrom( CODE_KEYPAD ) || (countDown > 0)) {
            servoOpen();
            //open gate
            displayCharPositionWrite ( 0,0 );
            displayStringWrite("      OPEN      ");
            displayCharPositionWrite( 0,1 );
            displayStringWrite("  CLOSING IN    ");
            countDown = 10 - count;
            displayCharPositionWrite( 13,1 );
            displayStringWrite("   ");
            displayCharPositionWrite( 13,1 );
            sprintf(str, "%.0f", countDown);
            displayStringWrite(str);
            //write countdown timer
            count++;
            //increment countdown timer

            //if the countdown reaches zero, close the gate
            if (count == 11) {
                closing = 1;
                //indicate the gate is closing
                count = 0.0;
                //reset count
                displayCharPositionWrite( 0,1 );
                displayStringWrite("                ");
                displayCharPositionWrite( 0,0 );
                displayStringWrite("    CLOSING     ");
                servoClose();
                //close gate
                incorrectTries = 0;
                //reset incorrect tries
                codeAttempt = 0;
                //indicate the code attempt is finished
                accumulatedTime = 0;
                //reset accumulated time
            }

        //display that the code is wrong, the tries remaining, and increment the tries attempted
        } else {
            wrong = 1;
            //indicate the wrong code was entered
            incorrectTries++;
            //increment incorrect attempts
            triesLeft = 3 - incorrectTries;
            //calculate the attempts left before blockign
            displayCharPositionWrite ( 0,0 );
            displayStringWrite("     WRONG      ");
            displayCharPositionWrite ( 0,1 );
            //if 1 try left print 'try left'
            if (triesLeft == 1) {
                displayStringWrite("    Try Left    ");
            } 
            //otherwise print 'tries left'
            else {
                displayStringWrite("    Tries Left  ");
            }
            displayCharPositionWrite( 2,1 );
            sprintf(str, "%.0f", triesLeft);
            displayStringWrite(str);
            //display how many tries remain before blocking

            //if three tries attempted, update system to blocked
            if (incorrectTries >= 3 ) {
                systemBlockedStateWrite(true);
                systemBlockedIndicatorUpdate();
            }
            codeAttempt = 0;
            //indicate the code attempt is finished
            accumulatedTime = 0;
            //reset timer

        }
    //if code is finished being checked and gate closing, wait 1 second before re-initializing the display
    //or if code is finished being attempted and code is wrong, wait 3 seconds before re-initializing the display
    } else if (((closing == 1) && (accumulatedTime >= 1000 )) || ((wrong == 1) && (accumulatedTime >= 3000))) {
        closing = 0;
        //indicate gate is no longer closing
        wrong = 0;
        //indicate code is done displaying it is wrong
        userInterfaceDisplayInit();
        //re-initialize display
        accumulatedTime = 0;
        //reset acculumated time
    
    //if code is attempted, the gate is closing, or the code is wrong when desired delay has not been reached
    //increment the accumulated time
    } else if ((codeAttempt == 1) || (closing == 1) || (wrong == 1)) {
        accumulatedTime = accumulatedTime + SYSTEM_TIME_INCREMENT_MS;
    }
}

static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}
