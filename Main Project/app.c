#include "stm32g030xx.h"
#include "app.h"
#include "crc32.h"
#include "gpio.h"
#include "sw_timebase.h"
#include "debug.h"
//#include "timebase.h"
#include "flash.h"



#define APP_START_ADDR   0x08000000
#define APP_SIZE_BYTES   (64 * 1024)   // Example: 64KB firmware
#define	delay_interval		1000
#define DATA_FLASH_START  0x08007000
#define DATA_FLASH_END    0x08007FFF     // 4 KB block
#define DATA_FLASH_PAGE   0x400          // 1 KB per page


//uint8_t t1, t2;

   


void App_Setup(void){
	
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	//GPIO_Init(GPIOA,4,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	GPIO_Init(GPIOA,6,GPIO_MODE_OUTPUT,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	GPIO_SetPin(GPIOA,6);
	//GPIO_SetPin(GPIOA,6);
	Debug_Init(38400);
	sw_timebase_Init(1000);
	sw_timebase_counter_ss_set_securely(0,2000);
	//sw_timebase_counter_ss_set_securely(2,5000);
	//sw_timebase_counter_ss_set_securely(1,500);
	//sw_timebase_counter_ss_set_period_value_securely(0,200);
	
}


void App_Main_Loop(void){
	
	
	sw_timebase_main_loop_executable();
	/*if(sw_timebase_counter_ss_period_value_expired_event(0)){
		GPIO_TogglePin(GPIOA, 4);
		sw_timebase_counter_ss_set_period_value_securely(0,200);
	}
	if (sw_timebase_counter_ss_continous_expired_event(0)){
		GPIO_TogglePin(GPIOA, 6);
		sw_timebase_counter_ss_set_period_value_securely(0,200);
	}
	
	if (sw_timebase_counter_ss_continous_expired_event(1)){
		Debug_Tx_Parameter_NL("UpTime",sw_timebase_get_shadow_sub_seconds_uptime_securely());
	}
	if (sw_timebase_counter_ss_continous_expired_event(2)){
		//Debug_Tx_Parameter_NL("UpTime",sw_timebase_get_shadow_sub_seconds_uptime_securely());
		//flash_erase_full_pages();
	}*/
	//sw_timebase_counter_delay_seconds(2);
	//GPIO_TogglePin(GPIOA, 4);
	if (sw_timebase_counter_ss_continous_expired_event(0)){
		GPIO_TogglePin(GPIOA, 6);
	}
	
	
	
}







