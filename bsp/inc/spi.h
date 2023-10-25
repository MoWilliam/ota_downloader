#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


extern SPI_HandleTypeDef SPI1_Handler;  //SPI���

#define SPI_AFE4300_CS       PBout(12)       //W25QXX的片选信号


//#define SPI_AFE4300_RESET    PBin(10)   //综采老板子为PBout(10) 新版为PBin

#define SPI_AFE4300_RESET      PBout(10)   //综采板子






void SPI2_Init(void);
void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);

void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);
#endif
