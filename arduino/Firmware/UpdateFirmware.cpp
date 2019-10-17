#include "UpdateFirmware.h"

/*-------------------------------------------------------------*
 *    Function prototypes  *
 *-------------------------------------------------------------*/
void WriteData(uint32_t *startAddr,uint16_t len);
void CheckTransfer(uint32_t startAddr);


/*struct and unions accesible from other files. Declared as extern in common.h file and initiliazed only here as 
 * local variables. All other files must only used the data types as external */
Firmware firmware;
crc32_firmware crc32_recieved;
firmware_data_size firmware_length;

uint8_t _update; // flag to indicate a new version uploaded to the SPI flash. the value is stored at 0x00 of the SPI Flash memory chip to indicate an update for bootloader
bool transfer_complete; // once the last byte of the 32-bit checksum is received the flag indicates that data could be read from the SPI flash and check for succesfull transfer
/* end of external variables */

/* initilization of the variables used in transferring the firmware to SPI flash */
void Init_firmwareUpdate(void){
  
    transfer_complete = false;
    comm_state = idle;
    memset(&firmware,0x00,sizeof(firmware));
}

/* init firmware update. The function should be called in the setup() loop */
void SetupFirmwareUpdate(void){
  SPI_init(); 
  Init_firmwareUpdate();
  EraseFirmwareSection(0x00,SECTOR_FLAGS_END_ADDRESS); // erase the UPDATE flag at each start, so the bootloader won't update again 
}

/* main function for receiving Firmware Update
 * It read bytes from Serial if any available and process them by calling a function based on the state of the communication state machine.
 *  Once all bytes are received or expiration time is reached it continue to function CheckTransfer()
 *  dependencies:
 *  firmware.addr - set the initial address for writing the actual data of the firmware
 *  firmware.sectorStrAddr - used in the CheckTransfer to reset firmware.addr to the beginning of the data 
 */
void FirmwareUpdate(void){

  if(Serial.available() ){
      char c = Serial.read();
      delay(DELAY_MSG);
      if((uint8_t) c == RECIEVE_FIRMWARE_UPDATE){
        firmware.addr = SECTOR_1_START_ADDRESS + SECTOR_OFFSET;
        firmware.sectorStrAddr = SECTOR_1_START_ADDRESS;
        EraseFirmwareSection(firmware.sectorStrAddr,SECTOR_END_ADDRESS(firmware.sectorStrAddr));
      }     
  }else{
      return;
    }

  unsigned long expiration = millis() + 300;
  while(Serial.available() > 0 || millis() < expiration){
    if(Serial.available()){
      expiration = millis() + 50;
      char c = Serial.read();
      delay(DELAY_MSG);
      (*FSM[comm_state].func)(&c);
      }
  }

  CheckTransfer((uint32_t)  (firmware.sectorStrAddr + SECTOR_OFFSET));
  Init_firmwareUpdate(); // reset values if another firmware should be transffered before restart
}

/* Verify the received firmware or eeprom by reading the sector on which data is stored and calculating the CRC32 
 *  @params
 *  - uint32_t startAddr - the beginning of the firmware or eeprom data on the section 
 *  if the transfer complete boolean is set to true (the value is set to true only if the crc32 is received and processed by the communication state machine)
 *  For firmware update - crc32 is calculated by reading the encrypted data (16 bytes )  + 1 byte of crc(used to verify the correctness of the data in one record) 
 *  +  the total size of the firmware size
 */
void CheckTransfer(uint32_t startAddr){

   if(transfer_complete){
       firmware.crc32 = 0;
       FLASH_ReadFirmware(startAddr);
       ReadReceivedCRC((uint32_t) SECTOR_CRC(firmware.sectorStrAddr));
       ReadReceivedLength((uint32_t) SECTOR_LENGTH(firmware.sectorStrAddr));       
     }

    if(check_crc32(firmware.crc32)){
        _update = UPDATE;
        FLASH_writeByte(0x00,_update); // set the updating flag 
    }else{
       EraseFirmwareSection(firmware.sectorStrAddr,SECTOR_END_ADDRESS(firmware.sectorStrAddr)); // erase the sector if the checksum failed
    }     
}

/* Read the Firmware length from SPI flash 
 * the total size of the firmware is used in the bootloader in order to stop the uploading process at the correct time and not overwritting the whole flash
 * for every new upload. Thus it requires to be verified and added to the checksum 
 */
