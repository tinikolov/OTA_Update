#define  INCLUDE_FROM_CATERINA_C
#include "Caterina.h"
#include "spi_flash.h"
#include "aes.h"

#include "util/delay.h"

/*-------------------------------------------------------------*
 *		Function prototypes			*
 *-------------------------------------------------------------*/
void Init(void);
bool line_checksum(void);
void EraseBootPage(void);
void crc8(uint8_t *crc, char data);
void SPI_FlashImage(void);
/***************************************************************************************/

static bool RunBootloader = true;

// StartSketch() is called to clean up our mess before passing execution to the sketch.
void StartSketch(void)
{
	cli();
	/* Undo TIMER1 setup and clear the count before running the sketch */
	TIMSK1 = 0;
	TCCR1B = 0;
	
	/* Relocate the interrupt vector table to the application section */
	MCUCR = (1 << IVCE);
	MCUCR = 0;

	L_LED_OFF();
	TX_LED_OFF();
	RX_LED_OFF();

	/* jump to beginning of application space */
	__asm__ volatile("jmp 0x0000");
	
}

/** Main program entry point. This routine configures the hardware required by the bootloader, then continuously
 *  runs the bootloader processing routine until it times out or is instructed to exit.
 */
int main(void)
{

	MCUSR = 0;							// clear all reset flags	

	// Relocate the interrupt vector table to the bootloader section
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);
		 	
	LED_SETUP();
    TX_LED_OFF();
  RX_LED_OFF();
	Init();

  uint8_t _update = FLASH_readByte(0x00);

  if(_update == UPDATE){
	 while(RunBootloader){
      SPI_FlashImage();
		  boot_rww_enable();
	   }
  }
	SPI_DISABLE;
	StartSketch();
}

void Init(void){
	strAddr = 0; 
	current_byte = 0;
	nextAddress  = 0;

	memset(&buffer,0x00,sizeof(buffer));
	SPI_ENABLE; 
	SPI_init();	

  AES_init_ctx_iv(&ctx, key, iv);
}

void SPI_FlashImage(void){

   	/* READ SIZE OF THE FIRMWARE */
    imagesize = ((FLASH_readByte((uint32_t)SECTOR_LENGTH(SECTOR_1_START_ADDRESS))) << 8) | ((FLASH_readByte((uint32_t) (SECTOR_LENGTH(SECTOR_1_START_ADDRESS) + 0x01))) & 0xFF) ;
    if (imagesize%2!=0) return; /* basic check that we got even # of bytes */
    strAddr = SECTOR_1_START_ADDRESS + SECTOR_OFFSET;
	
    while(SECTOR_1_START_ADDRESS <= SECTOR_END_ADDRESS(SECTOR_1_START_ADDRESS)){
       read_data = FLASH_readByte(strAddr);
        if(read_data == 0xFF){
        	break;
        }
        Flash_readByteArray(buffer,strAddr + 0x01,read_data + 0x01);
		    AES_CBC_decrypt_buffer(&ctx,(uint8_t *) &buffer[1], read_data - 0x01);
        if(!line_checksum()){
          	break;
        }    
        strAddr += (uint32_t) read_data + 0x02; 
    }
}

void EraseBootPage(void){
	
	for (uint32_t CurrFlashAddress = 0; CurrFlashAddress < BOOT_START_ADDR; CurrFlashAddress += SPM_PAGESIZE){
        boot_page_erase(CurrFlashAddress);
        boot_spm_busy_wait();
        boot_page_write(CurrFlashAddress);
        boot_spm_busy_wait();
    }
}

/* Calculate 8 bit crc */
void crc8(uint8_t *crc, char data)
{
  (*crc) ^= ((uint8_t) data);
}

bool line_checksum(void){

    uint8_t calculated_crc_line = 0;
    uint16_t b;
   /* the checksum is computed by taking the data length byte + 2 byte for address + data */
    for(uint8_t i = 1; i <= buffer[0]; i++){
        crc8(&calculated_crc_line,(uint8_t) buffer[i]);
    }

    if((uint8_t)buffer[17] != calculated_crc_line){
    		return false;	
    }else{
        for(uint8_t i = 1; i < buffer[0]; i+=2){
           
            b  = buffer[i] & 0xFF;
            b |= buffer[i + 1] << 8;
            boot_page_fill(current_byte,b);
            current_byte += 2;
       if ((current_byte%SPM_PAGESIZE==0 || current_byte==imagesize)){ 
         TX_LED_ON();
         // Write from programming buffer
         boot_page_write(nextAddress); //(current_byte+2-SPM_PAGESIZE)
         boot_spm_busy_wait();
         nextAddress += SPM_PAGESIZE;
         	TX_LED_OFF();
        if(current_byte == imagesize){ 
        	RunBootloader = false;  	
        	}
        }
      }
    }

    return true;
}