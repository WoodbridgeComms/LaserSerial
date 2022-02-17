#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int main()
{
  int fd;

  if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
  {
    perror("open_port: Unable to open /dev/ttyUSB0\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    ioctl(fd, TIOCSBRK, NULL);
    printf("Successfully set BREAK ON\n");
    close(fd);
  }
  
  return EXIT_SUCCESS;
}
