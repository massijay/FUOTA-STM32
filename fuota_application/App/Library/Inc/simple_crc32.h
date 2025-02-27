/*
 * simple_crc32.h
 *
 *  Created on: Nov 11, 2024
 *      Author: mcris
 */

#ifndef LIBRARY_INC_SIMPLE_CRC32_H_
#define LIBRARY_INC_SIMPLE_CRC32_H_

#include <stdint.h>

uint32_t SimpleCrc32(uint8_t* data, uint32_t size);
void SimpleCrc32_generate_nibble_table(uint32_t* table);

#endif /* LIBRARY_INC_SIMPLE_CRC32_H_ */
