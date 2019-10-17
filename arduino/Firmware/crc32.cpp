#include "crc32.h"
#include "common.h"

#include <Arduino.h>


void calculate_crc32(uint32_t *crc, char *data, uint8_t len){
   
   int8_t i = 0, j;
   uint32_t r_byte, mask;
   while (len-- > 0) {
      r_byte = (uint32_t) data[i++];            // Get next byte.
      *crc ^= r_byte;
      for (j = 7; j >= 0; j--) {    // 32 bits = 4 bytes. Find all masked bits in hex ( 8 times)
          mask = -(*crc & 1);
         *crc = (*crc >> 1) ^ (0xEDB88320 & mask);
      }
    
   }
}

/* The function is checking the calculated crc after reading the SPI flash data and received crc32 from the raspberry pi  */
bool check_crc32(uint32_t crc){
  if(crc != crc32_recieved.crc32u){
     Serial.write(CHECKSUM_INCORRECT);
     delay(DELAY_MSG);
     return false;
  }else{
     Serial.write(CHECKSUM_CORRECT);
     delay(DELAY_MSG);  
     return true;
  }
}


