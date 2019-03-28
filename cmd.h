#include <stdbool.h>
#include <stdint.h>

/*
 * Third level of abstraction: Wrap commands to enable sending complex instructions to the controller
 * E.g. Writing/Reading a large part of memory in burst-mode or displaying a image, which was previously transmitted.
 * Furthermore it is now possible to write to registers
 */

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

#define VCOM 1500 //e.g. -1.53 = 1530 = 0x5FA

bool initCmd();
void freeCmd();
void systemRun();
void standBy();
void initSleep();
uint16_t readReg(uint16_t regAddr);
void writeReg(uint16_t regAddr, uint16_t value);
uint16_t getVCOM();
void setVCOM(uint16_t vcom);
void burstReadStart(uint32_t memAddr , uint32_t readAmount);
void burstWrite(uint32_t memAddr , uint32_t writeAmount);
void burstEnd(void);
void burstWriteProc(uint32_t memAddr , uint32_t writeSize, uint16_t* sourceBuffer);
void burstReadProc(uint32_t memAddr , uint32_t readSize, uint16_t* destBuffer);
void loadImgStart(Image* image);
void loadImgAreaStart(Image* image, Area* area);
void loadImgEnd(void);
void printDeviceInfo(I80DevInfo info);
I80DevInfo getDeviceInfo();
void setImgBufBaseAddr(uint8_t *imgBufAddr);
void waitForDisplayReady();
void hostAreaPackedPixelWrite(Image* image, Area* area);
void displayArea(Area area, uint16_t usDpyMode);
void displayArea1bpp(Area area, uint16_t usDpyMode, uint8_t ucBGGrayVal, uint8_t ucFGGrayVal);
void displayAreaBuf(Area area, uint16_t usDpyMode, uint32_t ulDpyBufAddr);
