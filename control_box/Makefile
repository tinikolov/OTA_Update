
CC=gcc
CFLAGS = -g -Wall -lm
EXEC = ./comm

#source common to all targets
C_SOURCE_FILES +=  main.c \
rs232.c \
crc.c	


all: 
	$(CC) $(CFLAGS) $(C_SOURCE_FILES) -o $(EXEC)

run:  all
	$(EXEC)
