#ifndef PROGRAM_FLASH
#define PROGRAM_FLASH

#include "stm32g030xx.h"
#include "debug.h"
#include "flash.h"


void Program_Flash_Jump_To_App(void);
void Program_Flash_Process_Debug_Data(void);
uint8_t Program_Flash_Calculate_Checksum(uint8_t *data);
void Program_Flash_Parse_Frame(char* frame, uint32_t* address, uint8_t* data,uint8_t* data_lenth, uint8_t* checksum);


#endif