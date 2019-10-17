#include "generate_firmware.h"

void init(void);
void GenerateFile(void);
void atoh(char *ascii_ptr, char *hex_ptr,int len);
void encrypt(void);
void ReadFile(void);
void SaveEncryptedLine(char buffer[], uint8_t length);
void AppendCRC(void);
void AppendVersion(void);

void init(void){
   length = 0;
   crc = 0;
   crc32b = 0;
   msg_count = 0;
   memset(&buffer,0x00,sizeof(buffer));
   memset(&msg,0x00,sizeof(msg));
 
}

void GenerateFile(void){
	if(transfer_type == 0){
		firmware = fopen("firmware.hex","r");
		fprintf(stderr, "Firmware\n" );
	}
	firmware_encrypted = fopen("firm_encrypted.hex", "a");
    if(firmware == NULL){
      printf("File is not available \n");
      exit(0);
    }
    if(firmware_encrypted == NULL){
    	printf("Encryption  is not available\n");
    	exit(0);
    }   
     fprintf(firmware_encrypted, "%.02X\n",transfer_type);	 
     fprintf(firmware_encrypted, "%.04X\n",size);
}

void atoh(char *ascii_ptr, char *hex_ptr,int len){
    int i;
    for(i = 0; i < (len / 2); i++){
    *(hex_ptr+i)   = (*(ascii_ptr+(2*i)) <= '9') ? ((*(ascii_ptr+(2*i)) - '0') * 16 ) :  (((*(ascii_ptr+(2*i)) - 'A') + 10) << 4);
    *(hex_ptr+i)  |= (*(ascii_ptr+(2*i)+1) <= '9') ? (*(ascii_ptr+(2*i)+1) - '0') :  (*(ascii_ptr+(2*i)+1) - 'A' + 10);
    }
}

void encrypt(void){
   
    for (uint8_t i = 0; i < msg_count; i++){
    	crc8(&crc,(uint8_t)msg[i]);
    }

    memset(&msg[msg_count],0x00,sizeof(msg) - msg_count);
    AES_CBC_encrypt_buffer(&ctx,(uint8_t *) msg, msg_size);
  	crc32(&crc32b,msg,msg_size);
    for (int i = 0; i < msg_size; ++i){
    	fprintf(firmware_encrypted, "%.02X",(uint8_t)msg[i]);
    }    
    fprintf(firmware_encrypted, "%.02X\n",crc);
    crc32(&crc32b,(char *)&crc,1);
    crc = 0;
}

void SaveEncryptedLine(char buffer[],uint8_t length){

	uint8_t start_byte = REMOVE_ADDR;
	for (int i = 0; i < length*2; i +=2)
	{
		atoh(&buffer[start_byte],&msg[msg_count],2);
   		start_byte += 2;
   		if (msg_count == 16)
   		{
   			fprintf(firmware_encrypted, "%02X",msg_count);
   			encrypt();
   			memset(&msg,0x00,sizeof(msg));
			msg_count = 0;
   		}
   		msg_count++;
	}
	if(length != 0){
		fprintf(firmware_encrypted, "%02X",msg_count);
		encrypt();
	}
	msg_count = 0;
}

void AppendCRC(void){
	buffer[0] = size >> 8;
	buffer[1] = size;
	crc32(&crc32b,buffer,FILE_LENGTH);
	fprintf(firmware_encrypted, "C:");
	fprintf(firmware_encrypted, "%08X",crc32b);
	fprintf(firmware_encrypted, "\n");
}

void PrintRAWFile(uint8_t length){
	fprintf(stderr, "Length: %d ",length );
	 for (int i = 0; i < length; ++i){
        fprintf(stderr, "%c",buffer[i] );
     }
     fprintf(stderr, "\n");
}

void ReadFile(void){
    char ch;

    while ((ch = fgetc(firmware)) != EOF){
            if(ch != '\n'){
                buffer[length] = ch;
                length++;
            }else{
            	//PrintRAWFile(length);
            	if(buffer[0] == ':'){
   					atoh(&buffer[1],(char *)&line_size,2);
            	}
            	SaveEncryptedLine(buffer,line_size);
                length = 0;
            }
        }    
     AppendCRC();
}

int main(int argc,char* argv[]){

	/* include version input */
	if (argc != 2){
        printf("Usage: ./comm version size\n");
        return 1;
    }
   
	AES_init_ctx_iv(&ctx, key, iv);  
  size = atoi(argv[1]);
  transfer_type = 0;
  fprintf(stderr, "Size: %d\n",size);
	init();
	GenerateFile();
	ReadFile();
	printf("\n FILE SAVED SUCCESFULLY\n");
	return 0;
}