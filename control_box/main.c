#include "main.h"

void  delay_ms(unsigned int ms)
{
        struct timespec req, rem;
        req.tv_sec = ms / 1000;
        req.tv_nsec = 1000000 * (ms % 1000);
        assert(nanosleep(&req,&rem) == 0);
}

// send message to the Arduino 
// structure: [START-BYTE][header][message length][data length][record data][crc]
void send_message(){

    rs232_putchar(START_BYTE);
    rs232_putchar(msg_header);
    rs232_putchar(msg_length);
    rs232_putchar(data_length);
    for(uint8_t i = 0; i < msg_length; i++){   
        rs232_putchar(buffer[i]);
    }
    rs232_putchar(crc);
    crc = 0;   
}

// init the parameters and open the encrypted file
void init(){
    crc = 0;
    msg_length = 0;
    data_length = 0;
    crc_transfer = false;
    memset(buffer,0x00,buffer_size);
    memset(msg,0x00,buffer_size);
    crc32b = 0;
    error_count = 0;
    firmware_encrypted = fopen("firm_encrypted.hex", "r");
    if(firmware_encrypted == NULL){
      fprintf(stderr,"File is not available \n");
      exit(0);
    }    
}


// waits for received a confirmation from the Arduino if the file is received.
void file_transfered(){
    char c;
    for(;;){
    c =  rs232_getchar();
     if((uint8_t) c == CHECKSUM_CORRECT){
          fprintf(stderr,"\nFILE TRANSFERRED SUCCESFULLY\n");
          return;
     }else if((uint8_t) c == CHECKSUM_INCORRECT){
          fprintf(stderr,"\nCALCULATED AND RECIEVED CHECKSUM DOES NOT MATCH\n");    
          return;
     }
    }
    fprintf(stderr,"Exit\n");
}


// receive the response from the Arduino executed for each record. 
// If NAK is received it tries to send the same record 3 times and after that exit the program
void recieve_msg(){

    char c  =  rs232_getchar();

    if((uint8_t) c == ACK){
        fprintf(stderr,".");
        return;
    }
    while((uint8_t) c != ACK && (uint8_t) c != 0xFF && c != 0){
       
       for(uint8_t i = 0; i < msg_length; i++){
            crc8(&crc,buffer[i]);
       }
        fprintf(stderr," error code: %.02X\n",(uint8_t) c);
        error_count++;
        if(error_count >= 3){
                exit(0);
        }
        send_message();    
        c = rs232_getchar();
        if((uint8_t) c == ACK){
            fprintf(stderr,".");
            error_count = 0;
            return;
        }
    } 

}

char convert_to_char(char c[]){
     char result = 0;
    for (int i = 0; i < (sizeof(uint8_t) * 8); ++i)
    {
        result += (c[i] & 0x01) << i; 
    }
    return result;
}

void atoh(char *ascii_ptr, char *hex_ptr,int len)
{
    int i;
    for(i = 0; i < (len / 2); i++){
    *(hex_ptr+i)   = (*(ascii_ptr+(2*i)) <= '9') ? ((*(ascii_ptr+(2*i)) - '0') * 16 ) :  (((*(ascii_ptr+(2*i)) - 'A') + 10) << 4);
    *(hex_ptr+i)  |= (*(ascii_ptr+(2*i)+1) <= '9') ? (*(ascii_ptr+(2*i)+1) - '0') :  (*(ascii_ptr+(2*i)+1) - 'A' + 10);
    }

}

void SendLength(void){
    //fprintf(stderr,"size: %d",size);
    /* actual data size contained in the hex file*/
    msg_length = 2;
    data_length = 2;
    crc8(&crc,buffer[0]);
    crc8(&crc,buffer[1]);
    msg_header = MSG_TYPE_FIRMWARE_LENGTH;
    send_message();
    recieve_msg();
}


void SendCRC(void){
    
    data_length = 4;
    msg_length = 4;  
    atoh(data,buffer,8);
    fprintf(stderr, " CRC: ");
    for(uint8_t i = 0; i < data_length; i++){
        fprintf(stderr, "%.02X",(uint8_t) buffer[i]);
        crc8(&crc,buffer[i]);
    }
    
    msg_header = MSG_TYPE_CRC32;
}

void SendDataType(void){
    if(buffer[0] == 0){
        rs232_putchar(SEND_FIRMWARE_UPDATE);
        fprintf(stderr, "Sending Firmware data\n");
         delay_ms(250);  
    }
    transfer_type = buffer[0];
}


// Read the encrypted file line by line and process each character.
// 2 HEX digits are combined in one byte 
void ReadFile(){
    int ch;
    uint8_t count = 0;
    uint8_t len = 0;
    uint8_t line_count = 0;
    bool send_data = false;
    while((ch = fgetc(firmware_encrypted)) != EOF){
     if(line_count == 1){
         if(ch != '\n'){
            data[count] = ch;
            count++; 
         }else{
            atoh(data,buffer,4);
            SendLength();
            line_count++;
            count = 0;
            msg_length = 0;
            data_length = 0;
            send_data = true;
         }  
      }else{
        if(ch != '\n'){
         data[count] = ch;
         count++;
            if(data[0] == 'C' && data[1] == ':' && !crc_transfer){
                crc_transfer = true;
                count = 0;
            }else if (count == 2 && !version_transfer && !crc_transfer){
                if(len == 0 && line_count != 0){
                    atoh(data,(char *)&data_length,2);
                    len +=1;
                }else{ 
                    atoh(data,&buffer[msg_length],2);   
                    crc8(&crc,buffer[msg_length]);
                    msg_length++;
                }   
                count = 0;
            }
          
        }else{ 
            if(crc_transfer){  
                 SendCRC(); 
                 crc_transfer = false;
                  count = 0; 
            }else  if(transfer_type == 0){
                msg_header = MSG_TYPE_FIRMWARE_DATA;
            }

            if(send_data){
                send_message();
                recieve_msg();
            }

            if(line_count == 0){   
                SendDataType();
                line_count = 1;
            }
          
            len = 0;
            msg_length = 0;
        }
      }
    }

    file_transfered(); 
}

int main(void){
    
   rs232_open();
   printf("\nTerminal program\n");
   init();
   ReadFile();
   rs232_close();

    return 0;
}
