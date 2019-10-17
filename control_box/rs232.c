#include "rs232.h"

/*------------------------------------------------------------
 * Serial I/O
 * 8 bits, 1 stopbit, no parity,
 * 19,200 baud
 *------------------------------------------------------------
 */

int fd_RS232;

void rs232_open(void)
{
	char 	*name;
	int 	result;
	struct termios	tty;

   	fd_RS232 = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);  /* connection port
                     NOTE: check the port on which the controller is connected */ 

   	assert(fd_RS232>=0);

   	result = isatty(fd_RS232);
   	assert(result == 1);

   	name = ttyname(fd_RS232);
   	assert(name != 0);

   	result = tcgetattr(fd_RS232, &tty);
   	assert(result == 0);

	tty.c_iflag = IGNBRK; /* ignore break condition */
   	tty.c_oflag = 0;
   	tty.c_lflag = 0;

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bits-per-character */
	tty.c_cflag |= CLOCAL | CREAD; /* Ignore model status + read input */

   	cfsetospeed(&tty, B19200);
   	cfsetispeed(&tty, B19200);

   	tty.c_cc[VMIN]  = 0;
	tty.c_cc[VTIME] = 1; // added timeout

	tty.c_iflag &= ~(IXON|IXOFF|IXANY);

	result = tcsetattr (fd_RS232, TCSANOW, &tty); /* non-canonical */

	tcflush(fd_RS232, TCIOFLUSH); /* flush I/O buffer */
}

void rs232_close(void)
{
	int 	result;

	result = close(fd_RS232);
	assert (result==0);
}

int 	rs232_putchar(char c)
{
	int result;

	do {
		result = (int) write(fd_RS232, &c, 1);
	} while (result == 0);

	assert(result == 1);
	return result;
}

int	rs232_getchar_nb(char *c)
{
	int result;
	result = read(fd_RS232, c, 1);
	if (result == 0) {
		return -1;
    }else{
		assert(result == 1);
		return 0;
	}
}

char 	rs232_getchar()
{
	char c;
	if(rs232_getchar_nb(&c) != -1)
		;
	return c;
}




