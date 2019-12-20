#include "../isp2.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#include <ncurses.h>

#define BAUDRATE B19200
#define ISP2_FLAGS_VERBOSE 0x01
#define ISP2_FLAGS_DELAY 0x02
#define ISP2_FLAGS_SERIAL 0x04

const char* statusMessage(int status) {
  switch (status) {
    case isp2_status::ISP2_NORMAL:
      return "NORMAL";
    case isp2_status::ISP2_O2:
      return "O2 1/10%";
    case isp2_status::ISP2_CALIBRATING:
      return "CALIBRATING";
    case isp2_status::ISP2_NEED_CALIBRATION:
      return "NEED CALIB";
    case isp2_status::ISP2_WARMING:
      return "WARMING UP";
    case isp2_status::ISP2_HEATER_CALIBRATING:
      return "HTR CALIB";
    case isp2_status::ISP2_LAMBDA_ERROR_CODE:
      return "ERROR";
    case isp2_status::ISP2_RESERVED:
      return "RESERVED";
    default:
      return "UNKNOWN"; //TODO return string value of int
  }
}

int setupTTY(int fd, int speed)
{
  struct termios tty;

  //tcgetattr(fd, &oldtio); // Save current port settings
  if (tcgetattr(fd, &tty) < 0) {
    fprintf(stderr, "Error (%d) from tcgetattr: %s\n", errno, strerror(errno));
    return -1;
  }

  //bzero(&tty, sizeof(tty));

  cfsetospeed(&tty, (speed_t)speed);
  cfsetispeed(&tty, (speed_t)speed);

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

int main(int argc, char *argv[]) {
  struct termios oldtio, newtio;
  int fd = 0;
  int flags = 0;
  int opt = 0;
  char *device;
  isp2_t data;

  while ((opt = getopt(argc, argv, "vds")) != -1) {
    switch (opt) {
      case 'v':
        flags |= ISP2_FLAGS_VERBOSE;
        break;
      case 'd':
        flags |= ISP2_FLAGS_DELAY;
        break;
      case 's':
        flags |= ISP2_FLAGS_SERIAL;
        break;
      default:
        fprintf(stderr, "Usage: %s [-v] [-d] [-s] fd\n", argv[0]);
        return -1;
    }
  }

  printf("flags=%d; optind=%d\n", flags, optind);

  if (optind >= argc) {
    fprintf(stderr, "Expected file descriptor after options\n");
    return -1;
  }

  device = argv[optind];

  int openFlags = O_RDONLY;
  if (flags & ISP2_FLAGS_SERIAL) {
    openFlags |= O_NOCTTY;
  }

  fd = open(device, openFlags);
  if (fd == -1) {
    fprintf(stderr, "Open error (%d): %s\n", errno, strerror(errno));
    return -1;
  }

  if (flags & ISP2_FLAGS_SERIAL) {
    setupTTY(fd, BAUDRATE);
  }

  initscr();
  noecho();
  mvprintw(0, 0, "Reading: %s", device);
  refresh();
  
  while (ISP2::isp2_read(fd, data) != -1) {
    for (int i = 0; i < data.chainCount; ++i) {
      int screenOffset = i * 5 + 2;
      mvprintw(screenOffset, 0, "> Sensor %d", i);
      //mvprintw(screenOffset + 1, 2, "Packet length: %d\n", data.packet_length);
      mvprintw(screenOffset + 1, 0, "  Status: %-12s", statusMessage(data.chain[i].status));
      mvprintw(screenOffset + 2, 0, "  Lambda: %0.2f", data.chain[i].lambda / 1000.0);
      //mvprintw(screenOffset + 2, 13, "(Stoich: %2.2f)", data.afr_multiplier / 10.0);
    }

    // if (data.is_sensor_data) {
    //   mvprintw(0, 0, "Data: [Sensor]");
    // } else {
    //   mvprintw(0, 0, "Data: [      ]");
    // }

    if (data.is_recording) {
      mvprintw(1, 0, "RECORDING: [ON ]");
    } else {
      mvprintw(1, 0, "RECORDING: [OFF]");
    }
    
    if (data.sender_can_log) {
      mvprintw(1, 17, "LOGGING: [COULD]"); //TODO what?
    } else {
      mvprintw(1, 17, "LOGGING: [NOPE?]"); //TODO huh?
    }

    refresh();

    //1 packet (16-bit word) every 82 ms (so 1 byte every 41 ms)
    //Or did the docs mean all packets every 82ms?
    //This is not really accurate but close enough to real time.
    if (flags & ISP2_FLAGS_DELAY) {
      usleep(41000); //41ms = 41000 microseconds.
    }
  }

  mvprintw(0, 77, "EOF");
  getch();
  endwin();

  if (flags & ISP2_FLAGS_SERIAL) {
    //Restore old tty settings
    tcsetattr(fd, TCSANOW, &oldtio);
  }
  
  return 0;
}
