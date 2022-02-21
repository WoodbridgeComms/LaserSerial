#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <asm/termbits.h>
#include <linux/serial.h>

#define DIV_MASK		0x1FFF //Bits 0 -> 13 are integer divisors; 14 -> 16 are fractional

/*
	Obtains the constant definition for a specified baud rate
	Constants are not currently used
*/
int baud_to_const(int baud)
{
#define BAUD(n) case n: return B##n
	switch(baud)
	{
		BAUD(50); 	BAUD(75); 	BAUD(110);	BAUD(134); 	BAUD(150);
		BAUD(200);	BAUD(300); 	BAUD(600); 	BAUD(1200);	BAUD(1800);   	
		BAUD(2400);	BAUD(4800);	BAUD(9600);	BAUD(19200);	BAUD(38400);
		BAUD(57600);	BAUD(115200);	BAUD(230400);	BAUD(460800);	BAUD(500000);
		BAUD(576000);	BAUD(921600);	BAUD(1000000);	BAUD(1152000);	BAUD(1500000);
		BAUD(2000000);	BAUD(2500000);	BAUD(3000000);	BAUD(3500000);	BAUD(4000000);
		default:
			return -1;
	}
#undef BAUD
}

/*
	Returns the divisor used by the FT232H for a specified baud rate
*/
float get_divisor(int baud)
{
	unsigned char frac_div[8] = {0, 3, 2, 4, 1, 5, 6, 7};
	float frac[8] = {0, 0.5, 0.25, 0.125, 0.375, 0.625, 0.75, 0.875};
	int divisor, div;
	
	//Obtain divisor bytes
	div = (int) (120000000.0f * 8 / (baud * 10) + 0.5);
	divisor = div >> 3;
	divisor |= frac_div[div & 0x7] << 14;
	if (divisor == 1)
		divisor = 0;

	//Parse through bytes for integer & fractional components
	return (divisor & DIV_MASK) + frac[divisor >> 14];
}

/*
	Check user's disired baud rate:
	If it is not predifined by termios and its aliased form exceeds 3% from
	the desired baud rate, it will notify the user of what the real baud
	rate is.
	For all legal values, it changes the FT232H's baud rate.
*/
int main(int argc, char *argv[])
{
	struct termios2 term;
	struct serial_struct ftinfo;
	int fd, old, baud = 0, inp_baud = 0;
	float aliased_baud;
	
	//Check if there are insufficient arguments
	if (argc != 2)
	{
		fprintf(stderr, "argument error: input requires one integer argument\n");
		return -1;
	}
	
	inp_baud = atoi(argv[1]);
	aliased_baud = inp_baud;
	
	if ((inp_baud < 1200) || (inp_baud > 12000000))
	{
		printf("Only rates between 1200 and 12000000 are supported\n");
		exit(EXIT_FAILURE);
	} else if (inp_baud > 6000000)
	{
		printf("Rates above 6 MBaud are often inaccurate.");
	}

	//Open serial port
	if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
	{
		fprintf(stderr, "Open_port: Unable to open /dev/ttyUSB0\n");
		exit(EXIT_FAILURE);
	}
	
	if (ioctl(fd, TCGETS2, &term) < 0)
	{
		fprintf(stderr, "ioctl: error fetching port attributes\n");
		exit(EXIT_SUCCESS);
	}
	
	//Search through baud rate constants
	baud = baud_to_const(inp_baud);

	//Find aliased baud rate if it is not pre-defined
	if (baud == -1)
	{
		aliased_baud = 12000000.0f / get_divisor(inp_baud);
		
		if (aliased_baud > inp_baud * 1.03 || aliased_baud < inp_baud * 0.97)
			fprintf(stderr, "Custom serial speed %d exceeds +/-3%% of FT232H's abilities. "
			 "Speed set to closest rate of %.2f\n", inp_baud, aliased_baud);
  	}
  	
  	if (ioctl(fd, TCGETS2, &term) < 0)
  		exit(EXIT_FAILURE);
  		
	old = term.c_ospeed; 
	
	//Sets the new baud rate
	//There is no way to use both cfsetospeed() and ioctl()
	term.c_cflag &= ~CBAUD;
	term.c_cflag |= BOTHER;
	term.c_ospeed = inp_baud;
	
	if (ioctl(fd, TCSETS2, &term) < 0)
	{
		fprintf(stderr, "ioctl: error setting baud rate\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Output baud rate changed from %d to %d (aliased to %.0f)\n", old, inp_baud, aliased_baud);
	
	close(fd);
	return EXIT_SUCCESS;
}
