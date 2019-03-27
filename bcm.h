#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

bool initBCM(); //Call once
void waitForBus(); //Wait until bus is ready TODO check significance of this?
void openBus(); //Prepare for datatransfer
void closeBus(); //End datatransfer
uint16_t transfer(uint16_t value); //Send and receive data simultaneously
