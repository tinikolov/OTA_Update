#ifndef UPDATEFIRMWARE_H
#define UPDATEFIRMWARE_H

#include <stdbool.h>
#include <stdint.h>

#include "spi_flash.h"
#include "common.h"
#include "communication.h"
#include "crc32.h"

extern uint8_t sbox[];
extern uint8_t rsbox[];
#ifdef	__cplusplus
extern "C" {
#endif
 
  void SetupFirmwareUpdate(void);
  void ReadReceivedLength(uint32_t startAddr);
  void ReadReceivedCRC(uint32_t addr);
  void EraseSectors(void);
	void FirmwareUpdate(void);
  void FLASH_ReadFirmware(uint32_t startAddr);
  void ReadData(uint32_t *startAddr,uint8_t len);
  void EraseFirmwareSection(uint32_t start_addr,uint32_t end_addr);
  void Init_firmwareUpdate(void);
  void WriteEncryptionBoxes(void);
 
#ifdef	__cplusplus
}
#endif

#endif


