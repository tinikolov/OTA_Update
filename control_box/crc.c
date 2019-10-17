/*------------------------------------------------------------
 * CRC calculation
 *------------------------------------------------------------
 */

#include "crc.h"

/* Calculate 8 bit crc */
void crc8(uint8_t *crc, char data)
{
	(*crc) ^= ((uint8_t) data);
}

void crc16(uint16_t *crc, char data)
{

    *crc  = (unsigned char)(*crc >> 8) | (*crc << 8);
    *crc ^= data;
    *crc ^= (unsigned char)(*crc & 0xff) >> 4;
    *crc ^= (*crc << 8) << 4;
    *crc ^= ((*crc & 0xff) << 4) << 1;

}

void crc32(uint32_t *crc, char *data, uint8_t len){
   
   int8_t i = 0, j;
   uint32_t byte, mask;
   while (len-- > 0) {
      byte = (uint32_t) data[i++];            // Get next byte.
      *crc ^= byte;
      for (j = 7; j >= 0; j--) {    /* 32 bits = 4 bytes. Find all masked bits in hex ( 8 times) */
          mask = -(*crc & 1);
         *crc = (*crc >> 1) ^ (0xEDB88320 & mask);
      }
    
   }
}
