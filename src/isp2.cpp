#include "isp2.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

extern int setupTTY(int fd);

int ISP2::isp2_open(char *device, int flags) {
  int fd = 0;
  int openFlags = O_RDONLY | O_NOCTTY;

  fd = open(device, openFlags);
  if (fd == -1) {
    fprintf(stderr, "Open error (%d): %s\n", errno, strerror(errno));
    return -1;
  }

#ifndef NO_SERIAL_SUPPORT
  if (flags & ISP2_FLAGS_SERIAL) {
    setupTTY(fd);
  }
#endif

  return fd;
}

/** Reads from a file descriptor and parses it
 *  to pass back a isp2_t struct containing 
 *  relevant information.
 *  Makes use of the other private funtions.
 */
int ISP2::isp2_read(int file, isp2_t& isp_data) {
  uint16_t	header;
  uint16_t	current_word;

  header = ISP2::get_header(file);
  if (header == 0) {
    return -1;
  }

  isp_data.packet_length = ISP2::get_packet_length(header);
  isp_data.sender_can_log = ISP2::get_can_log(header);
  isp_data.is_sensor_data = ISP2::get_is_sensor(header);
  isp_data.is_recording = ISP2::get_is_recording(header);
  int remaining = isp_data.packet_length;
  isp_data.chainCount = 0;
  int i = 0;
  
  // get next word
  while (remaining > 0) {
    current_word = ISP2::get_next_word(file);
    remaining--;

    switch (ISP2::get_word_type(current_word)) {
      case ISP2_LC2_HEADER_WORD:
        isp_data.chainCount += 1;
        i = isp_data.chainCount - 1;

        isp_data.chain[i].status = ISP2::get_status(current_word);

        //Use the first LM/LC AFR multiplier for all subsequent LC data words in packet
        if (i == 0) {
          isp_data.afr_multiplier = ISP2::get_afr_multiplier(current_word);
        }
        
        current_word = ISP2::get_next_word(file);
        remaining--;

        if (ISP2::get_word_type(current_word) == ISP2_DATA_WORD) {
          isp_data.chain[i].lambda = ISP2::get_lambda(current_word);
          if (isp_data.chain[i].status == ISP2_NORMAL) {
            isp_data.chain[i].lambda += 500; // Protocol states "offset by 0.5 Lambda"
          }
        }
        break;
      
      // TODO aux packet
      case ISP2_DATA_WORD:
        break;

      // TODO lm-1 packet
      case ISP2_UNK_WORD:
        break;
    }
  }

  return 0;
}

// Read from 'file' until bytes match header
uint16_t ISP2::get_header(int file) {
  uint8_t header[2] = {0};
  
  int result;
  do {
    header[0] = header[1];
    result = read(file, &header[1], 1);
    if (result == -1) {
      //perror("ISP2 read error");
      return 0;
    } else if (result == 0) {
      //perror("ISP2 read nothing");
      return 0;
    }
  }
  while (ISP2::get_word_type(*(uint16_t*)header) != ISP2_HEADER_WORD);
  
  return *(uint16_t*)header;
}

bool ISP2::get_is_recording(uint16_t header) {
  if (header & ISP2_RECORDING_BIT) {
    return true;
  } else {
    return false;
  }
}

bool ISP2::get_is_sensor(uint16_t header) {
  if (header & ISP2_IS_SENSOR_BIT) {
    return true;
  } else {
    return false;
  }
}

bool ISP2::get_can_log(uint16_t header) {
  if (header & ISP2_CAN_LOG_BIT) {
    return true;
  } else {
    return false;
  }
}

uint8_t ISP2::get_packet_length(uint16_t header) {
  uint8_t* header_ptr = (uint8_t*)&header;
  
  // first byte
  header_ptr[0] <<= 7; // move low bit to high bit
  header_ptr[0] |= 0x7F; // make all lower bits high

  // second byte
  header_ptr[1] &= header_ptr[0]; // mask second byte with first to put length together

  return header_ptr[1];
}

uint16_t ISP2::get_next_word(int file) {
  uint8_t	buf[2] = {0};
  int result;
  // Read from 'file' until bytes match known word type
  int byte_count = 0;
  do {
    buf[0] = buf[1];
    result = read(file, &buf[1], 1);
    if (result == -1) {
      // Error in read
      //perror("ISP2 read error");
      return 0;
    }
    else if (result == 0) {
      // Didn't read anything
      //perror("ISP2 read nothing");
      return 0;
    }
    byte_count++;
  }
  while (byte_count < 2 || ISP2::get_word_type(*(uint16_t*)buf) == ISP2_UNK_WORD);
  
  return *(uint16_t*)buf;
}

word_type ISP2::get_word_type(uint16_t word) {
  uint16_t temp = word;

  temp &= ISP2_LC2_HEADER_BITS;
  if (!(temp ^ ISP2_LC2_HEADER_SIGNATURE)) {
    return ISP2_LC2_HEADER_WORD;
  }

  temp = word;
  temp &= ISP2_DATA_BITS;

  if (!(temp ^ ISP2_DATA_SIGNATURE)) {
    return ISP2_DATA_WORD;
  }

  temp = word;
  temp &= ISP2_HEADER_BITS;
  
  if (!(temp ^ ISP2_HEADER_SIGNATURE)) {
    return ISP2_HEADER_WORD;
  }

  return ISP2_UNK_WORD;
}

isp2_status ISP2::get_status(uint16_t word) {
  uint8_t byte1 = *(uint8_t*)&word;

  byte1 >>= 2;
  byte1 &= 0x07;
  return (isp2_status)byte1;
}

uint8_t ISP2::get_afr_multiplier(uint16_t word) {
  uint8_t* word_ptr = (uint8_t*)&word;
  
  // first byte
  word_ptr[0] <<= 7; // move low bit to high bit

  // second byte
  word_ptr[1] |= word_ptr[0]; // mask second byte with first to put length together

  return word_ptr[1];
}

uint16_t ISP2::get_lambda(uint16_t word) {
  uint8_t* word_ptr = (uint8_t*)&word;
  uint8_t low_byte_high_bit = word_ptr[0] << 7; // save low bit as high bit
  word_ptr[1] |= low_byte_high_bit; // put high bit back in the right byte
  word_ptr[0] >>= 1; // shift high byte over
  word_ptr[0] &= 0x1F; // mask high byte just in case extra bits got in there
  
  #ifndef WORDS_BIGENDIAN
  //TODO The spec specifies Bigendian, not sure why this is here.
  // swap bytes, standard XOR swap algorithm
  word_ptr[0] = word_ptr[0] ^ word_ptr[1];
  word_ptr[1] = word_ptr[1] ^ word_ptr[0];
  word_ptr[0] = word_ptr[0] ^ word_ptr[1];
  #endif /* WORDS_BIGENDIAN */
  
  return word;
}
