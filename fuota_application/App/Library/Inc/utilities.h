/*
 * utilities.h
 *
 *  Created on: Oct 29, 2024
 *      Author: Massimiliano Cristarella
 */

#ifndef LIBRARY_INC_UTILITIES_H_
#define LIBRARY_INC_UTILITIES_H_

#define NAMEOF(x) (#x)

#include <stdint.h>

void print_bytes(uint8_t* ptr, uint8_t size, uint8_t group);

static inline uint32_t ReadLittleEndian(const void* bytes)
{
  // little endian bytes = {0x78, 0x56, 0x34, 0x12}
  // => 0x12 00 00 00 | 0x34 00 00 | 0x56 00 | 0x78 = 0x12345678
  return ((uint32_t)((uint8_t*)bytes)[3] << 24) |
         ((uint32_t)((uint8_t*)bytes)[2] << 16) |
         ((uint32_t)((uint8_t*)bytes)[1] << 8) |
          (uint32_t)((uint8_t*)bytes)[0];
}
static inline uint32_t ReadBigEndian(const void* bytes)
{
  // big endian bytes = {0x12, 0x34, 0x56, 0x78}
  // => 0x12 00 00 00 | 0x34 00 00 | 0x56 00 | 0x78 = 0x12345678
  return ((uint32_t)((uint8_t*)bytes)[0] << 24) |
         ((uint32_t)((uint8_t*)bytes)[1] << 16) |
         ((uint32_t)((uint8_t*)bytes)[2] << 8) |
          (uint32_t)((uint8_t*)bytes)[3];
}

#endif /* LIBRARY_INC_UTILITIES_H_ */
