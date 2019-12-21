#include "tty.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/** Sets TTY to Innovate device specs:
 * 8 data bits
 * 1 stop bit
 * no parity
 * 19.2 kBaud
 * Further, no hardware flow control, and 1-byte/read minimum
 */
int setupTTY(int fd)
{
  struct termios tty;

  //tcgetattr(fd, &oldtio); // Save current port settings
  if (tcgetattr(fd, &tty) < 0) {
    fprintf(stderr, "Error (%d) from tcgetattr: %s\n", errno, strerror(errno));
    return -1;
  }

  //bzero(&tty, sizeof(tty));

  cfsetospeed(&tty, (speed_t)BAUDRATE);
  cfsetispeed(&tty, (speed_t)BAUDRATE);

  tty.c_cflag |= (CLOCAL | CREAD); //Ignore modem controls
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8; //8 data bits
  tty.c_cflag &= ~PARENB; //No parity bits
  tty.c_cflag &= ~CSTOPB; //1 stop bit
  tty.c_cflag &= ~CRTSCTS; //No hardware flowcontrol
  //tty.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  
  // Setup for non-canonical mode
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  //tty.c_iflag = IGNPAR;
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  //tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  tty.c_oflag &= ~OPOST;
  //tty.c_oflag = 0;

  // Block for VTIME while waiting for VMIN (1 byte or 1 sec)
  tty.c_cc[VMIN] = 1; //Min character to return
  tty.c_cc[VTIME] = 1; //Inter-character timer

  //tcflush(fd, TCIFLUSH);
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    fprintf(stderr, "Error (%d) from tcsetattr: %s\n", errno, strerror(errno));
    return -1;
  }

  return 0;
}

//TODO Restore old tty settings
//tcsetattr(fd, TCSANOW, &oldtio);
