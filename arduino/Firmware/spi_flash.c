#include "spi_flash.h"

// internal prototypes
void FLASH_writeEnable(void);
uint8_t SPI_transfer(uint8_t data);
uint8_t FLASH_busy(void);
uint16_t readDeviceId(void);

/* -============================== SPI DRIVER ==========================- */

bool SPI_init(void){

#ifdef __AVR_ATmega2560__
    DDRB |= _BV(SPI_SS_PIN) | _BV(FLASHSS_PIN) | _BV(SPI_MOSI_PIN) | _BV(SPI_CLK_PIN); //OUTPUTS for FLASH_SS and SS, MOSI, SCK
    FLASH_UNSELECT; //unselect FLASH chip
    PORTB |= _BV(FLASHSS_PIN); //set SS HIGH
#else
    SPI_DDR |= _BV(SPI_SS_PIN) | _BV(SPI_MOSI_PIN) | _BV(SPI_CLK_PIN); // OUTPUTS for SS, MOSI, SCK
    FLASHSS_DDR |= _BV(SPI_SS_PIN);
    SPI_PORT |= _BV(SPI_SS_PIN); //PB2 output: High (deselect other SPI chips) 
#endif
    // Warning: if the SS pin ever becomes a LOW INPUT then SPI automatically switches to Slave, so the data direction of the SS pin MUST be kept as OUTPUT.
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
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
    return SPDR;
  }


uint8_t FLASH_busy(void){

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

/* Write 1 byte to flash memory
 *  1. addr - starting address
 *  2. data - byte to save in flash
   WARNING: you can only write to previously erased memory locations (see datasheet)
          use FLASH_BlockErase to first clear memory (write 0xFFs) */
void FLASH_writeByte(uint32_t addr, uint8_t data){
  

  FLASH_writeEnable();
  SPI_transfer(SPIFLASH_BYTEPAGEPROGRAM);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  SPI_transfer(data);
  FLASH_UNSELECT;
   FLASH_writeEnable();
}

void FLASH_writeBytes(uint32_t addr,const void* buf, uint16_t len) {
  uint16_t n;
  uint16_t maxBytes = 256-(addr%256);  // force the first set of bytes to stay within the first page
  uint16_t offset = 0;
  while (len>0)
  {
    n = (len<=maxBytes) ? len : maxBytes;
    FLASH_writeEnable();
    SPI_transfer(SPIFLASH_BYTEPAGEPROGRAM);
    SPI_transfer(addr >> 16);
    SPI_transfer(addr >> 8);
    SPI_transfer(addr);
    
    for (uint16_t i = 0; i < n; i++)
     
      SPI_transfer(((uint8_t*) buf)[offset + i]);
    FLASH_UNSELECT;
    FLASH_writeEnable();
    addr+=n;  // adjust the addresses and remaining bytes by what we've just transferred.
    offset +=n;
    len -= n;
    maxBytes = 256;   // now we can do up to 256 bytes per loop
  }
}
/* function used in erase chip and write byte functions to enable write operations. */ 
void FLASH_writeEnable(void){
 
    FLASH_SELECT    
    SPI_transfer(SPIFLASH_WRITEENABLE);
    FLASH_UNSELECT
    while(FLASH_busy());
    FLASH_SELECT
}

/* Erase part of the SPI flash 
 *  1. addr - starting addr to begin the erasing
 *  2. size - supported size are currently 4 KB and 32 KB 
 */
void FLASH_BlockErase(uint32_t addr,uint8_t size) {

   FLASH_writeEnable();
  
  if(size == 4){
    SPI_transfer(SPIFLASH_BLOCKERASE_4K);
  }else if(size == 32){    
     SPI_transfer(SPIFLASH_BLOCKERASE_32K);
  }else{
    FLASH_UNSELECT
    return;
  }
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);  
  FLASH_UNSELECT
  FLASH_writeEnable();
}


