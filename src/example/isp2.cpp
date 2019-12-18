#include "../isp2.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>

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


  printf("Opening file: %s\n", argv[optind]);

  fd = open(argv[optind], O_RDONLY);
  if (fd == -1) {
    printf("Open error: (%d) %s\n", errno, strerror(errno));
  }
  
  while (ISP2::isp2_read(fd,data) != -1) {
    printf("Packet length: %d\n", data.packet_length);
    printf("Lambda: %f\n", data.lambda / 1000.0);
    printf("Status: %d\n", data.status);
    printf("afr_m: %f\n", data.afr_multiplier / 10.0);

    if (data.is_recording) {
      printf("Recording\n");
    }

    if (data.is_sensor_data) {
      printf("Sensor Data\n");
    }
    
    if (data.sender_can_log) {
      printf("Can Log\n");
    }
  }
  
  return 0;
}
