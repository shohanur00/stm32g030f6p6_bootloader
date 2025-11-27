#include "flash.h"


static void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static void flash_clear_flags(void) {
    FLASH->SR |= (FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR |
                  FLASH_SR_PGAERR | FLASH_SR_SIZERR | FLASH_SR_PGSERR |
                  FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_EOP);
}


static void flash_wait_busy(void)
{
    while (FLASH->SR & FLASH_SR_BSY1);
}

void flash_erase_page(uint32_t page_address)
{
    flash_wait_busy();
	
		flash_clear_flags();

    flash_unlock();

    FLASH->CR |= FLASH_CR_PER;          // enable page erase
    FLASH->CR &= ~FLASH_CR_PNB_Msk;     // clear page number bits
    FLASH->CR |= ((page_address - FLASH_BASE_ADDR) / FLASH_PAGE_SIZE) << FLASH_CR_PNB_Pos;
    FLASH->CR |= FLASH_CR_STRT;         // start erase

		flash_wait_busy();

    FLASH->CR &= ~FLASH_CR_PER;         // disable page erase
    FLASH->SR = FLASH_SR_EOP;          // clear EOP flag



    flash_lock();
}

//FLASH_RAMFUNC
void flash_erase_page_by_number(uint32_t page_number)
{
    flash_wait_busy();
	
		flash_clear_flags();

    flash_unlock();

    FLASH->CR |= FLASH_CR_PER;          // enable page erase
    FLASH->CR &= ~FLASH_CR_PNB_Msk;     // clear page number bits
    FLASH->CR |=  page_number << FLASH_CR_PNB_Pos;
    FLASH->CR |= FLASH_CR_STRT;         // start erase

    flash_wait_busy();

		FLASH->SR = FLASH_SR_EOP;         	// clear EOP flag
    FLASH->CR &= ~FLASH_CR_PER;         // disable page erase
    



    flash_lock();
}


/* Write one 32-bit word (runs from RAM) */
//FLASH_RAMFUNC
void flash_write_word(uint32_t addr, uint32_t data)
{
    flash_wait_busy();
    flash_clear_flags();
    flash_unlock();

    FLASH->CR |= FLASH_CR_PG;
    *(volatile uint32_t *)addr = data;
    flash_wait_busy();

    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
}


//FLASH_RAMFUNC
void flash_erase_full_pages(void){
		for(uint8_t i = 0; i <= FLASH_TOTAL_PAGES ; i++){
			flash_erase_page_by_number(i);
		}
	
}


uint32_t flash_get_page_address(uint32_t page_number)
{
    return FLASH_BASE_ADDR + (page_number * FLASH_PAGE_SIZE);
}
