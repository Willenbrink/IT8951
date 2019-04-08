#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "IT.h"
#include "cmd.h"

static uint16_t readReg(uint16_t regAddr)
{
  writeCmd(IT8951_TCON_REG_RD);
  writeData(regAddr);
  return readData();
}

static void writeReg(uint16_t regAddr, uint16_t value)
{
  writeCmd(IT8951_TCON_REG_WR);
  writeData(regAddr);
  writeData(value);
}

bool initCmd()
{
  bool failure = initIT();

  //Set to Enable I80 Packed mode
  writeReg(I80CPCR, 0x0001);

  setVCOM(VCOM);

  return failure;
}

void freeCmd()
{
  freeIT();
}

//TODO unused
void systemRun()
{
  writeCmd(IT8951_TCON_SYS_RUN);
}

//TODO unused
void standBy()
{
  writeCmd(IT8951_TCON_STANDBY);
}

//TODO unused
void initSleep()
{
  writeCmd(IT8951_TCON_SLEEP);
}


uint16_t getVCOM()
{
  writeCmd(USDEF_I80_CMD_VCOM);
  writeData(0);
  return readData();
}

void setVCOM(uint16_t vcom)
{
  writeCmd(USDEF_I80_CMD_VCOM);
  writeData(1);
  writeData(vcom);
}

static void burstReadStart(uint32_t memAddr , uint32_t readAmount)
{
  uint16_t args[4];
  //Setting Arguments for Memory Burst Read
  args[0] = (uint16_t)(memAddr & 0xFFFF); //addr[15:0]
  args[1] = (uint16_t)( (memAddr >> 16) & 0xFFFF ); //addr[25:16]
  args[2] = (uint16_t)(readAmount & 0xFFFF); //Cnt[15:0]
  args[3] = (uint16_t)( (readAmount >> 16) & 0xFFFF ); //Cnt[25:16]
  //Send Cmd and Arg
  writeCmdArg(IT8951_TCON_MEM_BST_RD_T, args, 4);
  //Start burst read
  writeCmd(IT8951_TCON_MEM_BST_RD_S);
}

static void burstWriteStart(uint32_t memAddr , uint32_t writeAmount)
{
  uint16_t args[4];
  //Setting Arguments for Memory Burst Write
  args[0] = (uint16_t)(memAddr & 0xFFFF); //addr[15:0]
  args[1] = (uint16_t)( (memAddr >> 16) & 0xFFFF ); //addr[25:16]
  args[2] = (uint16_t)(writeAmount & 0xFFFF); //Cnt[15:0]
  args[3] = (uint16_t)( (writeAmount >> 16) & 0xFFFF ); //Cnt[25:16]
  //Send Cmd and Arg
  writeCmdArg(IT8951_TCON_MEM_BST_WR , args , 4);
}

static void burstEnd(void)
{
  writeCmd(IT8951_TCON_MEM_BST_END);
}

//-----------------------------------------------------------
//Example of Memory Burst Write
//-----------------------------------------------------------
// ****************************************************************************************
// Function name: IT8951MemBurstWriteProc( )
//
// Description:
//   IT8951 Burst Write procedure
//
// Arguments:
//      uint32_t ulMemAddr: IT8951 Memory Target Address
//      uint32_t ulWriteSize: Write Size (Unit: Word)
//      uint8_t* pDestBuf - Buffer of Sent data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************
void burstWriteProc(uint32_t memAddr , uint32_t writeSize, uint16_t* sourceBuffer)
{
  //Send Burst Write Start Cmd and Args
  burstWriteStart(memAddr, writeSize);

  //Burst Write Data
  for(uint32_t i = 0; i < writeSize; i++)
  {
    //TODO how is this burst? We are always transmitting the preamble
    writeData(sourceBuffer[i]);
  }

  //Send Burst End Cmd
  burstEnd();
}

// ****************************************************************************************
// Function name: IT8951MemBurstReadProc( )
//
// Description:
//   IT8951 Burst Read procedure
//
// Arguments:
//      uint32_t ulMemAddr: IT8951 Read Memory Address
//      uint32_t ulReadSize: Read Size (Unit: Word)
//      uint8_t* pDestBuf - Buffer for storing Read data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************
void burstReadProc(uint32_t memAddr , uint32_t readSize, uint16_t* destBuffer)
{
  //Send Burst Read Start Cmd and Args
  burstReadStart(memAddr, readSize);

  //Burst Read Request for SPI interface only
  readDataN(destBuffer, readSize);

  //Send Burst End Cmd
  burstEnd(); //the same with IT8951MemBurstEnd()
}

