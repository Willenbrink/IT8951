#include "bcm.h"

void writeN(uint16_t preamble, uint16_t* data, uint32_t count)
{
  openBus();
  transfer(preamble);
  waitForBus();
  for(uint32_t i = 0; i < count; i++)
  {
    transfer(data[i]);
  }
  closeBus();
}

//-----------------------------------------------------------
//Host controller function 3---Write Data to host data Bus
//-----------------------------------------------------------

void writeDataN(uint16_t *data, uint32_t count)
{
  uint16_t preamble = 0x0000;
  writeN(preamble, data, count);
}

void writeData(uint16_t data)
{
  writeDataN(&data, 1);
}
//-----------------------------------------------------------
//  Read Burst N words Data
//-----------------------------------------------------------
void readDataN(uint16_t* pwBuf, uint32_t count)
{
  uint16_t preamble = 0x1000;
  openBus();
  transfer(preamble);
  waitForBus();
  transfer(0x00); //dummy-value
  waitForBus();
  for(uint32_t i = 0; i < count; i++)
    {
      pwBuf[i] = transfer(0x00);
    }
  closeBus();
}

//-----------------------------------------------------------
//Host controller function 4---Read Data from host data Bus
//-----------------------------------------------------------
uint16_t readData()
{
  uint16_t wRData = 0x00;
  readDataN(&wRData, 1);
  return wRData;
}
//-----------------------------------------------------------
//Host controller function 2---Write command code to host data Bus
//-----------------------------------------------------------
void writeCmd(uint16_t cmdCode)
{
  //Set Preamble for Write Command
  uint16_t preamble = 0x6000;
  writeN(preamble, &cmdCode, 1);
}

//-----------------------------------------------------------
//Host controller function 5---Write command to host data Bus with aruments
//-----------------------------------------------------------
void writeCmdArg(uint16_t usCmdCode,uint16_t* pArg, uint16_t usNumArg)
{
  //Send Cmd code
  writeCmd(usCmdCode);
  //Send Data
  for(uint16_t i = 0; i < usNumArg; i++)
  {
    writeData(pArg[i]);
  }
}
