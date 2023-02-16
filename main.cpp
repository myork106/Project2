//ECE-218 Project 1: Remote Room Temperature Control
//Michael York and Emma Watson

//Password-protected gated entryway addition to smart home security system
//LCD prompts user for 4 digit code to be entered on matrix keypad
//LCD displays code on screen as it is entered
//After code is entered, it is checked against preset code (1805)
//If code matches, gate opens (simulated by servo motor)
//10sec countdown displays, gate closes, and incorrect tries reset
//If code does not match, display indicates the code is wrong and shows how many attempts remain before it blocks
//If three consecutive incorrect codes are input, the system blocks with no further input allowed

//=====[Libraries]=============================================================

#include "smart_home_system.h"

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    smartHomeSystemInit();
    while (true) {
        smartHomeSystemUpdate();
    }
}
