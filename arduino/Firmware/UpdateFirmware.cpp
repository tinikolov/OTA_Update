#include "UpdateFirmware.h"

// encryption boxes used from bootloader to decrypt the received firmware. 
uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
  
uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };


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

/* write the sbox and rsbox values to the SPI Flash so the bootloader can decrypt the stored information. The operation is performed only in case the first element of the encryption
 *  boxes sector is 0xFF */
void WriteEncryptionBoxes(void){
  firmware.addr = SECTOR_SBOX_VALUE;
  FLASH_writeBytes(firmware.addr,sbox,sizeof(sbox));
  firmware.addr += sizeof(sbox);
  FLASH_writeBytes(firmware.addr,rsbox,sizeof(rsbox));

}

/* initilization of the variables used in transferring the firmware to SPI flash */
void Init_firmwareUpdate(void){
  
    transfer_complete = false;
    comm_state = idle;
    uint8_t sbox_check = FLASH_readByte(SECTOR_SBOX_VALUE);
     if(sbox_check == 0xFF){
        WriteEncryptionBoxes();
     }
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