void ReadReceivedLength(uint32_t startAddr){
     Flash_readByteArray(&buffer,startAddr,FILE_LENGTH);
     calculate_crc32(&firmware.crc32,buffer,FILE_LENGTH);
}

/* Read the CRC32 from SPI flash 
 *  @params 
 *  - uint32_t addr - the start address of the CRC stored in the firmware sector
 */
void ReadReceivedCRC(uint32_t addr){
     uint8_t count = 3;
     for(uint8_t i = 0; i < 4; i++){
       crc32_recieved.crc32_8u[count--] = FLASH_readByte(addr);
       addr += 1;
     }
}

/*  Function to write the recieved version, total firmware size and final checksum of the file */
void WriteData(uint32_t *startAddr,uint16_t len){ 
   FLASH_writeBytes(*startAddr,&buffer[0],len);
   *startAddr += len;
}

/*  The function is writing(firmware data or eeprom data, crc32, version, firmware length) the received msg based on the header_type 
 *  If header is invalid it returns without saving any information to the SPI flash
 *  dependencies:
 *  msg_length - the total size of the message, for firmware data the size is fixed based on the encryption and final crc line (currently 16 bytes for encryption and 1 byte for crc)
 *  data_length - the decrypted data length of each line. Most of the data is with the max size 16 bytes however few lines have less size thus this byte is required in order to 
 *  correctly flush the chip from the bootloader
 *  transfer_complete - set to true once the crc32 is received from the raspberry pi. This is the last byte transmitted to the Arduino
 */
void FLASH_WriteFirmware(void){
    if(header_type == MSG_TYPE_FIRMWARE_DATA){
      FLASH_writeByte(firmware.addr,msg_length);
      firmware.addr += 1;
      FLASH_writeByte(firmware.addr,data_length);
      firmware.addr += 1;
      WriteData(&firmware.addr,msg_length);
    }else if(header_type == MSG_TYPE_CRC32){
        firmware.addr =  SECTOR_CRC(firmware.sectorStrAddr);
        WriteData(&firmware.addr,msg_length);
        transfer_complete = true;       
    }else if(header_type == MSG_TYPE_FIRMWARE_LENGTH){
        firmware.addr = (uint32_t) SECTOR_LENGTH(firmware.sectorStrAddr);
        WriteData(&firmware.addr,msg_length);
    }else{
         return;
    }
    
}

void ReadData(uint32_t *startAddr,uint8_t len){
   Flash_readByteArray(buffer,*startAddr,len);
   *startAddr += (uint32_t) len;
    calculate_crc32(&firmware.crc32,buffer,len);
}

/* The function reads the firmware data from the SPI flash 
 *  @params 
 *  - firmware.addr- the value is set to  the start address of the sector  from which the firmware data begins
 *  - uint32_t end_addr - the sector end address to exit the loop
 *  In case the first byte of the line (the msg length) is 0xFF the loop is exit earlier by using the else condition
 */
void FLASH_ReadFirmware(uint32_t startAddr){ 
    while(startAddr <= (uint32_t)SECTOR_END_ADDRESS(firmware.sectorStrAddr)){
        firmware.encrypted_data_size = FLASH_readByte(startAddr);
        if(firmware.encrypted_data_size != 0xFF){
          startAddr += 2; // skip data_length
          ReadData(&startAddr,firmware.encrypted_data_size);
        }else{
            break;
        }   
    }
}

/* The function erase only a section of the SPI flash  
 *  @params 
 *  - uint32_t start_addr- the sector start address 
 *  - uint32_t end_addr - the sector end address 
 *  As the minimum allowed erase operation is 4KB (4095 bytes) the function will loop until the start address is less then the end address 
 *  erasing the sector by blocks of 4 KB
 */
void EraseFirmwareSection(uint32_t start_addr, uint32_t end_addr){
    while(start_addr <  end_addr){
        FLASH_BlockErase(start_addr,4);
        start_addr += SECTOR_FLAGS_END_ADDRESS;
    }
}


/* The function erase the whole space resevered for firmware updates
 *  including the FLAG sector, Sector 1 and Sector 2
 */
void EraseSectors(void){
    uint32_t addr = 0x00;
    uint32_t end_addr = SECTOR_END_ADDRESS(SECTOR_1_START_ADDRESS+SECTOR_FLAGS_END_ADDRESS+0x01);
    while(addr < end_addr){
       FLASH_BlockErase(addr,4);
       addr += SECTOR_FLAGS_END_ADDRESS;
    }
}
