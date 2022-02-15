#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int fd, duration;
  if (argc != 2)
  {
    printf("Argument error: input requires one integer argument\n");
    exit(EXIT_FAILURE);
  }
  
  duration = 1000 * atoi(argv[1]);

  if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
  {
    printf("open_port: Unable to open /dev/ttyUSB0\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    tcsendbreak(fd, duration);
    printf("Successfully sent %d sec BREAK to /dev/ttyUSB0 \n", atoi(argv[1]));
    close(fd);
  }
  
  return EXIT_SUCCESS;
}
