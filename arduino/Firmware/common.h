#ifndef COMMON_H
#define COMMON_H

/*-------------------------------------------------------------*
 *		Includes and dependencies			*
 *-------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* communication defines the buffer maximum size and delay between each byte */
#define BUFFER_SIZE        32  
#define DELAY_MSG          1

/*defines  below  are used for crc32 send at the end of the file to the sender */
#define CHECKSUM_CORRECT   0xF9 
#define CHECKSUM_INCORRECT 0xF8

#define CRC_CHECK_SIZE     4
#define FILE_LENGTH        2



typedef struct {
  uint32_t crc32, addr, sectorStrAddr;
  uint8_t  encrypted_data_size;
}Firmware;

extern uint8_t _update;

typedef union{
  uint32_t crc32u;
  uint8_t  crc32_8u[4];
}crc32_firmware;


typedef union{
  uint16_t length16;
  uint8_t  length8[2];
}firmware_data_size;

extern firmware_data_size firmware_length;
extern crc32_firmware crc32_recieved;
extern Firmware firmware;

extern char buffer[BUFFER_SIZE];

#ifdef  __cplusplus
extern "C" {
#endif

void FLASH_WriteFirmware(void);
void FLASH_WriteLenguageStrings(void);
  
#ifdef  __cplusplus
}
#endif

#endif


