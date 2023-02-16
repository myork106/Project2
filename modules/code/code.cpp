//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "code.h"

#include "user_interface.h"
#include "pc_serial_com.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

//=====[Declaration of external public global variables]=======================

extern char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
extern char codeSequenceFromPcSerialCom[CODE_NUMBER_OF_KEYS];
extern float incorrectTries;
//extrernal variable declared in user_interface
//overrides numberOfIncorrectCodes variable if still there have been less than three tries
//recorded in the user_interface

//=====[Declaration and initialization of private global variables]============

static int numberOfIncorrectCodes = 0;
static char codeSequence[CODE_NUMBER_OF_KEYS] = { '1', '8', '0', '5' };
static char printSequence[CODE_NUMBER_OF_KEYS + 1];
//new code sequence with null character

//=====[Declarations (prototypes) of private functions]========================

static bool codeMatch( char* codeToCompare );
static void codeDeactivate();

//=====[Implementations of public functions]===================================

//add null character to end of codeSequence to print to serial terminal
char* getCode() {
    printSequence[0] = codeSequence[0];
    printSequence[1] = codeSequence[1];
    printSequence[2] = codeSequence[2];
    printSequence[3] = codeSequence[3];
    printSequence[4] = '\0';
    return printSequence;
}

void codeWrite( char* newCodeSequence )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        codeSequence[i] = newCodeSequence[i];
    }
}

bool codeMatchFrom( codeOrigin_t codeOrigin )
{
    bool codeIsCorrect = false;
    switch (codeOrigin) {
        case CODE_KEYPAD:
            if( userInterfaceCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromUserInterface);
                userInterfaceCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                }
            }


        break;
        case CODE_PC_SERIAL:
            if( pcSerialComCodeCompleteRead() ) {
                codeIsCorrect = codeMatch(codeSequenceFromPcSerialCom);
                pcSerialComCodeCompleteWrite(false);
                if ( codeIsCorrect ) {
                    codeDeactivate();
                    pcSerialComStringWrite( "\r\nThe code is correct\r\n\r\n" );
                } else {
                    incorrectCodeStateWrite(ON);
                    numberOfIncorrectCodes++;
                    pcSerialComStringWrite( "\r\nThe code is incorrect\r\n\r\n" );
                }
            }

        break;
        default:
        break;
    }

    //number of incorrect codes allowed before blocking changed to 3
    if ( numberOfIncorrectCodes >= 3 ) {
        systemBlockedStateWrite(ON);
    }

    //if there are less than three incorrect codes detected by user_interface
    //override blocked state write to OFF
    if ( incorrectTries < 3 ) {
        systemBlockedStateWrite(OFF);
    }

    return codeIsCorrect;
}

//=====[Implementations of private functions]==================================

static bool codeMatch( char* codeToCompare )
{
    int i;
    for (i = 0; i < CODE_NUMBER_OF_KEYS; i++) {
        if ( codeSequence[i] != codeToCompare[i] ) {
            return false;
        }
    }
    return true;
}

static void codeDeactivate()
{
    systemBlockedStateWrite(OFF);
    incorrectCodeStateWrite(OFF);
    numberOfIncorrectCodes = 0;
}
