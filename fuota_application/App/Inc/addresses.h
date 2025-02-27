/*
 * addresses.h
 *
 *  Created on: Nov 19, 2024
 *      Author: Massimiliano Cristarella
 */

#ifndef INC_ADDRESSES_H_
#define INC_ADDRESSES_H_


#define APP_SLOTS_N 2
#define APP_SLOT0_ADDRESS 0x0800A000
#define APP_SLOT1_ADDRESS 0x08082000
#define APP_SLOT_PAGES ((APP_SLOT1_ADDRESS - APP_SLOT0_ADDRESS) / FLASH_PAGE_SIZE)

#ifndef SLOT
#define SLOT -1
#endif

#if SLOT == 0
#define BOOT_ADDRESS APP_SLOT0_ADDRESS
#elif SLOT == 1
#define BOOT_ADDRESS APP_SLOT1_ADDRESS
#else
#define BOOT_ADDRESS 0x08FFFFFF
#error "Please define either SLOT=0 or SLOT=1"
#endif

#ifndef LINKER_SCRIPT
/* Put non-preprocessor C code here */

#endif /* LINKER_SCRIPT */

#endif /* INC_ADDRESSES_H_ */
