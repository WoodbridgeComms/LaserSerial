#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <asm/termbits.h>

int main(int argc, char *argv[])
{
  int fd, rc;
  struct termios2 term; //Apparently, its recommended to use cfsetospeed() instead of termios flags
  
  if (argc != 2)
  {
    printf("Argument error: input requires one integer argument\n");
    return -1;
  }
  

  if ((fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
  {
    perror("Open_port: Unable to open /dev/ttyUSB0");
    exit(EXIT_FAILURE);
  }
  else
  {	
    rc = ioctl(fd, TCGETS2, &term);
    
    if (rc)
    {
        perror("Error fetching port attributes: ");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    printf("current output baud rate: %u\n", term.c_ospeed);
    
    term.c_cflag &= ~CBAUD;
    term.c_cflag |= BOTHER;
    term.c_ospeed = atoi(argv[1]);
    
    rc = ioctl(fd, TCSETS2, &term); //Baud rate exceeding +/-3% of FT232H's pre- & post-scalers will not work
    
    if (rc)
    {
        perror("Error changing baud rate: ");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    printf("new output baud rate: %u\n", term.c_ospeed);
    close(fd);
  }
  
  return fd;
}
