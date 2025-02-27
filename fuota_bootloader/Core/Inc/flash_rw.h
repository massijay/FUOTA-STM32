/*
 * flash_rw.h
 *
 *  Created on: Oct 14, 2024
 *      Author: Massimiliano Cristarella
 */

#ifndef INC_FLASH_RW_H_
#define INC_FLASH_RW_H_

#include <stdint.h>
#include "stm32wbaxx_hal.h"

typedef uint8_t byte;
typedef uint32_t word;


#define WORD_LENGTH 4U /* Word length is 32 bit = 4 Byte */
#define QUADWORD_LENGTH (4U * 4) /* Minimum writable block in FLASH = QUADWORD = 128 bit = 16 Byte */
#define FLASH_RW_GET_PAGE_N(address) (((address) - FLASH_BASE_NS) / FLASH_PAGE_SIZE)
#define FLASH_RW_MEMORY_ARRAY_QUADWORDS 64L
#define FLASH_RW_MEMORY_ARRAY_LENGTH (FLASH_RW_MEMORY_ARRAY_QUADWORDS * QUADWORD_LENGTH) /* 1024 Byte */
#define MASK_WORD 0b11UL
#define MASK_QUADWORD 0b1111UL

typedef enum
{
  FLASH_RW_OK    = 0x00,
  FLASH_RW_ERROR = 0x01
} Flash_RW_Result;

typedef struct
{
  word start_address;
} Flash_RW_State;

Flash_RW_Result Flash_RW_erase(Flash_RW_State* state);
Flash_RW_Result Flash_RW_write(Flash_RW_State* state, word index, const void* data, word size, byte purge);
void Flash_RW_read(Flash_RW_State* state, word offset, void* dest, word size);
byte Flash_RW_is_empty(Flash_RW_State* state, word offset, word size);


#endif /* INC_FLASH_RW_H_ */
