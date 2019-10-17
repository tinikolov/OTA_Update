
/*
 * @param[in] data The input data block for computation.
 * @param[in] size   The size of the input data block in bytes.
 * @param[in] old_crc  The previous calculated CRC-16 value or NULL if first call.  
 *
 * @return The updated CRC-16 value, based on the input supplied.
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void crc8(uint8_t *crc, char data);
void crc16(uint16_t *crc,char data);
void crc32(uint32_t *crc, char *data, uint8_t len);