//-----------------------------------------------------------
//Host Cmd 10---LD_IMG
//-----------------------------------------------------------
void loadImgStart(Image* image)
{
  //Setting Argument for Load image start
  uint16_t arg = (image->bigEndian << 8 )
    |(image->bpp << 4)
    |(image->rot);
  //Send Cmd
  writeCmd(IT8951_TCON_LD_IMG);
  //Send Arg
  writeData(arg);
}
//-----------------------------------------------------------
//Host Cmd 11---LD_IMG_AREA
//-----------------------------------------------------------
void loadImgAreaStart(Image* image, Area* area)
{
  uint16_t args[5];
  //Setting Argument for Load image start
  args[0] = (image->bigEndian << 8 )
    |(image->bpp << 4)
    |(image->rot);
  args[1] = area->x;
  args[2] = area->y;
  args[3] = area->width;
  args[4] = area->height;
  //Send Cmd and Args
  writeCmdArg(IT8951_TCON_LD_IMG_AREA, args, 5);
}
//-----------------------------------------------------------
//Host Cmd 12---LD_IMG_END
//-----------------------------------------------------------
void loadImgEnd(void)
{
  writeCmd(IT8951_TCON_LD_IMG_END);
}

void printDeviceInfo(I80DevInfo info)
{
  printf("****** IT8951 ******\n");
  //Show Device information of IT8951
  printf("Panel(W,H) = (%d,%d)\n", info.width, info.height);
  printf("Image Buffer Address = %p\n", info.bufferAddr);
  //Show Firmware and LUT Version
  printf("FW Version = %s\n", info.FWVersion);
  printf("LUT Version = %s\n", info.LUTVersion);
}

I80DevInfo getDeviceInfo()
{
  struct {
    uint16_t width;
    uint16_t height;
    uint16_t imgBufAddrL;
    uint16_t imgBufAddrH;
    char FWVersion[16];
    char LUTVersion[16];
  } rawInfo;

  I80DevInfo info;

  //Send I80 CMD
  writeCmd(USDEF_I80_CMD_GET_DEV_INFO);

  //Burst Read Request for SPI interface only
  //size/2 because we always read 2 bytes
  //Polling HRDY for each words(2-bytes) if possible
  readDataN((uint16_t *) &rawInfo, sizeof(rawInfo)/2);

  //IT8951 only supports reading two byte at once, these need to be switched.
  for(uint16_t i = 0; i < 16; i += 2)
  {
    char tmp = rawInfo.FWVersion[i];
    rawInfo.FWVersion[i] = rawInfo.FWVersion[i+1];
    rawInfo.FWVersion[i+1] = tmp;
    tmp = rawInfo.LUTVersion[i];
    rawInfo.LUTVersion[i] = rawInfo.LUTVersion[i+1];
    rawInfo.LUTVersion[i+1] = tmp;
  }

  //Tranfer rawInfo into final info struct
  info.width = rawInfo.width;
  info.height = rawInfo.height;
  info.bufferAddr = (uint8_t *) ((rawInfo.imgBufAddrH << 16) | rawInfo.imgBufAddrL);
  strncpy(info.FWVersion, rawInfo.FWVersion, 16);
  strncpy(info.LUTVersion, rawInfo.LUTVersion, 16);

  printDeviceInfo(info);
  return info;
}

//-----------------------------------------------------------
//Initial function 2---Set Image buffer base address
//-----------------------------------------------------------
void setImgBufBaseAddr(uint8_t *imgBufAddr)
{
  uint16_t wordH = (uint16_t)(((int) imgBufAddr >> 16) & 0xFFFF);
  uint16_t wordL = (uint16_t)( (int) imgBufAddr        & 0xFFFF);
  //Write LISAR Reg
  writeReg(LISAR + 2, wordH);
  writeReg(LISAR, wordL);
}

//-----------------------------------------------------------
// 3.6. Display Functions
//-----------------------------------------------------------

