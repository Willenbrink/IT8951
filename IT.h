void writeN(uint16_t preamble, uint16_t* data, uint32_t count);
void writeDataN(uint16_t *data, uint32_t count);
void writeData(uint16_t data);
void readDataN(uint16_t* pwBuf, uint32_t ulSizeWordCnt);
uint16_t readData();
void writeCmd(uint16_t commandCode);
void writeCmdArg(uint16_t commandCode,uint16_t* args, uint16_t count);
