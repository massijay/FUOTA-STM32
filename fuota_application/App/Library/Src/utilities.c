/*
 * utilities.c
 *
 *  Created on: Oct 29, 2024
 *      Author: Massimiliano Cristarella
 */

#include "utilities.h"

#include "log_module.h"

/**
  * @brief  Print (log) a byte array in hexadecimal format
  * @param  ptr: Pointer to the array
  * @param  size: Size of the array (in bytes)
  * @param  group: Group bytes and separate them by a space (e.g. 0xABCDEF01, group = 2 => 0xABCD EF01), 0 to not group
  * @retval Flash_RW_Result: Result enum
  */
void print_bytes(uint8_t* ptr, uint8_t size, uint8_t group)
{
  LOG_WARNING_APP("\n0x");
  for (uint8_t i = 0; i < size; ++i)
  {
    if (group != 0 && i > 0 && i % group == 0)
    {
      LOG_WARNING_APP(" ");
    }
    LOG_WARNING_APP("%02X", ptr[i]);
  }
  LOG_WARNING_APP("\n");
}
