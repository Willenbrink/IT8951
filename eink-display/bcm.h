#include <stdint.h>
#include <stdbool.h>

/*
 * First level of abstraction: Transfer bytes to and from controller
 */

bool initBCM(); //Call once
void freeBCM();
void waitForBus(); //Wait until bus is ready TODO check significance of this?
void openBus(); //Prepare for datatransfer
void closeBus(); //End datatransfer
uint16_t transfer(uint16_t value); //Send and receive data simultaneously
