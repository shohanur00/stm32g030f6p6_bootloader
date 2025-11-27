#include "stm32g030xx.h"
#include "app.h"
#include "crc32.h"
#include "gpio.h"
#include "sw_timebase.h"
#include "debug.h"
//#include "timebase.h"
#include "flash.h"



//#define APP_START_ADDR   0x08000000
//#define APP_SIZE_BYTES   (64 * 1024)   // Example: 64KB firmware
#define	delay_interval		1000

#define MAIN_APP_ADDRESS   0x08002000U
#define SRAM_START         0x20000000U
#define SRAM_SIZE_BYTES    (8U * 1024U)          /* STM32G030F6P6: 8 KB */
#define SRAM_END           (SRAM_START + SRAM_SIZE_BYTES - 1U)

#define APP_ADDR 0x08002000

void JumpToApp(void);



void JumpToApp(void)
{
	typedef void (*app_fn)(void);
	uint32_t* reset_vector_entry = (uint32_t*)(APP_ADDR+4U);
	uint32_t* reset_vector = (uint32_t *)(*reset_vector_entry);
	app_fn jump_fn = (app_fn)reset_vector;
	jump_fn();
	
}

void App_Setup(void){
	
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIO_Init(GPIOA,4,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	
	GPIO_SetPin(GPIOA,4);
	
	Debug_Init(38400);
	sw_timebase_Init(1000);
	sw_timebase_counter_ss_set_securely(0,500);
	sw_timebase_counter_ss_set_securely(2,5000);

	
}


void App_Main_Loop(void){
	
	
	sw_timebase_main_loop_executable();

	if (sw_timebase_counter_ss_continous_expired_event(0)){
		GPIO_TogglePin(GPIOA, 4);
	}
	
	if (sw_timebase_counter_ss_continous_expired_event(2)){

		JumpToApp();
	}
	
	
}







