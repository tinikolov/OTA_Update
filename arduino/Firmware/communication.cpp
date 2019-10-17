#include "communication.h"
#include "common.h"

/* The array defines the function pointers based on the current state of the communication
 *  It ensure that the state is always one of the states from the communication by relating it to a function.
 *  In case state have a different value from the one defined in the communication.h enum it will not perform any action
 */
COMM_stateMachine FSM[6] = 
{
    {idle, comm_idle},
    {read_header, comm_read_header},
    {read_length, comm_read_length},
    {read_data_length, comm_read_data_length},
    {process_msg, comm_process_msg},
    {crc_rx, comm_check_crc},
};

/* init the enum states */
COMM_StateType comm_state;


/*-------------------------------------------------------------*
 *    Function prototypes  *
 *-------------------------------------------------------------*/
uint8_t isEnum(uint8_t x);
uint8_t crc,buffer_index;
uint8_t header_type,msg_length,data_length;

/* global buffer used in many cases for the firmware update to process the line once it's received 
 * The length of the buffer should be always equal or larger to the maximum size of one line in the firmware update (currently 17 bytes)
 * The value is defined as extern in common.h
 */
char buffer[];

/* This function is executed on the initial state of the communication process. 
 *  The state is only changed once a start byte is received.
 */
void comm_idle(char *c){
    
    if((uint8_t) *c == START_BYTE){
      comm_state = read_header;
    }
}

/* The function is processing the next byte of the communication which is the header of the message.
 *  It compares the header  with the pre defined headers under ENUM_VALUES in communication.h and in case of valid header it changes the state,else it returns to the
 *  idle state
 */
void comm_read_header(char *c){
  header_type =(uint8_t) *c;
  if(isEnum(header_type )){
    comm_state = read_length;
     
  }else{
    comm_state = idle;
  }
}

/* The function is processing the next byte of the communication which is the length of the message. 
 *  If the length of the message is  valid it change the state to read the next byte
 *  Else it returns to idle
 *  msg_length variable is used for storing the data to the SPI flash 
 */
void comm_read_length(char *c){
  msg_length =(uint8_t) *c;
   
  if(msg_length != 0x00 && msg_length != 0xFF && msg_length <= BUFFER_SIZE){
    comm_state = read_data_length;
  }else{
    comm_state = idle;
  }
}

/* The function is processing the next byte of the communication which is the data langth of the decrypted firmware line. 
 *  The data_length is used in the bootloader for retrieving the correct firmware data after decryption
 *  If the data length is  valid it change the state to read the next byte
 *  Else it returns to idle
 */
void comm_read_data_length(char *c){
   data_length = (uint8_t) *c;
   if(data_length != 0x00 && data_length != 0xFF && data_length <= BUFFER_SIZE){
         comm_state = process_msg;
    }else{
       comm_state = idle;
    }
}

/*  The function is processing the next bytes of the communication until it reaches. 
 *  the state of communication is changed only when the buffer index reaches the size of the msg length - 1 (as the last byte of the communication is the crc)
 */
void comm_process_msg(char *c){
  buffer[buffer_index] = *c;
  
  comm_calculate_crc(&crc,c);
  if(buffer_index == msg_length - 1){
     
    comm_state = crc_rx;
    buffer_index = 0;
    return;
  }
  buffer_index++;
}

/* The function is used to calculate the crc of the received msg in comm_process_msg()  */
void comm_calculate_crc(uint8_t *crc, char *c){
      (*crc) ^= ((uint8_t) *c);
}

/* The function retrieves the last byte received from the communication (crc of the message) and compares it with the calculated
 *  crc of the received data to the arduino board
 *  If the crc is correct it send acknolegement to the raspberry pi in order to receive the next message and call FLASH_WriteFirmware() to store it on the SPI flash
 *  Else it send NAK, meaning the same message will be re send from the raspberry pi
 */
void comm_check_crc(char *c){

    if((uint8_t) *c != crc){
       Serial.write(ACK);
       delay(DELAY_MSG);
    }else{
      Serial.write(ACK);
      delay(DELAY_MSG);
        FLASH_WriteFirmware();
    }

    comm_state = idle;
    crc = 0;
    buffer_index = 0;
}

/* Function to check if the received header is valid */
uint8_t isEnum(uint8_t x){
  
    for(uint8_t i = 0; i < sizeof(headers); i++){
        if(headers[i] == x){ return 1; }
    }
    return 0;
}



