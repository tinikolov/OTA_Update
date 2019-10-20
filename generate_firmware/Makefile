CC=gcc
CFLAGS = -g -Wall -lm
EXEC = ./gen_firmware

#source common to all targets
C_SOURCE_FILES += aes.c \
		  crc.c \
		  generate_firmware.c

all: 
	$(CC) $(CFLAGS) $(C_SOURCE_FILES) -o $(EXEC)

run:  all
	$(EXEC)

