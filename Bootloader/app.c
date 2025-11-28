#include "stm32g030xx.h"
#include "app.h"
#include "crc32.h"
#include "gpio.h"
#include "sw_timebase.h"
#include "debug.h"
#include "program_flash.h"
#include "flash.h"
#include "string.h"
#include "stdint.h"


#define	delay_interval		1000



void App_Setup(void){
	

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIO_Init(GPIOA,4,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	GPIO_SetPin(GPIOA,4);
	
	Debug_Init(115200);
	sw_timebase_Init(1000);
	sw_timebase_counter_ss_set_securely(0,500);
	sw_timebase_counter_ss_set_securely(1,3000);
	sw_timebase_counter_ss_set_securely(2,20000);

	
}


void App_Main_Loop(void){
	
	
	sw_timebase_main_loop_executable();

	if (sw_timebase_counter_ss_continous_expired_event(0)){
		GPIO_TogglePin(GPIOA, 4);
	}
	
	if (sw_timebase_counter_ss_expired_event(2)){
		Program_Flash_Jump_To_App();
	}
	

	
	Program_Flash_Process_Debug_Data();
	
	
}







