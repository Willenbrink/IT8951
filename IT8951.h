#ifndef _IT8951_H_
#define _IT8951_H_

#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "miniGUI.h"

#define CS 8
#define HRDY 24
#define RESET 17
#define VCOM 1500 //e.g. -1.53 = 1530 = 0x5FA

typedef enum
{
 ROTATE_0 = 0,
 ROTATE_90 = 1,
 ROTATE_180 = 2,
 ROTATE_270 = 3,
} rotation;

typedef enum
{
 bpp2 = 0,
 bpp3 = 1,
 bpp4 = 2,
 bpp8 = 3,
} bpp;

//prototype of structure
//structure prototype 1
typedef struct IT8951LdImgInfo
{
    bool bigEndian; //little or Big Endian
    bpp bpp; //bpp
    rotation rot; //Rotate mode
    uint8_t *sourceBuffer; //Start address of source Frame buffer
    uint8_t *targetBuffer;//Base address of target image buffer

} Image;

//structure prototype 2
typedef struct IT8951AreaImgInfo
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} Area;

typedef struct IT8951DevInfo
{
    uint16_t width;
    uint16_t height;
    uint8_t *bufferAddr;
    char FWVersion[16];  //16 Bytes String
    char LUTVersion[16];   //16 Bytes String
} I80DevInfo;

//Built in I80 Command Code
#define IT8951_TCON_SYS_RUN      0x0001
#define IT8951_TCON_STANDBY      0x0002
#define IT8951_TCON_SLEEP        0x0003
#define IT8951_TCON_REG_RD       0x0010
#define IT8951_TCON_REG_WR       0x0011
#define IT8951_TCON_MEM_BST_RD_T 0x0012
#define IT8951_TCON_MEM_BST_RD_S 0x0013
#define IT8951_TCON_MEM_BST_WR   0x0014
#define IT8951_TCON_MEM_BST_END  0x0015
#define IT8951_TCON_LD_IMG       0x0020
#define IT8951_TCON_LD_IMG_AREA  0x0021
#define IT8951_TCON_LD_IMG_END   0x0022

//I80 User defined command code
#define USDEF_I80_CMD_DPY_AREA     0x0034
#define USDEF_I80_CMD_GET_DEV_INFO 0x0302
#define USDEF_I80_CMD_DPY_BUF_AREA 0x0037
#define USDEF_I80_CMD_VCOM       0x0039

//Panel
#define IT8951_PANEL_WIDTH   1024 //it Get Device information
#define IT8951_PANEL_HEIGHT   758

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

//-----------------------------------------------------------------------
// IT8951 TCon Registers defines
//-----------------------------------------------------------------------
//Register Base Address
#define DISPLAY_REG_BASE 0x1000               //Register RW access for I80 only
//Base Address of Basic LUT Registers
#define LUT0EWHR  (DISPLAY_REG_BASE + 0x00)   //LUT0 Engine Width Height Reg
#define LUT0XYR   (DISPLAY_REG_BASE + 0x40)   //LUT0 XY Reg
#define LUT0BADDR (DISPLAY_REG_BASE + 0x80)   //LUT0 Base Address Reg
#define LUT0MFN   (DISPLAY_REG_BASE + 0xC0)   //LUT0 Mode and Frame number Reg
#define LUT01AF   (DISPLAY_REG_BASE + 0x114)  //LUT0 and LUT1 Active Flag Reg
//Update Parameter Setting Register
#define UP0SR (DISPLAY_REG_BASE + 0x134)      //Update Parameter0 Setting Reg

#define UP1SR     (DISPLAY_REG_BASE + 0x138)  //Update Parameter1 Setting Reg
#define LUT0ABFRV (DISPLAY_REG_BASE + 0x13C)  //LUT0 Alpha blend and Fill rectangle Value
#define UPBBADDR  (DISPLAY_REG_BASE + 0x17C)  //Update Buffer Base Address
#define LUT0IMXY  (DISPLAY_REG_BASE + 0x180)  //LUT0 Image buffer X/Y offset Reg
#define LUTAFSR   (DISPLAY_REG_BASE + 0x224)  //LUT Status Reg (status of All LUT Engines)

#define BGVR      (DISPLAY_REG_BASE + 0x250)  //Bitmap (1bpp) image color table
//-------System Registers----------------
#define SYS_REG_BASE 0x0000

//Address of System Registers
#define I80CPCR (SYS_REG_BASE + 0x04)
//-------Memory Converter Registers----------------
#define MCSR_BASE_ADDR 0x0200
#define MCSR (MCSR_BASE_ADDR  + 0x0000)
#define LISAR (MCSR_BASE_ADDR + 0x0008)

bool init(void);
void cancel(void);
void example1(void);
void example2(void);
void IT8951Display1bppExample2(void);
void IT8951DisplayExample3(void);
void example3(void);
void IT8951_BMP_Example(uint32_t x, uint32_t y,char *path);

uint16_t IT8951ReadReg(uint16_t usRegAddr);
void IT8951SetImgBufBaseAddr(uint8_t *ulImgBufAddr);
void LCDWaitForReady(void);
void GetIT8951SystemInfo(void* pBuf);
void gpio_i80_16b_cmd_out(uint16_t usCmd);
void GPIO_Configuration_Out(void);
void GPIO_Configuration_In(void);

void IT8951DisplayClear(void);

//uint16_t IT8951ReadReg(uint16_t usRegAddr);
void IT8951WriteReg(uint16_t usRegAddr,uint16_t usValue);

uint16_t IT8951GetVCOM(void);
void IT8951SetVCOM(uint16_t vcom);

#endif
