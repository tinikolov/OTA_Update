#include "spi_flash.h"

// internal prototypes
uint8_t SPI_transfer(uint8_t data);
uint8_t FLASH_busy(void);
uint16_t readDeviceId(void);
/* -============================== SPI DRIVER ==========================- */
bool SPI_init(void){
   
    SPI_DDR |= _BV(SPI_SS_PIN) | _BV(SPI_MOSI_PIN) | _BV(SPI_CLK_PIN); // OUTPUTS for SS, MOSI, SCK
    FLASHSS_DDR |= _BV(SPI_SS_PIN);
    SPI_PORT |= _BV(SPI_SS_PIN); //PB2 output: High (deselect other SPI chips) 

    /* Warning: if the SS pin ever becomes a LOW INPUT then SPI automatically switches to Slave,
     * so the data direction of the SS pin MUST be kept as OUTPUT. */
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0); //enable SPI and set SPI to MASTER mode
   
    /*read first 2 bytes of JEDECID, if chip is present it should return a non-0 and non-FF value */
    uint16_t deviceId = readDeviceId();
    if (deviceId==0 || deviceId==0xFF) return false;

    return true;
}

uint8_t SPI_transfer(uint8_t data) {
    SPDR = data;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
     __asm__ __volatile__("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
    return SPDR;
  }


uint8_t FLASH_busy(void) {

  FLASH_SELECT;
  SPI_transfer(SPIFLASH_STATUSREAD);
  uint8_t status = SPI_transfer(0);
  FLASH_UNSELECT;

  return status & 1;
}

/* Return device unique ID. Used to verify the connection with the FLASH during initiliaziation */
uint16_t readDeviceId(void)
{
  FLASH_SELECT
  SPI_transfer(SPIFLASH_JEDECID);
  uint16_t jedecid = SPI_transfer(0) << 8;
  jedecid |= SPI_transfer(0);
  FLASH_UNSELECT
  return jedecid;
}

/* Read array of bytes 
 *  1. *buf - array where the data will be stored
 *  2. addr - starting address 
 *  3. len - number of bytes to read 
 */
void Flash_readByteArray(void *buf, uint32_t addr, uint8_t len){

    while(FLASH_busy());
    FLASH_SELECT
    SPI_transfer(SPIFLASH_ARRAYREAD);
    SPI_transfer(addr >> 16);
    SPI_transfer(addr >> 8);
    SPI_transfer(addr);
    SPI_transfer(0); //"dont care"
    
    for (uint8_t i = 0; i < len; ++i)
      ((uint8_t*) buf)[i] = SPI_transfer(0);
    FLASH_UNSELECT
}

/* Read one byte from SPI flash with giving the address.
 *  Function return the read data from this address 
 */
uint8_t FLASH_readByte(uint32_t addr) {

  while(FLASH_busy());
  FLASH_SELECT
  SPI_transfer(SPIFLASH_READ_DATA);
  SPI_transfer((addr >> 16) & 0xFF);
  SPI_transfer((addr >> 8) & 0xFF);
  SPI_transfer(addr & 0xFF);

  uint8_t result = SPI_transfer(0);
  FLASH_UNSELECT

  return result;
}


/* -============================== END SPI DRIVER ==========================- */