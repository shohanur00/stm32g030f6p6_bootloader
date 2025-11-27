#ifndef FLASH_H
#define FLASH_H

#include "stm32g030xx.h" // or your device header

#define FLASH_KEY1 								0x45670123
#define FLASH_KEY2 								0xCDEF89AB
#define FLASH_PAGE_SIZE  					0x800   // 2KB
#define FLASH_BASE_ADDR  					0x08000000
#define FLASH_TOTAL_PAGES			 		0x0F



/* RAM function macro */
#if defined(__GNUC__)
  #define FLASH_RAMFUNC __attribute__((section(".ramfunc"), noinline))
#else
  #define FLASH_RAMFUNC
#endif



void flash_erase_page(uint32_t page_address);
uint32_t flash_get_page_address(uint32_t page_number);
void flash_write_word(uint32_t address, uint32_t data);
void flash_erase_page_by_number(uint32_t page_number);
void flash_erase_full_pages(void);

#endif
