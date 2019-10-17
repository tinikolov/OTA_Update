#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <math.h>
#include <time.h>


void rs232_open(void);
void rs232_close(void);
int  rs232_putchar(char c);
int  rs232_getchar_nb(char *c);
char 	rs232_getchar();
