#include <stdlib.h>
#include <string.h>
#include "cmd.h"
#include "AsciiLib.h"

typedef uint8_t color;

color *buffer;
int bSize;
I80DevInfo devInfo;
int posX, posY;
color drawingColor;


Area getArea(int x1, int y1, int x2, int y2)
{
  int xl = x1 < x2 ? x1 : x2;
  int yl = y1 < y2 ? y1 : y2;
  int xh = x1 > x2 ? x1 : x2;
  int yh = y1 > y2 ? y1 : y2;

  //TODO leaky abstraction: always draw 2 pixels, therefore display area must be "rounded down/up"
  xl -= xl % 2;
  xh += xh % 2;
  yl -= yl % 2;
  yh += yh % 2;

  Area area = {
               .x = xl,
               .y = yl,
               .width = xh - xl,
               .height = yh - yl,
  };
  return area;
}

void loadImg(void *img, int x1, int y1, int x2, int y2)
{
  Image image = {
                 .sourceBuffer = img,
                 .targetBuffer = devInfo.bufferAddr,
                 .width = devInfo.width,
                 .height = devInfo.height,
                 .bigEndian = false,
                 .bpp = bpp8,//we use 8bpp because IT8951 dose not support 1bpp mode for load image?Aso we use Load 8bpp mode ,but the transfer size needs to be reduced to Size/8
                 .rot = ROTATE_0,
  };
  Area area = getArea(x1, y1, x2, y2);

  loadImage(&image, &area);
}

void display(int x1, int y1, int x2, int y2, int mode)
{
  Area area = getArea(x1, y1, x2, y2);

  displayArea(area, mode);
}

void displayBuffer(int x1, int y1, int x2, int y2)
{
  Area area = getArea(x1, y1, x2, y2);

  loadImg(buffer, x1, y1, x2, y2);
  displayArea(area, 2);
}

color rgb(int r, int g, int b)
{
  //TODO not weighed appropriately?
  return
    ( (r && 0xFF)
    + (g && 0xFF)
    + (b && 0xFF)
      ) / 3;
}

void clearColor(color c)
{
  memset(buffer, c, bSize);
}

int width()
{
  return devInfo.width;
}

int height()
{
  return devInfo.height;
}

void setColor(color c)
{
  drawingColor = c;
}

void plot(int x, int y)
{
  buffer[y * devInfo.width + x] = drawingColor;
}

color pointColor(int x, int y)
{
  return buffer[y * devInfo.width + x];
}

void moveto(int x, int y)
{
  posX = x;
  posY = y;
}

void rmoveto(int x, int y)
{
  posX += x;
  posY += y;
}

int currentX()
{
  return posX;
}

int currentY()
{
  return posY;
}

void drawLine(int x, int y, int x2, int y2)
{
  int deltax = 0, deltay = 0, xinc1 = 0, xinc2 = 0,
    yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0;

    deltax = abs(x2 - x);        /* The difference between the x's */
    deltay = abs(y2 - y);        /* The difference between the y's */

    if (x2 >= x)                 /* The x-values are increasing */
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else                          /* The x-values are decreasing */
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y)                 /* The y-values are increasing */
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else                          /* The y-values are decreasing */
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)         /* There is at least one x-value for every y-value */
    {
        xinc1 = 0;              /* Don't change the x when numerator >= denominator */
        yinc2 = 0;              /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        num_add = deltay;
        num_pixels = deltax;    /* There are more x-values than y-values */
    }
    else                          /* There is at least one y-value for every x-value */
    {
        xinc2 = 0;              /* Don't change the x for every iteration */
        yinc1 = 0;              /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        num_add = deltax;
        num_pixels = deltay;    /* There are more y-values than x-values */
    }

    for (int current = 0; current <= num_pixels; current++)
    {
        plot(x, y);          /* Draw the current pixel */
        num += num_add;                      /* Increase the numerator by the top of the fraction */
        if (num >= den)                      /* Check if numerator >= denominator */
        {
            num -= den;                    /* Calculate the new numerator value */
            x += xinc1;                    /* Change the x as appropriate */
            y += yinc1;                    /* Change the y as appropriate */
        }
        x += xinc2;                          /* Change the x as appropriate */
        y += yinc2;                          /* Change the y as appropriate */
    }
}

void drawCircle(int x, int y, int r)
{
  int   decision;     /* Decision Variable */
  int  current_x;    /* Current X Value */
  int  current_y;    /* Current Y Value */

  decision = 3 - (r << 1);
  current_x = 0;
  current_y = r;

  while (current_x <= current_y)
  {
    plot((x + current_x), (y - current_y));
    plot((x - current_x), (y - current_y));
    plot((x + current_y), (y - current_x));
    plot((x - current_y), (y - current_x));
    plot((x + current_x), (y + current_y));
    plot((x - current_x), (y + current_y));
    plot((x + current_y), (y + current_x));
    plot((x - current_y), (y + current_x));

    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
}

void fillCircle(int x, int y, int r)
{
  int  decision;     /* Decision Variable */
  int  current_x;   /* Current X Value */
  int  current_y;   /* Current Y Value */


  decision = 3 - (r << 1);

  current_x = 0;
  current_y = r;

  while (current_x <= current_y)
  {
    if(current_y > 0)
    {
      drawLine(x - current_y, y + current_x, x + current_y, y + current_x);
      drawLine(x - current_y, y - current_x, x + current_y, y - current_x);
    }

    if(current_x > 0)
    {
      drawLine(x - current_x, y - current_y,x+current_x,y - current_y);
      drawLine(x - current_x, y + current_y,x+current_x,y + current_y);
    }
    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
  drawCircle(x, y, r);
}

void putChar(uint16_t x, uint16_t y, char charac, color fg, color bg)
{
  char buffer[16], tmp_char;
  color old = drawingColor;
  drawingColor = bg;
  GetASCIICode(buffer, charac);
  for(uint16_t i = 0; i < 16; i++)
  {
    tmp_char = buffer[i];
    for(uint16_t j = 0; j < 8; j++)
    {
      if(((tmp_char >> (7-j)) & 0x01) == 0x01)
      {
        drawingColor = fg;
        plot(x + j, y + i);
        drawingColor = bg;
      }
      else
      {
        plot(x + j, y + i);
      }
    }
  }
  drawingColor = old;
}


void putText(uint16_t x, uint16_t y, char *str, color fg, color bg)
{
  while (*str != 0)
  {
    //Check bounds and insert linebreaks
    if(x < devInfo.width - 8)
    {
      x += 8;
    }
    else if (y < devInfo.height - 16)
    {
      x = 0;
      y += 16;
    }
    else
    {
      x = 0;
      y = 0;
    }
    putChar(x, y, *(str++), fg, bg);
  }
}


bool initGraphics()
{
  bool ret = initCmd();
  devInfo = getDeviceInfo();
  bSize = devInfo.width * devInfo.height;
  buffer = (color *) calloc(1, bSize);

  posX = 0;
  posY = 0;
  loadImg(buffer, 0, 0, devInfo.width - 1, devInfo.height - 1);

  return ret;
}

void freeGraphics()
{
  freeCmd();
}
