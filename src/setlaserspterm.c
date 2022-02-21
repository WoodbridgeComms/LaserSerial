#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	struct termios term;
	speed_t baud;
	int fd;
	
	switch(atoi(argv[1]))
	{
		case 0:
			baud = B0;
			break;
		case 50:
			baud = B50;
			break;
		case 75:
			baud = B75;
			break;
		case 110:
			baud = B110;
			break;
		case 134:
			baud = B134;
			break;
		case 150:
			baud = B150;
			break;
		case 200:
			baud = B200;
			break;
		case 300:
			baud = B300;
			break;
		case 600:
			baud = B600;
			break;
		case 1200:
			baud = B1200;
			break;
		case 1800:
			baud = B1800;
			break;
		case 2400:
			baud = B2400;
			break;
		case 4800:
			baud = B4800;
			break;
		case 9600:
			baud = B9600;
			break;
		case 19200:
			baud = B19200;
			break;
		case 38400:
			baud = B38400;
			break;
		case 57600:
			baud = B57600;
			break;
		case 115200:
			baud = B115200;
			break;
		case 230400:
			baud = B230400;
			break;
		case 460800:
			baud = B460800;
			break;
		case 500000:
			baud = B500000;
			break;
		case 576000:
			baud = B576000;
			break;
		case 921600:
			baud = B921600;
			break;
		case 1000000:
			baud = B1000000;
			break;
		case 1152000:
			baud = B1152000;
			break;
		case 1500000:
			baud = B1500000;
			break;
		case 2000000:
			baud = B2000000;
			break;
		case 2500000:
			baud = B2500000;
			break;
		case 3000000:
			baud = B3000000;
			break;
		case 3500000:
			baud = B3500000;
			break;
		case 4000000:
			baud = B4000000;
			break;
		default:
			printf("Input a termios defined baud rate constant\n");
			return EXIT_FAILURE;
	}

	//Add error checking if this becomes used
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	tcgetattr(fd, &term);
	cfsetospeed(&term, baud);
	//term.c_cflag &= ~CSTOPB;
	//term.c_cflag &= ~CSIZE;
	//term.c_cflag |= CS8;
	tcsetattr(fd, TCSANOW, &term);
	close(fd);
	return EXIT_SUCCESS;
}
