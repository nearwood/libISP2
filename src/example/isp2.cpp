#include "../isp2.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>

#include <ncurses.h>

#define ISP2_FLAGS_VERBOSE 0x01
#define ISP2_FLAGS_DELAY 0x02

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

int main(int argc, char *argv[]) {
  int fd = 0;
  int flags = 0;
  int opt = 0;
  isp2_t data;

  while ((opt = getopt(argc, argv, "vd")) != -1) {
    switch (opt) {
      case 'v':
        flags |= ISP2_FLAGS_VERBOSE;
        break;
      case 'd':
        flags |= ISP2_FLAGS_DELAY;
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-v] [-d] fd\n", argv[0]);
        return -1;
    }
  }

  printf("flags=%d; optind=%d\n", flags, optind);

  if (optind >= argc) {
    fprintf(stderr, "Expected file descriptor after options\n");
    return -1;
  }

  fd = open(argv[optind], O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Open error: (%d) %s\n", errno, strerror(errno));
    return -1;
  }

  initscr();
  noecho();
  mvprintw(0, 0, "Reading: %s", argv[optind]);
  refresh();
  
  while (ISP2::isp2_read(fd, data) != -1) {
    //mvprintw(1, 2, "Packet length: %d\n", data.packet_length);
    mvprintw(2, 0, "Status: %-12s", statusMessage(data.status));
    mvprintw(3, 0, "Lambda: %0.2f", data.lambda / 1000.0);
    mvprintw(3, 13, "(Stoich: %2.2f)", data.afr_multiplier / 10.0);

    if (data.is_sensor_data) {
      mvprintw(1, 0, "Data: [Sensor]");
    } else {
      mvprintw(1, 0, "Data: [      ]");
    }

    if (data.is_recording) {
      mvprintw(1, 16, "RECORDING: [ON ]");
    } else {
      mvprintw(1, 16, "RECORDING: [OFF]");
    }
    
    if (data.sender_can_log) {
      mvprintw(1, 34, "LOG: [COULD]"); //TODO what?
    } else {
      mvprintw(1, 34, "LOG: [NOPE ]"); //TODO what?
    }

    refresh();

    //1 packet (word) every 82 ms (so 1 byte every 41 ms)
    //41ms = 41000 microseconds.
    if (flags & ISP2_FLAGS_DELAY) {
      usleep(41000);
    }
  }

  mvprintw(0, 77, "EOF");
  getch();
  endwin();
  
  return 0;
}
