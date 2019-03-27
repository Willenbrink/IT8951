#include "bcm.h"
#include "IT8951.h"
#include "IT.h"

//Global varivale
I80DevInfo devInfo;
uint8_t* gpFrameBuf; //Host Source Frame buffer
uint32_t frameBufSize;


//-----------------------------------------------------------
//Host Cmd 1---SYS_RUN
//-----------------------------------------------------------
void IT8951SystemRun()
{
    writeCmd(IT8951_TCON_SYS_RUN);
}

//-----------------------------------------------------------
//Host Cmd 2---STANDBY
//-----------------------------------------------------------
void IT8951StandBy()
{
    writeCmd(IT8951_TCON_STANDBY);
}

//-----------------------------------------------------------
//Host Cmd 3---SLEEP
//-----------------------------------------------------------
void IT8951Sleep()
{
    writeCmd(IT8951_TCON_SLEEP);
}

//-----------------------------------------------------------
//Host Cmd 4---REG_RD
//-----------------------------------------------------------
uint16_t IT8951ReadReg(uint16_t usRegAddr)
{
  uint16_t usData;

  //Send Cmd and Register Address
  writeCmd(IT8951_TCON_REG_RD);
  writeData(usRegAddr);
  //Read data from Host Data bus
  usData = readData();
  return usData;
}
//-----------------------------------------------------------
//Host Cmd 5---REG_WR
//-----------------------------------------------------------
void IT8951WriteReg(uint16_t usRegAddr,uint16_t usValue)
{
  //Send Cmd , Register Address and Write Value
  writeCmd(IT8951_TCON_REG_WR);
  writeData(usRegAddr);
  writeData(usValue);
}

