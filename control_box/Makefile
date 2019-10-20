
CC=gcc
CFLAGS = -g -Wall -lm
EXEC = ./transfer

#source common to all targets
C_SOURCE_FILES +=  main.c \
rs232.c \
crc.c	


all: 
	$(CC) $(CFLAGS) $(C_SOURCE_FILES) -o $(EXEC)

run:  all
	$(EXEC)
