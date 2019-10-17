#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>
#ifdef	__cplusplus
extern "C" {
#endif
	
	void calculate_crc32(uint32_t *crc, char *data, uint8_t len);
	bool check_crc32(uint32_t crc);
#ifdef	__cplusplus
}
#endif

#endif