//-----------------------------------------------------------
//Host Cmd 6---MEM_BST_RD_T
//-----------------------------------------------------------
void IT8951MemBurstReadTrigger(uint32_t ulMemAddr , uint32_t ulReadSize)
{
    uint16_t usArg[4];
    //Setting Arguments for Memory Burst Read
    usArg[0] = (uint16_t)(ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (uint16_t)( (ulMemAddr >> 16) & 0x0000FFFF ); //addr[25:16]
    usArg[2] = (uint16_t)(ulReadSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (uint16_t)( (ulReadSize >> 16) & 0x0000FFFF ); //Cnt[25:16]
    //Send Cmd and Arg
    writeCmdArg(IT8951_TCON_MEM_BST_RD_T , usArg , 4);
}
//-----------------------------------------------------------
//Host Cmd 7---MEM_BST_RD_S
//-----------------------------------------------------------
void IT8951MemBurstReadStart()
{
    writeCmd(IT8951_TCON_MEM_BST_RD_S);
}
//-----------------------------------------------------------
//Host Cmd 8---MEM_BST_WR
//-----------------------------------------------------------
void IT8951MemBurstWrite(uint32_t ulMemAddr , uint32_t ulWriteSize)
{
    uint16_t usArg[4];
    //Setting Arguments for Memory Burst Write
    usArg[0] = (uint16_t)(ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (uint16_t)( (ulMemAddr >> 16) & 0x0000FFFF ); //addr[25:16]
    usArg[2] = (uint16_t)(ulWriteSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (uint16_t)( (ulWriteSize >> 16) & 0x0000FFFF ); //Cnt[25:16]
    //Send Cmd and Arg
    writeCmdArg(IT8951_TCON_MEM_BST_WR , usArg , 4);
}
//-----------------------------------------------------------
//Host Cmd 9---MEM_BST_END
//-----------------------------------------------------------
void IT8951MemBurstEnd(void)
{
    writeCmd(IT8951_TCON_MEM_BST_END);
}

uint16_t IT8951GetVCOM(void)
{
  uint16_t vcom;

  writeCmd(USDEF_I80_CMD_VCOM);
  writeData(0);
  //Read data from Host Data bus
  vcom = readData();
  return vcom;
}

void IT8951SetVCOM(uint16_t vcom)
{
  writeCmd(USDEF_I80_CMD_VCOM);
  writeData(1);
  //Read data from Host Data bus
  writeData(vcom);
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
void IT8951MemBurstWriteProc(uint32_t ulMemAddr , uint32_t ulWriteSize, uint16_t* pSrcBuf )
{
    //Send Burst Write Start Cmd and Args
    IT8951MemBurstWrite(ulMemAddr , ulWriteSize);

    //Burst Write Data
    for(uint32_t i=0;i<ulWriteSize;i++)
    {
        writeData(pSrcBuf[i]);
    }

    //Send Burst End Cmd
    IT8951MemBurstEnd();
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
void IT8951MemBurstReadProc(uint32_t ulMemAddr , uint32_t ulReadSize, uint16_t* pDestBuf )
{
    //Send Burst Read Start Cmd and Args
    IT8951MemBurstReadTrigger(ulMemAddr , ulReadSize);

    //Burst Read Fire
    IT8951MemBurstReadStart();

    //Burst Read Request for SPI interface only
    readDataN(pDestBuf, ulReadSize);

    //Send Burst End Cmd
    IT8951MemBurstEnd(); //the same with IT8951MemBurstEnd()
}

//-----------------------------------------------------------
//Host Cmd 10---LD_IMG
//-----------------------------------------------------------
void IT8951LoadImgStart(Image* image)
{
    //Setting Argument for Load image start
    uint16_t usArg = (image->bigEndian << 8 )
      |(image->bpp << 4)
      |(image->rot);
    //Send Cmd
    writeCmd(IT8951_TCON_LD_IMG);
    //Send Arg
    writeData(usArg);
}
//-----------------------------------------------------------
//Host Cmd 11---LD_IMG_AREA
//-----------------------------------------------------------
void IT8951LoadImgAreaStart(Image* image ,Area* area)
{
    uint16_t usArg[5];
    //Setting Argument for Load image start
    usArg[0] = (image->bigEndian << 8 )
      |(image->bpp << 4)
      |(image->rot);
    usArg[1] = area->x;
    usArg[2] = area->y;
    usArg[3] = area->width;
    usArg[4] = area->height;
    //Send Cmd and Args
    writeCmdArg(IT8951_TCON_LD_IMG_AREA , usArg , 5);
}
//-----------------------------------------------------------
//Host Cmd 12---LD_IMG_END
//-----------------------------------------------------------
void IT8951LoadImgEnd(void)
{
    writeCmd(IT8951_TCON_LD_IMG_END);
}

void printScreenInfo(I80DevInfo info)
{
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
    char FWVersion[16];  //16 Bytes String
    char LUTVersion[16];   //16 Bytes String
  } rawInfo;

  I80DevInfo info;

  //Send I80 CMD
  writeCmd(USDEF_I80_CMD_GET_DEV_INFO);

  //Burst Read Request for SPI interface only
  //size/2 because we always read 2 bytes
  //Polling HRDY for each words(2-bytes) if possible
  readDataN((uint16_t *) &rawInfo, sizeof(rawInfo)/2);

  //Tranfer rawInfo into final info struct
  info.width = rawInfo.width;
  info.height = rawInfo.height;
  info.bufferAddr = (uint8_t *) ((rawInfo.imgBufAddrH << 16) | rawInfo.imgBufAddrL);
  strncpy(info.FWVersion, rawInfo.FWVersion, 16);
  strncpy(info.LUTVersion, rawInfo.LUTVersion, 16);

  printScreenInfo(info);
  return info;
}

//-----------------------------------------------------------
//Initial function 2---Set Image buffer base address
//-----------------------------------------------------------
void IT8951SetImgBufBaseAddr(uint8_t *ulImgBufAddr)
{
  uint16_t usWordH = (uint16_t)(((int) ulImgBufAddr >> 16) & 0x0000FFFF);
  uint16_t usWordL = (uint16_t)( (int) ulImgBufAddr & 0x0000FFFF);
  //Write LISAR Reg
  IT8951WriteReg(LISAR + 2 ,usWordH);
  IT8951WriteReg(LISAR ,usWordL);
}

//-----------------------------------------------------------
// 3.6. Display Functions
//-----------------------------------------------------------

//-----------------------------------------------------------
//Display function 1---Wait for LUT Engine Finish
//                     Polling Display Engine Ready by LUTNo
//-----------------------------------------------------------
void IT8951WaitForDisplayReady()
{
  //Check IT8951 Register LUTAFSR => NonZero Busy, 0 - Free
  while(IT8951ReadReg(LUTAFSR));
}

//-----------------------------------------------------------
//Display function 2---Load Image Area process
//-----------------------------------------------------------
void IT8951HostAreaPackedPixelWrite(Image* image, Area* area)
{
  //Set Image buffer(IT8951) Base address
  IT8951SetImgBufBaseAddr(image->targetBuffer);
  //Send Load Image start Cmd
  IT8951LoadImgAreaStart(image, area);
  //Host Write Data
  for(uint32_t j = 0; j < area->height; j++)
  {
    for(uint32_t i = 0; i < area->width; i += 2) //pixel are 8bpp but transmission is 2 bytes
      {
        uint32_t pos = j * area->width + i;
        //Write a Word(2-Bytes) for each time
        uint16_t value = image->sourceBuffer[pos+1];
        value = value << 8;
        value |= image->sourceBuffer[pos];
        writeData(value);
      }
  }
  //Send Load Img End Command
  IT8951LoadImgEnd();
}

//-----------------------------------------------------------
//Display functions 3---Application for Display panel Area
//-----------------------------------------------------------
void IT8951DisplayArea(Area area, uint16_t usDpyMode)
{
  //Send I80 Display Command (User defined command of IT8951)
  writeCmd(USDEF_I80_CMD_DPY_AREA); //0x0034
  //Write arguments
  writeData(area.x);
  writeData(area.y);
  writeData(area.width);
  writeData(area.height);
  writeData(usDpyMode);
}

//Display Area with bitmap on EPD
//-----------------------------------------------------------
// Display Function 4---for Display Area for 1-bpp mode format
//   the bitmap(1bpp) mode will be enable when Display
//   and restore to Default setting (disable) after displaying finished
//-----------------------------------------------------------
void IT8951DisplayArea1bpp(Area area, uint16_t usDpyMode, uint8_t ucBGGrayVal, uint8_t ucFGGrayVal)
{
    //Set Display mode to 1 bpp mode - Set 0x18001138 Bit[18](0x1800113A Bit[2])to 1
    IT8951WriteReg(UP1SR+2, IT8951ReadReg(UP1SR+2) | (1<<2));

    //Set BitMap color table 0 and 1 , => Set Register[0x18001250]:
    //Bit[7:0]: ForeGround Color(G0~G15)  for 1
    //Bit[15:8]:Background Color(G0~G15)  for 0
    IT8951WriteReg(BGVR, (ucBGGrayVal<<8) | ucFGGrayVal);

    //Display
    IT8951DisplayArea(area, usDpyMode);
    IT8951WaitForDisplayReady();

    //Restore to normal mode
    IT8951WriteReg(UP1SR+2, IT8951ReadReg(UP1SR+2) & ~(1<<2));
}

//-------------------------------------------------------------------------------------------------------------
//  Command - 0x0037 for Display Base addr by User
//  uint32_t ulDpyBufAddr - Host programmer need to indicate the Image buffer address of IT8951
//                                         In current case, there is only one image buffer in IT8951 so far.
//                                         So Please set the Image buffer address you got  in initial stage.
//                                         (gulImgBufAddr by Get device information 0x0302 command)
//
//-------------------------------------------------------------------------------------------------------------
void IT8951DisplayAreaBuf(Area area, uint16_t usDpyMode, uint32_t ulDpyBufAddr)
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

//-----------------------------------------------------------
//Test function 1---Software Initial
//-----------------------------------------------------------
bool init()
{
  if (initBCM())
  {
    printf("bcm2835_init error \n");
    return 1;
  }
  printf("****** IT8951 ******\n");

  //Get Device Info
  devInfo = getDeviceInfo();

  frameBufSize = devInfo.width * devInfo.height;
  gpFrameBuf = (uint8_t *) malloc(frameBufSize);
  if (!gpFrameBuf)
  {
    perror("malloc error!\n");
    return 1;
  }

  //Set to Enable I80 Packed mode
  IT8951WriteReg(I80CPCR, 0x0001);

  if (VCOM != IT8951GetVCOM())
  {
    IT8951SetVCOM(VCOM);
    printf("VCOM = -%.02fV\n",(float)IT8951GetVCOM()/1000);
  }

  return 0;
}

void cancel()
{
  free(gpFrameBuf);

  bcm2835_spi_end();
  bcm2835_close();
}

Image getDefaultImage()
{
  Image image;
  image.bigEndian = false;
  image.bpp = bpp8;
  image.rot = ROTATE_0;
  image.sourceBuffer = gpFrameBuf;
  image.targetBuffer = devInfo.bufferAddr;
  return image;
}

Area getDefaultArea()
{
  Area area;
  area.x = 0;
  area.y = 0;
  area.width = devInfo.width;
  area.height = devInfo.height;
  return area;
}

//-----------------------------------------------------------
//Test function 2---Example of Display Flow
//-----------------------------------------------------------
void example1()
{
  //Prepare image
  //Write pixel 0xF0(White) to Frame Buffer
  memset(gpFrameBuf, 0xF0, frameBufSize);

  //Check TCon is free ? Wait TCon Ready (optional)
  IT8951WaitForDisplayReady();

  //--------------------------------------------------------------------------------------------
  //      initial display - Display white only
  //--------------------------------------------------------------------------------------------
  //Load Image and Display
  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 0 for initial White to clear Panel
  IT8951DisplayArea(area, 0);

  //--------------------------------------------------------------------------------------------
  //      Regular display - Display Any Gray colors with Mode 2 or others
  //--------------------------------------------------------------------------------------------
  //Preparing buffer to All black (8 bpp image)
  //or you can create your image pattern here..
  memset(gpFrameBuf, 0xF0, frameBufSize);

  IT8951WaitForDisplayReady();

  //Setting Load image information
  image = getDefaultImage();
  //Set Load Area
  area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  IT8951DisplayArea(area, 2);
}

void example2()
{
  uint16_t width = devInfo.width;
  uint16_t high = devInfo.height/16; // /16 because of 16 bars

  //--------------------------------------------------------------------------------------------
  //      Regular display - Display Any Gray colors with Mode 2 or others
  //--------------------------------------------------------------------------------------------
  //Preparing buffer to All black (8 bpp image)
  //or you can create your image pattern here..
  memset(gpFrameBuf                 ,  0x00, width * high * 1);
  memset(gpFrameBuf+width * high * 1,  0x11, width * high * 1);
  memset(gpFrameBuf+width * high * 2,  0x22, width * high * 1);
  memset(gpFrameBuf+width * high * 3,  0x33, width * high * 1);
  memset(gpFrameBuf+width * high * 4,  0x44, width * high * 1);
  memset(gpFrameBuf+width * high * 5,  0x55, width * high * 1);
  memset(gpFrameBuf+width * high * 6,  0x66, width * high * 1);
  memset(gpFrameBuf+width * high * 7,  0x77, width * high * 1);
  memset(gpFrameBuf+width * high * 8,  0x88, width * high * 1);
  memset(gpFrameBuf+width * high * 9,  0x99, width * high * 1);
  memset(gpFrameBuf+width * high * 10, 0xaa, width * high * 1);
  memset(gpFrameBuf+width * high * 11, 0xbb, width * high * 1);
  memset(gpFrameBuf+width * high * 12, 0xcc, width * high * 1);
  memset(gpFrameBuf+width * high * 13, 0xdd, width * high * 1);
  memset(gpFrameBuf+width * high * 14, 0xee, width * high * 1);
  memset(gpFrameBuf+width * high * 15, 0xff, frameBufSize - width * high * 15);
  IT8951WaitForDisplayReady();
  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  IT8951DisplayArea(area, 2);
}

extern uint16_t bmp01[];

void example3()
{
  Point point[4];

  //memset(gpFrameBuf, 0xff, gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH);
  uint16_t width = devInfo.width;
  uint16_t high = devInfo.height; //TODO no /16 here?

  EPD_Clear(0xff);

  EPD_DrawLine(width/4, high/4, width/2, high/2, 0x40);
  EPD_DrawLine(width/2, high/2, width*3/4, high/4, 0x40);
  EPD_DrawLine(width*3/4, high/4, width/2, 0, 0x40);
  EPD_DrawLine(width/2, 0, width/4, high/4, 0x40);

  EPD_DrawRect(width/4, 0, width/2, high/2, 0x00);

  EPD_DrawCircle(width/2, high/4, high/4, 0x00);

  point[0].X = width/2;
  point[0].Y = 0;

  point[1].X = width/4;
  point[1].Y = high/2;

  point[2].X = width/2;
  point[2].Y = high;

  point[3].X = width*3/4;
  point[3].Y = high/2;

  EPD_DrawPolygon(point, 4, 0x00);

  EPD_DrawEllipse(width/2, high/2, width/4, high/4, 0x00);

  EPD_FillRect(width/12, high*8/10, width/10, high/10, 0x00);

  EPD_FillCircle(width*10/12, high*8/10, width/10, 0x00);

  EPD_Text(0,  0, (uint8_t*)"hello world",0x00, 0xff);

  //EPD_DrawMatrix(0,0,550,412,bmp01);

  IT8951WaitForDisplayReady();

  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &area);//Display function 2

  IT8951DisplayArea(area, 2);
}

void IT8951_BMP_Example(uint32_t x, uint32_t y,char *path)
{
  EPD_Clear(0xff);

  //ÏÔÊ¾Í¼Ïñ
  Show_bmp(x,y,path);

  IT8951WaitForDisplayReady();

  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  IT8951DisplayArea(area, 2);
}

//-----------------------------------------------------------
// Load 1bpp image flow (must display with IT8951DisplayArea1bpp()
//-----------------------------------------------------------

void IT8951Load1bppImage(uint8_t* p1bppImgBuf, Area area)
{
  //Setting Load image information
  Image image = {
                 .sourceBuffer = p1bppImgBuf,
                 .targetBuffer = devInfo.bufferAddr,
                 .bigEndian = false,
                 .bpp = bpp8,//we use 8bpp because IT8951 dose not support 1bpp mode for load image?Aso we use Load 8bpp mode ,but the transfer size needs to be reduced to Size/8
                 .rot = ROTATE_0,
  };
    //Set Load Area
  Area areaNew = {
               .x = area.x/8,
               .y = area.y,
               .width = area.width/8,//1bpp, Chaning Transfer size setting to 1/8X of 8bpp mode
               .height = area.height,
  };

  printf("IT8951HostAreaPackedPixelWrite [wait]\n\r");
  //Load Image from Host to IT8951 Image Buffer
  IT8951HostAreaPackedPixelWrite(&image, &areaNew);//Display function 2
}

//-----------------------------------------------------------
//Test function 3---Example of Display 1bpp Flow
//-----------------------------------------------------------
void IT8951Display1bppExample()
{
    //Prepare image
    //Write pixel 0x00(Black) to Frame Buffer
    //or you can create your image pattern here..
     memset(gpFrameBuf, 0x00, frameBufSize/8);//Host Frame Buffer(Source)

     //Check TCon is free ? Wait TCon Ready (optional)
     IT8951WaitForDisplayReady();

     //Load Image and Display
     //Set Load Area
     Area area = getDefaultArea();

     //Load Image from Host to IT8951 Image Buffer
     IT8951Load1bppImage(gpFrameBuf, area);//Display function 4, Arg

     //Display Area - (x,y,w,h) with mode 2 for Gray Scale
     //e.g. if we want to set b0(Background color) for Black-0x00 , Set b1(Foreground) for White-0xFF
     IT8951DisplayArea1bpp(area, 0, 0x00, 0xFF);
}

void IT8951Display1bppExample2()
{
    //Prepare image
    //Write pixel 0x00(Black) to Frame Buffer
    //or you can create your image pattern here..
    memset(gpFrameBuf, 0xff, frameBufSize/8);//Host Frame Buffer(Source)

    //Check TCon is free ? Wait TCon Ready (optional)
    IT8951WaitForDisplayReady();

    //Load Image and Display
    //Set Load Area
    Area area = getDefaultArea();

    //Load Image from Host to IT8951 Image Buffer
    IT8951Load1bppImage(gpFrameBuf, area);//Display function 4, Arg

    //Display Area - (x,y,w,h) with mode 2 for Gray Scale
    //e.g. if we want to set b0(Background color) for Black-0x00 , Set b1(Foreground) for White-0xFF
    IT8951DisplayArea1bpp(area, 0, 0x00, 0xFF);
}
