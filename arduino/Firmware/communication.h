#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/*-------------------------------------------------------------*
 *		Includes and dependencies			*
 *-------------------------------------------------------------*/
#include <stdio.h> 
#include <stdbool.h>
#include <Arduino.h>

#define START_BYTE                0x00
#define ACK                       0xFE
#define NAK                       0xFD
#define WRONG_HEADER              0xFC
#define WRONG_LENGTH              0xFB
#define TRANSFER_COMPLETE         0xF7
#define INVALID_SECTOR            0xF6
#define RECIEVE_FIRMWARE_UPDATE   0xF5

/* Define header type values here and they will be automatically processed by the pre compiler in form of array */
#define ENUM_VALUES \
    X(MSG_TYPE_FIRMWARE_DATA, 0x01)  \
    X(MSG_TYPE_CRC32, 0x02)  \
    X(MSG_TYPE_CHECKSUM, 0x03) \
    X(MSG_TYPE_FIRMWARE_LENGTH, 0x04)  \
    X(MSG_TYPE_UNKNOWN,0xFF)
    
#define X(a, b) a = b,
    enum {
        ENUM_VALUES
    };
#undef X

#define X(a, b) a,
    const uint8_t headers[] = {
        ENUM_VALUES
    };
#undef X

/* states for the communication */
typedef enum{
    idle,
    read_header,
    read_length,
    read_data_length,
    process_msg,
    crc_rx,
}COMM_StateType;

extern COMM_StateType comm_state;

/* a struct to create the relation between the functions and the state */
typedef struct{
    
    COMM_StateType State;
    void (*func)(char *);
    
}COMM_stateMachine;

extern COMM_stateMachine FSM[6];
extern uint8_t header_type,msg_length,data_length;


#ifdef	__cplusplus
extern "C" {
#endif

void comm_idle(char *c);
void comm_read_header(char *c);
void comm_read_data_length(char *c);
void comm_read_length(char *c);
void comm_process_msg(char *c);
void comm_check_crc(char *c);
void comm_calculate_crc(uint8_t *crc, char *c);

#ifdef	__cplusplus
}
#endif

#endif


