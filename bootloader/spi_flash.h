#ifndef SPIFLASH_H
#define SPIFLASH_H

/*-------------------------------------------------------------*
 *		Includes and dependencies			*
 *-------------------------------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#define INIT_FLASH_ADDRESS              0x7fffff
#define FIRMWARE_SIZE                   ((uint32_t)0x8FFF)  
#define SECTOR_FLAGS_END_ADDRESS        0xFFF  
#define SECTOR_1_START_ADDRESS          0x01 + SECTOR_FLAGS_END_ADDRESS*2
#define SECTOR_END_ADDRESS(s)           (uint32_t) (s + FIRMWARE_SIZE)
#define SECTOR_LENGTH(l)                l + CRC_CHECK_SIZE
#define SECTOR_CRC(c)                   c
#define SECTOR_OFFSET                   ((uint32_t) (CRC_CHECK_SIZE + FILE_LENGTH))
#define SECTOR_SBOX_VALUE               0x01 + SECTOR_FLAGS_END_ADDRESS
#define SECTOR_RSBOX_VALUE              SECTOR_SBOX_VALUE + 0x100  

#define UPDATE                          0x44 // 'D'

#define SPI_DDR             DDRB        /* DDR SPI of MCU  */
#define SPI_PORT            PORTB       /* SPI port of MCU */ 
#define SPI_MISO_PIN        PB3         /* DataOut */
#define SPI_MOSI_PIN        PB2         /* DataIn */
#define SPI_CLK_PIN         PB1         /* Clock  */
#define SPI_SS_PIN          PB0         /* SS pin of SPI interface */

#define FLASHSS_DDR         PORTC       /* external SPI flash port */
#define FLASHSS_PIN         PINC7         /* SPI flash pin */


/* flash registers */
#define SPI_ENABLE 		  { SPCR |= _BV(MSTR) | _BV(SPE); } //enable SPI and set SPI to MASTER mode
#define SPI_DISABLE       { SPCR &= ~_BV(SPE); }
#define FLASH_SELECT      { FLASHSS_DDR &= ~(_BV(FLASHSS_PIN)); } /* select FLASH chip */
#define FLASH_UNSELECT    { FLASHSS_DDR |= _BV(FLASHSS_PIN); }    /* unselect FLASH chip */

#define SPIFLASH_JEDECID            0x9F  /* read JEDEC ID */

#define SPIFLASH_STATUSWRITE        0x01        /* write status register */
#define SPIFLASH_WRITEENABLE        0x06        /* write enable */
#define SPIFLASH_WRITEDISABLE       0x04        /* write disable */
#define SPIFLASH_BYTEPAGEPROGRAM    0x02        /* page program for writing byte or bytes (up to 256) */

#define SPIFLASH_STATUSREAD         0x05  /* read status register */
#define SPIFLASH_ARRAYREAD          0x0B  /* read array (fast, need to add 1 dummy byte after 3 address bytes) */
#define SPIFLASH_READ_DATA          0x03  /* read data array (low frequency) */

#define SPIFLASH_BLOCKERASE_4K      0x20        /* erase one 4K block of flash memory */
#define SPIFLASH_BLOCKERASE_32K     0x52        /* erase one 32K block of flash memory */
#define SPIFLASH_CHIPERASE          0x60        /* chip erase (may take several seconds) */


#ifdef	__cplusplus
extern "C" {
#endif

bool SPI_init(void);
uint8_t FLASH_readByte(uint32_t addr);
void Flash_readByteArray(void *buf, uint32_t addr, uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif




