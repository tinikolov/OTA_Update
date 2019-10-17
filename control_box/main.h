#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

#include "crc.h"
#include "rs232.h"

//#define TESTING			0

#define buffer_size 	255

#define START_BYTE         			0x00
#define ACK                			0xFE
#define NAK                			0xFD
#define WRONG_HEADER       			0xFC
#define WRONG_LENGTH       			0xFB
#define CHECKSUM_CORRECT   			0xF9
#define CHECKSUM_INCORRECT 			0xF8
#define TRANSFER_COMPLETE  			0xF7
#define INVALID_SECTOR     			0xF6
#define SEND_FIRMWARE_UPDATE   		0xF5
#define SEND_EEPROM_UPDATE     	    0xF4
#define SEND_LENGUAGE_UPDATE		0xF3

#define MSG_TYPE_FIRMWARE_DATA      0x01
#define MSG_TYPE_CRC32              0x02
#define MSG_TYPE_CHECKSUM           0x03
#define MSG_TYPE_FIRMWARE_LENGTH    0x04
#define MSG_TYPE_FIRMWARE_VERSION	0x05
#define MSG_TYPE_EEPROM 			0x06
#define MSG_TYPE_LENGUAGE_UPDATE    0x07

#define BYTE    					8

FILE *logger;
FILE *firmware_encrypted;

bool crc_transfer, version_transfer;
char buffer[buffer_size];
char msg[buffer_size];
char data[BYTE];
uint8_t msg_length;
uint8_t data_length;
uint8_t  error_count;
char transfer_type;
uint8_t crc; /* 8 bit CRC */
uint32_t crc32b;
uint8_t msg_header;
uint16_t size;

//char version[BYTE] = "gf072c33";