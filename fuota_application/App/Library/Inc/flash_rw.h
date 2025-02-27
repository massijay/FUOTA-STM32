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
#define WORDS(bytes) (bytes >> 2)
#define QWORDS(bytes) (bytes >> 4)
#define BYTES_FROM_WORDS(words) (words << 2)
#define BYTES_FROM_QWORDS(qwords) (qwords << 4)
#define WORDS_FROM_QWORDS(qwords) (qwords << 2)
#define QWORDS_FROM_WORDS(qwords) (qwords >> 2)
#define FLASH_RW_MEMORY_ARRAY_QUADWORDS 64L
#define FLASH_RW_MEMORY_ARRAY_LENGTH (FLASH_RW_MEMORY_ARRAY_QUADWORDS * QUADWORD_LENGTH) /* 1024 Byte */
#define MASK_WORD 0b11UL
#define MASK_QUADWORD 0b1111UL

typedef enum
{
  FLASH_RW_OK    = 0x00,
  FLASH_RW_ERROR = 0x01,
  FLASH_RW_ERROR_TIMEOUT,
  FLASH_RW_ERROR_BUSY,
  FLASH_RW_ERROR_SRC_ADDR_NOT_ALIGNED,
  FLASH_RW_ERROR_DST_ADDR_NOT_ALIGNED
} Flash_RW_Result;

typedef struct
{
  word start_address;
} Flash_RW_State;

Flash_RW_Result Flash_RW_erase(Flash_RW_State* state);
Flash_RW_Result Flash_RW_erase_pages(Flash_RW_State* state, word pages_n);
Flash_RW_Result Flash_RW_write_qword(Flash_RW_State* state, word offset_qwords, const word* data, word qwords_count);
Flash_RW_Result Flash_RW_write(Flash_RW_State* state, word index, const void* data, word size, byte purge);
void Flash_RW_read(Flash_RW_State* state, word offset, void* dest, word size);
byte Flash_RW_is_empty(Flash_RW_State* state, word offset, word size);


#endif /* INC_FLASH_RW_H_ */
