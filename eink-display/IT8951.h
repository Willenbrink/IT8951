#ifndef _IT8951_H_
#define _IT8951_H_

#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "miniGUI.h"

//Waveform Mode
#define IT8951_MODE_0   0
#define IT8951_MODE_1   1
#define IT8951_MODE_2   2
#define IT8951_MODE_3   3
#define IT8951_MODE_4   4
//Endian Type
#define IT8951_LDIMG_L_ENDIAN   0
#define IT8951_LDIMG_B_ENDIAN   1
//Auto LUT
#define IT8951_DIS_AUTO_LUT   0
#define IT8951_EN_AUTO_LUT    1
//LUT Engine Status
#define IT8951_ALL_LUTE_BUSY 0xFFFF

bool init(void);
void cancel(void);
void example1(void);
void example2(void);
void example3(void);
void IT8951_BMP_Example(uint32_t x, uint32_t y,char *path);

void gpio_i80_16b_cmd_out(uint16_t usCmd);
void GPIO_Configuration_Out(void);
void GPIO_Configuration_In(void);
#endif