//-----------------------------------------------------------
//Display function 1---Wait for LUT Engine Finish
//                     Polling Display Engine Ready by LUTNo
//-----------------------------------------------------------
void waitForDisplayReady()
{
  //Check IT8951 Register LUTAFSR => NonZero -> Busy, 0 -> Free
  while(readReg(LUTAFSR));
}

//-----------------------------------------------------------
//Display function 2---Load Image Area process
//-----------------------------------------------------------
void loadImage(Image* image, Area* area)
{
  //Set Image buffer(IT8951) Base address
  //Specify where
  setImgBufBaseAddr(image->targetBuffer);
  //Send Load Image start Cmd
  //Specify what
  loadImgAreaStart(image, area);

  //Host Write Data
  for(uint32_t j = 0; j < area->height; j++)
  {
    for(uint32_t i = 0; i < area->width; i += 2) //pixel are 8bpp but transmission is 2 bytes
    {
      uint32_t yOffset = (area->y + j) * image->width;
      uint32_t xOffset = area->x + i;
      uint32_t pos = yOffset + xOffset;
      //Write a Word(2-Bytes) for each time
      uint16_t value = image->sourceBuffer[pos+1];
      value = value << 8;
      value |= image->sourceBuffer[pos];
      writeData(value);
    }
  }
  //Send Load Img End Command
  loadImgEnd();
}

//-----------------------------------------------------------
//Display functions 3---Application for Display panel Area
//-----------------------------------------------------------
void displayArea(Area area, uint16_t displayMode)
{
  //NOTE: this is perhaps used to check whether transmission is finished (and the buffer can be overwritten)
  //It should then be called before loadImage instead of before displaying
  //TODO: what does this comment mean exactly?
  //Check TCon is free ? Wait TCon Ready (optional)
  waitForDisplayReady();
  //Send I80 Display Command (User defined command of IT8951)
  writeCmd(USDEF_I80_CMD_DPY_AREA); //0x0034
  //Write arguments
  writeData(area.x);
  writeData(area.y);
  writeData(area.width);
  writeData(area.height);
  writeData(displayMode);
}

//Display Area with bitmap on EPD
//-----------------------------------------------------------
// Display Function 4---for Display Area for 1-bpp mode format
//   the bitmap(1bpp) mode will be enable when Display
//   and restore to Default setting (disable) after displaying finished
//-----------------------------------------------------------
void displayArea1bpp(Area area, uint16_t usDpyMode, uint8_t ucBGGrayVal, uint8_t ucFGGrayVal)
{
  //Set Display mode to 1 bpp mode - Set 0x18001138 Bit[18](0x1800113A Bit[2])to 1
  writeReg(UP1SR+2, readReg(UP1SR+2) | (1<<2));

  //Set BitMap color table 0 and 1 , => Set Register[0x18001250]:
  //Bit[7:0]: ForeGround Color(G0~G15)  for 1
  //Bit[15:8]:Background Color(G0~G15)  for 0
  writeReg(BGVR, (ucBGGrayVal<<8) | ucFGGrayVal);

  //Display
  displayArea(area, usDpyMode);
  waitForDisplayReady();

  //Restore to normal mode
  writeReg(UP1SR+2, readReg(UP1SR+2) & ~(1<<2));
}

//-------------------------------------------------------------------------------------------------------------
//  Command - 0x0037 for Display Base addr by User
//  uint32_t ulDpyBufAddr - Host programmer need to indicate the Image buffer address of IT8951
//                                         In current case, there is only one image buffer in IT8951 so far.
//                                         So Please set the Image buffer address you got  in initial stage.
//                                         (gulImgBufAddr by Get device information 0x0302 command)
//
//-------------------------------------------------------------------------------------------------------------
void displayAreaBuf(Area area, uint16_t usDpyMode, uint32_t ulDpyBufAddr)
{
  //Send I80 Display Command (User defined command of IT8951)
  writeCmd(USDEF_I80_CMD_DPY_BUF_AREA); //0x0037

  //Write arguments
  writeData(area.x);
  writeData(area.y);
  writeData(area.width);
  writeData(area.height);
  writeData(usDpyMode);
  writeData((uint16_t)ulDpyBufAddr);       //Display Buffer Base address[15:0]
  writeData((uint16_t)(ulDpyBufAddr>>16)); //Display Buffer Base address[26:16]
}
