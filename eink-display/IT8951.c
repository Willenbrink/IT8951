#include "IT8951.h"
#include "cmd.h"

//Global varivale
I80DevInfo devInfo;
uint8_t* gpFrameBuf; //Host Source Frame buffer
uint32_t frameBufSize;



//-----------------------------------------------------------
//Test function 1---Software Initial
//-----------------------------------------------------------
bool init()
{
  if (initCmd())
    {
      printf("bcm2835_init error \n");
      return 1;
    }
  //Get Device Info
  devInfo = getDeviceInfo();

  frameBufSize = devInfo.width * devInfo.height;
  gpFrameBuf = (uint8_t *) malloc(frameBufSize);
  if (!gpFrameBuf)
    {
      perror("malloc error!\n");
      return 1;
    }

  return 0;
}

void cancel()
{
  freeCmd();
  free(gpFrameBuf);
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

  //--------------------------------------------------------------------------------------------
  //      initial display - Display white only
  //--------------------------------------------------------------------------------------------
  //Load Image and Display
  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  loadImage(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 0 for initial White to clear Panel
  displayArea(area, 0);

  //--------------------------------------------------------------------------------------------
  //      Regular display - Display Any Gray colors with Mode 2 or others
  //--------------------------------------------------------------------------------------------
  //Preparing buffer to All black (8 bpp image)
  //or you can create your image pattern here..
  memset(gpFrameBuf, 0xF0, frameBufSize);

  //Setting Load image information
  image = getDefaultImage();
  //Set Load Area
  area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  loadImage(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  displayArea(area, 2);
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
  memset(gpFrameBuf+width * high * 1,  0x10, width * high * 1);
  memset(gpFrameBuf+width * high * 2,  0x20, width * high * 1);
  memset(gpFrameBuf+width * high * 3,  0x30, width * high * 1);
  memset(gpFrameBuf+width * high * 4,  0x40, width * high * 1);
  memset(gpFrameBuf+width * high * 5,  0x50, width * high * 1);
  memset(gpFrameBuf+width * high * 6,  0x60, width * high * 1);
  memset(gpFrameBuf+width * high * 7,  0x70, width * high * 1);
  memset(gpFrameBuf+width * high * 8,  0x80, width * high * 1);
  memset(gpFrameBuf+width * high * 9,  0x90, width * high * 1);
  memset(gpFrameBuf+width * high * 10, 0xa0, width * high * 1);
  memset(gpFrameBuf+width * high * 11, 0xb0, width * high * 1);
  memset(gpFrameBuf+width * high * 12, 0xc0, width * high * 1);
  memset(gpFrameBuf+width * high * 13, 0xd0, width * high * 1);
  memset(gpFrameBuf+width * high * 14, 0xe0, width * high * 1);
  memset(gpFrameBuf+width * high * 15, 0xf0, frameBufSize - width * high * 15);
  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  loadImage(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  displayArea(area, 2);
}

extern uint16_t bmp01[];

void example3()
{
  Point point[4];

  //memset(gpFrameBuf, 0xff, gstI80DevInfo.usPanelW * gstI80DevInfo.usPanelH);
  uint16_t width = devInfo.width;
  uint16_t high = devInfo.height;

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

  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  loadImage(&image, &area);//Display function 2

  displayArea(area, 2);
}

void IT8951_BMP_Example(uint32_t x, uint32_t y,char *path)
{
  EPD_Clear(0xff);

  //ÏÔÊ¾Í¼Ïñ
  Show_bmp(x,y,path);

  //Setting Load image information
  Image image = getDefaultImage();
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  loadImage(&image, &area);//Display function 2
  //Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform
  displayArea(area, 2);
}

//-----------------------------------------------------------
// Load 1bpp image flow (must display with IT8951DisplayArea1bpp()
//-----------------------------------------------------------

void Load1bppImage(uint8_t* p1bppImgBuf, Area area)
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
  loadImage(&image, &areaNew);//Display function 2
}

//-----------------------------------------------------------
//Test function 3---Example of Display 1bpp Flow
//-----------------------------------------------------------
void Display1bppExample()
{
  //Prepare image
  //Write pixel 0x00(Black) to Frame Buffer
  //or you can create your image pattern here..
  memset(gpFrameBuf, 0x00, frameBufSize/8);//Host Frame Buffer(Source)

  //Load Image and Display
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  Load1bppImage(gpFrameBuf, area);//Display function 4, Arg

  //Display Area - (x,y,w,h) with mode 2 for Gray Scale
  //e.g. if we want to set b0(Background color) for Black-0x00 , Set b1(Foreground) for White-0xFF
  displayArea1bpp(area, 0, 0x00, 0xFF);
}

void Display1bppExample2()
{
  //Prepare image
  //Write pixel 0x00(Black) to Frame Buffer
  //or you can create your image pattern here..
  memset(gpFrameBuf, 0xff, frameBufSize/8);//Host Frame Buffer(Source)

  //Load Image and Display
  //Set Load Area
  Area area = getDefaultArea();

  //Load Image from Host to IT8951 Image Buffer
  Load1bppImage(gpFrameBuf, area);//Display function 4, Arg

  //Display Area - (x,y,w,h) with mode 2 for Gray Scale
  //e.g. if we want to set b0(Background color) for Black-0x00 , Set b1(Foreground) for White-0xFF
  displayArea1bpp(area, 0, 0x00, 0xFF);
}
