#include "../isp2.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>

#include <ncurses.h>

#define ISP2_FLAGS_VERBOSE 0x01;

int main(int argc, char *argv[]) {
  int fd = 0;
  int flags = 0;
  int opt = 0;
  isp2_t data;

  while ((opt = getopt(argc, argv, "nt:")) != -1) {
    switch (opt) {
      case 'v':
        flags |= ISP2_FLAGS_VERBOSE;
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-v] fd\n", argv[0]);
        return -1;
    }
  }

  printf("flags=%d; optind=%d\n", flags, optind);

  if (optind >= argc) {
    fprintf(stderr, "Expected file descriptor argument after options\n");
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
    mvprintw(1, 0, "Status: %d", data.status);
    mvprintw(2, 0, "Lambda: %0.2f", data.lambda / 1000.0);
    mvprintw(2, 16, "AFR: %2.2f", data.afr_multiplier / 10.0);

    if (data.is_sensor_data) {
      mvprintw(1, 12, "Sensor Data\n");
    }

    if (data.is_recording) {
      mvprintw(1, 24, "Recording\n");
    }    
    
    if (data.sender_can_log) {
      mvprintw(1, 36, "Can Log\n");
    }

    refresh();

    //TODO behind simulate flag
    //1 packet (word) every 82 ms (so 1 byte every 41 ms)
    //41ms = 41000 microseconds.
    usleep(41000);
  }

  mvprintw(0, 77, "EOF");
  getch();
  endwin();
  
  return 0;
}
