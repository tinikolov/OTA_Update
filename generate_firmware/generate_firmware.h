#ifndef __GENERATEFIRMWARE_H
#define __GENERATEFIRMWARE_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "aes.h"
#include "crc.h"

#define buffer_size 	255
#define msg_size 		16
#define BYTE			8
#define REMOVED_BYTES	11
#define REMOVE_ADDR		9
#define FILE_LENGTH		2	

char buffer[buffer_size];
char msg[msg_size];


uint8_t transfer_type;

uint8_t length;
uint32_t crc32b;
uint16_t size;
uint16_t line_size;
uint8_t msg_count;
uint8_t crc;

FILE *firmware;
FILE *firmware_encrypted;

uint8_t key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

char version[BYTE];
#endif
