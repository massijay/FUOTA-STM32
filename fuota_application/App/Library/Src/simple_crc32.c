/*
 * simple_crc32.c
 *
 *  Created on: Nov 11, 2024
 *      Author: mcris
 */

#include <simple_crc32.h>

#define POLYNOMIAL 0x04C11DB7

// CRC32 nibble lookup table
// Generated using 0x04C11DB7 polynomial
static const uint32_t table[16] =
{
  0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
  0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
};

uint32_t SimpleCrc32(uint8_t* data, uint32_t size)
{
  uint32_t crc = 0xFFFFFFFF;

  for (uint32_t i = 0; i < size; ++i)
  {
    crc = (crc << 4) ^ table[(crc >> 28) ^ (data[i] >> 4)];
    crc = (crc << 4) ^ table[(crc >> 28) ^ (data[i] & 0x0F)];
  }
  return crc;
}

void SimpleCrc32_generate_nibble_table(uint32_t* table)
{
  for (uint32_t i = 0; i < 16; ++i)
  {
    uint32_t crc = i << 28;
    for (uint8_t j = 0; j < 4; ++j)
    {
      if (crc & 0x80000000)
      {
        crc = (crc << 1) ^ POLYNOMIAL;
      }
      else
      {
        crc <<= 1;
      }
    }
    table[i] = crc;
  }
}
