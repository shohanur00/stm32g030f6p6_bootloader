#include "sw_timebase.h"
#include "gpio.h"



typedef union {
  struct {
    uint8_t PeriodFlag : 1;
    uint8_t Value      : 4;
    uint8_t Reserved   : 3;
  };
  uint8_t StatusByte;
} volatile sw_timebase_status_t;



typedef	struct sw_timebase_config_t{
	
	volatile uint32_t UpdateRate;
	
}sw_timebase_config_t;


typedef struct sw_timebase_time_t{
	
	volatile	uint16_t	sub_seconds;															//This will change at main loop
	volatile 	uint32_t	seconds;																	//This will change at main loop
	volatile	uint16_t	shadow_subseconds;												//This will change during interrupt happen
	volatile	uint32_t	shadow_seconds; 													//This will change during interrupt happen
	volatile	uint64_t	shadow_sub_seconds_uptime;								//Give sub_seconds value. This will only return uptime at subseconds
	volatile 	uint8_t		VariablesSync;
	
}sw_timebase_time_t;


typedef struct sw_timebase_counter_t{
	
	sw_timebase_status_t status;
	uint32_t	end_value;
	uint32_t	value;
	uint32_t	period_value;
	uint32_t	reload_value;
	uint32_t	target;
	uint32_t	temporary;
	
}sw_timebase_counter_t;

typedef struct sw_timebase_counter_ss_t{
	
	sw_timebase_status_t status;
	uint32_t	end_value;
	uint32_t	end_value_sub_sec;
	uint32_t	value;
	uint32_t	period_value;
	uint32_t	reload_value;
	uint32_t	target;
	uint32_t	temporary;
	
}sw_timebase_counter_ss_t;



typedef struct sw_timebase_t{
	
	sw_timebase_config_t sw_config;
	sw_timebase_time_t	sw_time;
	uint8_t	updateReq;
	
	sw_timebase_counter_t sw_counter[TIMEBASE_COUNTER];
	sw_timebase_counter_ss_t sw_counter_ss[TIMEBASE_COUNTER_SS];
	
}sw_timebase_t;


enum{
  COUNTER_UPDATE_REQ      	=  0x01,
  UPCOUNTER_SS_UPDATE_REQ   =  0x02,

};


enum{
	
	TIMER_VARIABLE_SYNC_TRUE				=		0x01,
	TIMER_VARIABLE_SYNC_FALSE				=		0x00,
	
};



enum{
  COUNTER_STATE_RESET       = 0,
  COUNTER_STATE_START       = 1,
  COUNTER_STATE_STARTED     = 1,
  COUNTER_STATE_STOP        = 2,
  COUNTER_STATE_STOPPED     = 2,
  COUNTER_STATE_EXPIRED     = 4
};



enum{
  FLAG_STATE_RESET          = 0,
  FLAG_STATE_SET            = 1,
};


enum{
  TIMEBASE_FALSE            = 0,
  TIMEBASE_TRUE             = 1,
};


sw_timebase_t		sw_timebase_type;
sw_timebase_t*	sw_timebase;


void sw_timebase_struct_init(void){
	
	sw_timebase = &sw_timebase_type;
	sw_timebase->sw_config.UpdateRate = 1;
	sw_timebase->updateReq = 0;
	sw_timebase->sw_time.seconds = 0;
	sw_timebase->sw_time.shadow_seconds	= 0;
	sw_timebase->sw_time.shadow_subseconds	=	0;
	sw_timebase->sw_time.shadow_sub_seconds_uptime = 0;
	sw_timebase->sw_time.sub_seconds = 0;
	sw_timebase->sw_time.VariablesSync = TIMER_VARIABLE_SYNC_FALSE;
	
	for( uint8_t i = 0; i < TIMEBASE_COUNTER; i++){
		
		sw_timebase->sw_counter[i].status.StatusByte = 0;
		sw_timebase->sw_counter[i].end_value = 0;
		sw_timebase->sw_counter[i].value = 0;
		sw_timebase->sw_counter[i].period_value = 0;
		sw_timebase->sw_counter[i].reload_value = 0;
		sw_timebase->sw_counter[i].target = 0;
		sw_timebase->sw_counter[i].temporary = 0;
		
	}
	
		for( uint8_t i = 0; i < TIMEBASE_COUNTER_SS; i++){
		
		sw_timebase->sw_counter_ss[i].status.StatusByte = 0;
		sw_timebase->sw_counter_ss[i].end_value = 0;
		sw_timebase->sw_counter_ss[i].value = 0;
		sw_timebase->sw_counter_ss[i].period_value = 0;
		sw_timebase->sw_counter_ss[i].reload_value = 0;
		sw_timebase->sw_counter_ss[i].target = 0;
		sw_timebase->sw_counter_ss[i].temporary = 0;
		
	}
	
}


void sw_timebase_enable(uint32_t UpdateHz){
	
		RCC->APBENR2|=RCC_APBENR2_TIM17EN;
		TIM17->CR1|=TIM_CR1_ARPE;
		TIM17->PSC=15;									//16MHz is used
		TIM17->ARR=(1000000/UpdateHz) - 1;
		TIM17->DIER|=TIM_DIER_UIE;
		NVIC_SetPriority(TIM17_IRQn, 0);
		NVIC_EnableIRQ(TIM17_IRQn);
		TIM17->CR1|=TIM_CR1_CEN;
		sw_timebase->sw_config.UpdateRate= UpdateHz;
}


void sw_timebase_Init(uint32_t UpdateHz){
	
	sw_timebase_struct_init();
	sw_timebase_enable(UpdateHz);
	
}

void sw_timebase_disable(uint8_t timer_num){
	
	
}



void sw_hardware_timer_disable(void)
{
    TIM17->CR1 &= ~TIM_CR1_CEN;    // Disable counter
}

void sw_hardware_timer_reset(void)
{
    TIM17->CNT = 0;                // Reset counter value
    TIM17->SR &= ~TIM_SR_UIF;      // Clear update interrupt flag
}

void sw_hardware_timer_interrupt_enable(void)
{
    TIM17->DIER |= TIM_DIER_UIE;   // Enable update interrupt
    NVIC_EnableIRQ(TIM17_IRQn);    // Enable interrupt in NVIC
}

void sw_hardware_timer_interrupt_disable(void)
{
    TIM17->DIER &= ~TIM_DIER_UIE;  // Disable update interrupt
    NVIC_DisableIRQ(TIM17_IRQn);   // Disable NVIC interrupt
}





/********************************Atomic Functions Start*******************************/


void sw_timebase_atomic_operation_start(void){
  __disable_irq();
}


void sw_timebase_atomic_operation_end(void){
  __enable_irq();
}


/***********************************Timer ISR Start**********************************/

void TIM17_IRQHandler(void){
	
	if (TIM17->SR & TIM_SR_UIF) {
        TIM17->SR &= ~TIM_SR_UIF;   // Clear flag immediately
    }
	
  sw_timebase_ISR_executables();
	
}


void sw_timebase_ISR_executables(void){
	
	sw_timebase->sw_time.shadow_subseconds++;
	sw_timebase->sw_time.shadow_sub_seconds_uptime++;
	
	if(sw_timebase->sw_time.shadow_subseconds >= sw_timebase->sw_config.UpdateRate){
		
		sw_timebase->sw_time.shadow_subseconds = 0;
		sw_timebase->sw_time.shadow_seconds++;
		//GPIO_TogglePin(GPIOA, 4);
	}
	
	sw_timebase->updateReq	|= COUNTER_UPDATE_REQ;
	
}


/*****************************Base Timer Functions Start*****************************/

uint16_t sw_timebase_get_sub_seconds(void) {
    return sw_timebase->sw_time.sub_seconds;
}

uint16_t sw_timebase_get_shadow_sub_seconds(void) {
    return sw_timebase->sw_time.shadow_subseconds;
}

uint32_t sw_timebase_get_seconds(void) {
    return sw_timebase->sw_time.seconds;
}

uint32_t sw_timebase_get_shadow_seconds(void) {
    return sw_timebase->sw_time.shadow_seconds;
}

uint64_t sw_timebase_get_shadow_sub_seconds_uptime(void) {
    return sw_timebase->sw_time.shadow_sub_seconds_uptime;
}



void sw_timebase_set_sub_seconds(uint16_t value){
		sw_timebase->sw_time.sub_seconds = value;
	
}


void sw_timebase_set_shadow_sub_seconds(uint16_t value){
	sw_timebase_atomic_operation_start();
	sw_timebase->sw_time.shadow_subseconds = value;
	sw_timebase_atomic_operation_end();
	
}

void sw_timebase_set_seconds(uint32_t value){
		sw_timebase->sw_time.seconds = value;
	
}

void sw_timebase_set_shadow_seconds(uint32_t value){
	sw_timebase_atomic_operation_start();
	sw_timebase->sw_time.shadow_seconds = value;
	sw_timebase_atomic_operation_end();
	
}

void sw_timebase_set_shadow_sub_seconds_uptime(uint64_t value){
	sw_timebase_atomic_operation_start();
	sw_timebase->sw_time.shadow_sub_seconds_uptime = value;
	sw_timebase_atomic_operation_end();
	
}



uint16_t sw_timebase_get_shadow_sub_seconds_securely(void) {
    uint16_t tmp0, tmp1;
    while (1) {
        tmp0 = sw_timebase->sw_time.shadow_subseconds;
        tmp1 = sw_timebase->sw_time.shadow_subseconds;
        if (tmp0 == tmp1) break;
    }
    return tmp0;
}


uint32_t sw_timebase_get_shadow_seconds_securely(void) {
    uint32_t tmp0, tmp1;
    while (1) {
        tmp0 = sw_timebase->sw_time.shadow_seconds;
        tmp1 = sw_timebase->sw_time.shadow_seconds;
        if (tmp0 == tmp1) break;
    }
    return tmp0;
}



uint64_t sw_timebase_get_shadow_sub_seconds_uptime_securely(void) {
    uint64_t tmp0, tmp1;
    while (1) {
        tmp0 = sw_timebase->sw_time.shadow_sub_seconds_uptime;
        tmp1 = sw_timebase->sw_time.shadow_sub_seconds_uptime;
        if (tmp0 == tmp1) break;
    }
    return tmp0;
}


/***********************************Counter part for Seconds**********************************/

// --- STATUS FUNCTIONS ---
uint8_t sw_timebase_counter_get_status(uint8_t index) {
    return sw_timebase->sw_counter[index].status.Value;
}

void sw_timebase_counter_set_status(uint8_t index, uint8_t value) {
    sw_timebase->sw_counter[index].status.Value = value; // 4-bit mask
}

uint8_t sw_timebase_counter_get_period_status_flag(uint8_t index) {
    return sw_timebase->sw_counter[index].status.PeriodFlag;
}

void sw_timebase_counter_set_period_status_flag(uint8_t index, uint8_t value) {
    sw_timebase->sw_counter[index].status.PeriodFlag = (value != 0) ? 1 : 0;
}

void sw_timebase_counter_reset_period_status_flag(uint8_t index) {
    sw_timebase->sw_counter[index].status.PeriodFlag = 0;
}

// --- VALUE FUNCTIONS ---
uint32_t sw_timebase_counter_get_current_value(uint8_t index) {
    return sw_timebase->sw_counter[index].value;
}

void sw_timebase_counter_set_current_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].value = value;
}

uint32_t sw_timebase_counter_get_end_value(uint8_t index) {
    return sw_timebase->sw_counter[index].end_value;
}

void sw_timebase_counter_set_end_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].end_value = value;
}

uint32_t sw_timebase_counter_get_period_value(uint8_t index) {
    return sw_timebase->sw_counter[index].period_value;
}

void sw_timebase_counter_set_period_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].period_value = value;
}

uint32_t sw_timebase_counter_get_reload_value(uint8_t index) {
    return sw_timebase->sw_counter[index].reload_value;
}

void sw_timebase_counter_set_reload_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].reload_value = value;
}

uint32_t sw_timebase_counter_get_target_value(uint8_t index) {
    return sw_timebase->sw_counter[index].target;
}

void sw_timebase_counter_set_target_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].target = value;
}

uint32_t sw_timebase_counter_get_temporary_value(uint8_t index) {
    return sw_timebase->sw_counter[index].temporary;
}

void sw_timebase_counter_set_temporary_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter[index].temporary = value;
}


void sw_timebase_counter_reset(uint8_t index){
		
		sw_timebase_counter_set_status(index,COUNTER_STATE_RESET);
		sw_timebase_counter_set_temporary_value(index,0);
		sw_timebase_counter_set_target_value(index,0);
		sw_timebase_counter_set_reload_value(index,0);
		sw_timebase_counter_set_period_value(index,0);
		sw_timebase_counter_set_end_value(index,0);
		sw_timebase_counter_set_current_value(index,0);
		sw_timebase_counter_reset_period_status_flag(index);
	
	
}

void sw_timebase_counter_clear_flag(uint8_t	index){
		sw_timebase_counter_reset(index);
}


void sw_timebase_counter_start(uint8_t	index){
		if(sw_timebase_counter_get_status(index) != COUNTER_STATE_STARTED){
				sw_timebase_counter_set_status(index,COUNTER_STATE_START);
		}
	
}

void sw_timebase_counter_stop(uint8_t	index){
		if(sw_timebase_counter_get_status(index) != COUNTER_STATE_STOPPED){
				sw_timebase_counter_set_status(index,COUNTER_STATE_STOP);
		}
	
}


void sw_timebase_counter_set_securely(uint8_t index,uint32_t value){
	
	if(sw_timebase_counter_get_status(index) == COUNTER_STATE_RESET){
		sw_timebase_counter_set_current_value(index,0);
		sw_timebase_counter_set_temporary_value(index,0);
		sw_timebase_counter_set_target_value(index,value);
		sw_timebase_counter_set_end_value(index,sw_timebase_get_seconds()+value);
		sw_timebase_counter_set_reload_value(index,value);
		sw_timebase_counter_set_status(index,COUNTER_STATE_START);
	}
	
}


void sw_timebase_counter_set_forcefully(uint8_t index,uint32_t value){
		sw_timebase_counter_reset(index);
		sw_timebase_counter_set_securely(index,value);
}


void sw_timebase_counter_update(uint8_t index){
	uint32_t curr_state_value = 0;
	if(sw_timebase_counter_get_status(index) == COUNTER_STATE_STARTED){
		curr_state_value = sw_timebase_get_seconds();
		sw_timebase_counter_set_temporary_value(index,sw_timebase_counter_get_end_value(index) - curr_state_value);
		sw_timebase_counter_set_current_value(index,sw_timebase_counter_get_target_value(index) - sw_timebase_counter_get_temporary_value(index));
		if(sw_timebase_counter_get_temporary_value(index) <= 0){
			
			sw_timebase_counter_set_current_value(index,sw_timebase_counter_get_target_value(index));
			sw_timebase_counter_set_temporary_value(index,0);
			sw_timebase_counter_set_end_value(index,0);
			sw_timebase_counter_set_reload_value(index,sw_timebase_counter_get_reload_value(index));
			sw_timebase_counter_set_status(index,COUNTER_STATE_EXPIRED);
		}
	}
	else if(sw_timebase_counter_get_status(index) == COUNTER_STATE_STOPPED){
			curr_state_value = sw_timebase_get_seconds();
			sw_timebase_counter_set_end_value(index,curr_state_value+sw_timebase_counter_get_temporary_value(index));
			if (sw_timebase_counter_get_current_value(index) !=(sw_timebase_counter_get_target_value(index) - sw_timebase_counter_get_temporary_value(index))) {
        
				sw_timebase_counter_set_current_value(index,sw_timebase_counter_get_target_value(index) - sw_timebase_counter_get_temporary_value(index));
			
			}
	}
}

uint8_t sw_timebase_counter_expired(uint8_t index){
	if(sw_timebase_counter_get_status(index) == COUNTER_STATE_EXPIRED){
			return	TIMEBASE_TRUE;
	}else {
			return	TIMEBASE_FALSE;
	}
	
}

uint8_t sw_timebase_counter_expired_event(uint8_t index){											//Onshot
	if(sw_timebase_counter_get_status(index) == COUNTER_STATE_EXPIRED){
			sw_timebase_counter_clear_flag(index);
			return	TIMEBASE_TRUE;
	}else {
			return	TIMEBASE_FALSE;
	}
	
}


uint8_t sw_timebase_counter_oneshot_expired_event(uint8_t index){
	
	return sw_timebase_counter_expired_event(index);
	
}

uint8_t sw_timebase_counter_continous_expired_event(uint8_t index){
	
	if(sw_timebase_counter_get_status(index) == COUNTER_STATE_EXPIRED){
			uint32_t reload_val = sw_timebase_counter_get_reload_value(index);
			sw_timebase_counter_clear_flag(index);
			sw_timebase_counter_set_securely(index,reload_val);
			return	TIMEBASE_TRUE;
	}else {
			return	TIMEBASE_FALSE;
	}
	
}



void sw_timebase_counter_set_period_value_securely(uint8_t index, uint32_t value){
    if(sw_timebase_counter_get_period_status_flag(index) == FLAG_STATE_RESET){

        uint32_t curr   = sw_timebase_counter_get_current_value(index);
        uint32_t target = sw_timebase_counter_get_target_value(index);

        // If counter already reached or passed target, skip setting new period
        if(curr >= target){
            return;
        }

        uint32_t next_period = curr + value;

        // Ensure period end doesn't exceed target
        if(next_period > target){
            next_period = target;
        }

        sw_timebase_counter_set_period_value(index, next_period);
        sw_timebase_counter_set_period_status_flag(index, FLAG_STATE_SET);
    }
}



uint32_t sw_timebase_counter_get_remaining_period_value(uint8_t index){
    uint32_t curr   = sw_timebase_counter_get_current_value(index);
    uint32_t target = sw_timebase_counter_get_target_value(index);
    uint32_t period = sw_timebase_counter_get_period_value(index);

    // If already beyond target, no remaining period
    if(curr >= target) return 0;

    int32_t temp = (int32_t)(period - curr);
    if(temp < 0) temp = 0;
    return (uint32_t)temp;
}


static inline uint8_t _period_expired_condition(uint8_t index) {
    return (sw_timebase_counter_get_remaining_period_value(index) == 0 &&
            sw_timebase_counter_get_period_status_flag(index) == FLAG_STATE_SET);
}


uint8_t sw_timebase_counter_period_value_expired(uint8_t index){
    return _period_expired_condition(index) ? TIMEBASE_TRUE : TIMEBASE_FALSE;
}

uint8_t sw_timebase_counter_period_value_expired_event(uint8_t index){
    if(_period_expired_condition(index)){
        sw_timebase_counter_set_period_status_flag(index, FLAG_STATE_RESET);
        return TIMEBASE_TRUE;
    }
    return TIMEBASE_FALSE;
}



void sw_timebase_counter_delay_seconds(uint32_t delay)
{
    uint32_t start_sec = sw_timebase_get_shadow_seconds_securely();
    uint32_t start_sub = sw_timebase_get_shadow_sub_seconds_securely();
    uint32_t target_ticks = delay * sw_timebase->sw_config.UpdateRate;

    uint32_t elapsed_ticks = 0;

    while (elapsed_ticks < target_ticks) {
        uint32_t curr_sec = sw_timebase_get_shadow_seconds_securely();
        uint32_t curr_sub = sw_timebase_get_shadow_sub_seconds_securely();

        if (curr_sub >= start_sub) {
            elapsed_ticks = (curr_sec - start_sec) * sw_timebase->sw_config.UpdateRate
                          + (curr_sub - start_sub);
        } else {
            // Handle sub-second overflow
            elapsed_ticks = (curr_sec - start_sec - 1) * sw_timebase->sw_config.UpdateRate
                          + (sw_timebase->sw_config.UpdateRate + curr_sub - start_sub);
        }
    }
}





void sw_timebase_counter_update_all(void){
  for(uint8_t i=0; i<TIMEBASE_COUNTER; i++){
    sw_timebase_counter_update(i);
  }
}


void sw_timebase_counter_reset_all(void){
  for(uint8_t i=0; i<TIMEBASE_COUNTER; i++){
    sw_timebase_counter_reset(i);
  }
}


/************************************************************************************/


/***********************************Counter part for Sub Seconds**********************************/

// --- STATUS FUNCTIONS ---
// --- STATUS FUNCTIONS ---
uint8_t sw_timebase_counter_ss_get_status(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].status.Value;
}

void sw_timebase_counter_ss_set_status(uint8_t index, uint8_t value) {
    sw_timebase->sw_counter_ss[index].status.Value = value; // 4-bit mask
}

uint8_t sw_timebase_counter_ss_get_period_status_flag(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].status.PeriodFlag;
}

void sw_timebase_counter_ss_set_period_status_flag(uint8_t index, uint8_t value) {
    sw_timebase->sw_counter_ss[index].status.PeriodFlag = (value != 0) ? 1 : 0;
}

void sw_timebase_counter_ss_reset_period_status_flag(uint8_t index) {
    sw_timebase->sw_counter_ss[index].status.PeriodFlag = 0;
}


// --- VALUE FUNCTIONS ---
uint32_t sw_timebase_counter_ss_get_current_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].value;
}

void sw_timebase_counter_ss_set_current_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].value = value;
}

uint32_t sw_timebase_counter_ss_get_end_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].end_value;
}

void sw_timebase_counter_ss_set_end_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].end_value = value;
}

uint32_t sw_timebase_counter_ss_get_end_value_sub_sec(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].end_value_sub_sec;
}

void sw_timebase_counter_ss_set_end_value_sub_sec(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].end_value_sub_sec = value;
}

uint32_t sw_timebase_counter_ss_get_period_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].period_value;
}

void sw_timebase_counter_ss_set_period_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].period_value = value;
}

uint32_t sw_timebase_counter_ss_get_reload_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].reload_value;
}

void sw_timebase_counter_ss_set_reload_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].reload_value = value;
}

uint32_t sw_timebase_counter_ss_get_target_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].target;
}

void sw_timebase_counter_ss_set_target_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].target = value;
}

uint32_t sw_timebase_counter_ss_get_temporary_value(uint8_t index) {
    return sw_timebase->sw_counter_ss[index].temporary;
}

void sw_timebase_counter_ss_set_temporary_value(uint8_t index, uint32_t value) {
    sw_timebase->sw_counter_ss[index].temporary = value;
}




void sw_timebase_counter_ss_reset(uint8_t index) {

    sw_timebase_counter_ss_set_status(index, COUNTER_STATE_RESET);
    sw_timebase_counter_ss_set_temporary_value(index, 0);
    sw_timebase_counter_ss_set_target_value(index, 0);
    sw_timebase_counter_ss_set_reload_value(index, 0);
    sw_timebase_counter_ss_set_period_value(index, 0);
    sw_timebase_counter_ss_set_end_value(index, 0);
    sw_timebase_counter_ss_set_current_value(index, 0);
    sw_timebase_counter_ss_reset_period_status_flag(index);
}


void sw_timebase_counter_ss_clear_flag(uint8_t index) {
    sw_timebase_counter_ss_reset(index);
}

void sw_timebase_counter_ss_start(uint8_t index) {
    if (sw_timebase_counter_ss_get_status(index) != COUNTER_STATE_STARTED) {
        sw_timebase_counter_ss_set_status(index, COUNTER_STATE_START);
    }
}

void sw_timebase_counter_ss_stop(uint8_t index) {
    if (sw_timebase_counter_ss_get_status(index) != COUNTER_STATE_STOPPED) {
        sw_timebase_counter_ss_set_status(index, COUNTER_STATE_STOP);
    }
}

void sw_timebase_counter_ss_set_securely(uint8_t index, uint32_t value) {
    if (sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_RESET) {
        uint32_t curr_s, curr_ss;
        uint32_t subsec_val, sec_val;

        // Reset and initialize
        sw_timebase_counter_ss_set_current_value(index, 0);
        sw_timebase_counter_ss_set_temporary_value(index, 0);
        sw_timebase_counter_ss_set_target_value(index, value);

        // Get current time in seconds + sub-seconds
        curr_s  = sw_timebase_get_seconds();
        curr_ss = sw_timebase_get_sub_seconds();

        // Split 'value' into seconds and sub-seconds
        subsec_val = value % sw_timebase->sw_config.UpdateRate;
        sec_val    = value / sw_timebase->sw_config.UpdateRate;

        // Add current sub-second offset
        subsec_val += curr_ss;

        // Handle sub-second overflow
        if (subsec_val >= sw_timebase->sw_config.UpdateRate) {
            sec_val += 1;
            subsec_val %= sw_timebase->sw_config.UpdateRate;
        }

        // Add current second offset
        sec_val += curr_s;

        // Store end values
        sw_timebase_counter_ss_set_end_value(index, sec_val);
        sw_timebase_counter_ss_set_end_value_sub_sec(index, subsec_val);
        sw_timebase_counter_ss_set_reload_value(index, value);

        // Start counter
        sw_timebase_counter_ss_set_status(index, COUNTER_STATE_STARTED);
    }
}

void sw_timebase_counter_ss_set_forcefully(uint8_t index, uint32_t value) {
    sw_timebase_counter_ss_reset(index);
    sw_timebase_counter_ss_set_securely(index, value);
}



void sw_timebase_counter_ss_update(uint8_t index) {
    uint32_t curr_s, curr_ss, temp_val, sec_val, subsec_val;

    if (sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_STARTED) {
        curr_s  = sw_timebase_get_seconds();
        curr_ss = sw_timebase_get_sub_seconds();

        // Compute remaining sub-seconds
        sec_val    = sw_timebase_counter_ss_get_end_value(index) - curr_s;
        sec_val   *= sw_timebase->sw_config.UpdateRate;
        subsec_val = sw_timebase_counter_ss_get_end_value_sub_sec(index) - curr_ss;
        temp_val   = sec_val + subsec_val;

        // Store temporary remaining value
        sw_timebase_counter_ss_set_temporary_value(index, temp_val);

        // Update current progress
        sw_timebase_counter_ss_set_current_value(
            index,
            sw_timebase_counter_ss_get_target_value(index) - temp_val
        );

        // Handle expiry
        if (temp_val <= 0) {
            sw_timebase_counter_ss_set_end_value(index, 0);
            sw_timebase_counter_ss_set_end_value_sub_sec(index, 0);
            sw_timebase_counter_ss_set_temporary_value(index, 0);
            sw_timebase_counter_ss_set_current_value(index, sw_timebase_counter_ss_get_target_value(index));
            sw_timebase_counter_ss_set_reload_value(index, sw_timebase_counter_ss_get_reload_value(index));
            sw_timebase_counter_ss_set_status(index, COUNTER_STATE_EXPIRED);
        }

    } else if (sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_STOPPED) {
        curr_s  = sw_timebase_get_seconds();
        curr_ss = sw_timebase_get_sub_seconds();

        temp_val   = sw_timebase_counter_ss_get_temporary_value(index);
        subsec_val = temp_val % sw_timebase->sw_config.UpdateRate;
        sec_val    = temp_val / sw_timebase->sw_config.UpdateRate;

        subsec_val += curr_ss;
        if (subsec_val >= sw_timebase->sw_config.UpdateRate) {
            sec_val += 1;
            subsec_val %= sw_timebase->sw_config.UpdateRate;
        }
        sec_val += curr_s;

        // Store new end values
        sw_timebase_counter_ss_set_end_value(index, sec_val);
        sw_timebase_counter_ss_set_end_value_sub_sec(index, subsec_val);

        // Recalculate current value
        sw_timebase_counter_ss_set_current_value(
            index,
            sw_timebase_counter_ss_get_target_value(index) - sw_timebase_counter_ss_get_temporary_value(index)
        );
    }
}


uint8_t sw_timebase_counter_ss_expired(uint8_t index){
    if(sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_EXPIRED){
        return TIMEBASE_TRUE;
    } else {
        return TIMEBASE_FALSE;
    }
}

uint8_t sw_timebase_counter_ss_expired_event(uint8_t index){
    if(sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_EXPIRED){
        sw_timebase_counter_ss_clear_flag(index);
        return TIMEBASE_TRUE;
    } else {
        return TIMEBASE_FALSE;
    }
}

uint8_t sw_timebase_counter_ss_oneshot_expired_event(uint8_t index){
    return sw_timebase_counter_ss_expired_event(index);
}

uint8_t sw_timebase_counter_ss_continous_expired_event(uint8_t index){
    if(sw_timebase_counter_ss_get_status(index) == COUNTER_STATE_EXPIRED){
        uint32_t reload_val = sw_timebase_counter_ss_get_reload_value(index);
        sw_timebase_counter_ss_clear_flag(index);
        sw_timebase_counter_ss_set_securely(index, reload_val);
        return TIMEBASE_TRUE;
    } else {
        return TIMEBASE_FALSE;
    }
}

void sw_timebase_counter_ss_set_period_value_securely(uint8_t index, uint32_t value){
    if(sw_timebase_counter_ss_get_period_status_flag(index) == FLAG_STATE_RESET){
        uint32_t curr   = sw_timebase_counter_ss_get_current_value(index);
        uint32_t target = sw_timebase_counter_ss_get_target_value(index);

        if(curr >= target) return;

        uint32_t next_period = curr + value;
        if(next_period > target) next_period = target;

        sw_timebase_counter_ss_set_period_value(index, next_period);
        sw_timebase_counter_ss_set_period_status_flag(index, FLAG_STATE_SET);
    }
}

uint32_t sw_timebase_counter_ss_get_remaining_period_value(uint8_t index){
    uint32_t curr   = sw_timebase_counter_ss_get_current_value(index);
    uint32_t target = sw_timebase_counter_ss_get_target_value(index);
    uint32_t period = sw_timebase_counter_ss_get_period_value(index);

    if(curr >= target) return 0;

    int32_t temp = (int32_t)(period - curr);
    if(temp < 0) temp = 0;
    return (uint32_t)temp;
}

static inline uint8_t _period_ss_expired_condition(uint8_t index) {
    return (sw_timebase_counter_ss_get_remaining_period_value(index) == 0 &&
            sw_timebase_counter_ss_get_period_status_flag(index) == FLAG_STATE_SET);
}

uint8_t sw_timebase_counter_ss_period_value_expired(uint8_t index){
    return _period_ss_expired_condition(index) ? TIMEBASE_TRUE : TIMEBASE_FALSE;
}

uint8_t sw_timebase_counter_ss_period_value_expired_event(uint8_t index){
    if(_period_ss_expired_condition(index)){
        sw_timebase_counter_ss_set_period_status_flag(index, FLAG_STATE_RESET);
        return TIMEBASE_TRUE;
    }
    return TIMEBASE_FALSE;
}


void sw_timebase_counter_delay_subseconds(uint32_t delay)
{
    uint32_t start_sec, start_subsec;
    uint32_t curr_sec, curr_subsec;
    uint64_t elapsed_ticks;

    // Capture shadow time atomically
    start_sec    = sw_timebase_get_shadow_seconds_securely();
    start_subsec = sw_timebase_get_shadow_sub_seconds_securely();

    while (1)
    {
        // Capture shadow time atomically
        curr_sec    = sw_timebase_get_shadow_seconds_securely();
        curr_subsec = sw_timebase_get_shadow_sub_seconds_securely();

        // Calculate elapsed ticks
        if (curr_subsec >= start_subsec) {
            elapsed_ticks = (uint64_t)(curr_sec - start_sec) * sw_timebase->sw_config.UpdateRate
                          + (curr_subsec - start_subsec);
        } else {
            // Handle sub-second overflow
            elapsed_ticks = (uint64_t)(curr_sec - start_sec - 1) * sw_timebase->sw_config.UpdateRate
                          + (sw_timebase->sw_config.UpdateRate + curr_subsec - start_subsec);
        }

        if (elapsed_ticks >= delay)
            break;
    }
}









void sw_timebase_counter_ss_update_all(void){
    for(uint8_t i = 0; i < TIMEBASE_COUNTER_SS; i++){
        sw_timebase_counter_ss_update(i);
    }
}

void sw_timebase_counter_ss_reset_all(void){
    for(uint8_t i = 0; i < TIMEBASE_COUNTER_SS; i++){
        sw_timebase_counter_ss_reset(i);
    }
}



/************************************************************************************/

void sw_timebase_main_loop_executable(void){
	
	
	if(sw_timebase->updateReq & COUNTER_UPDATE_REQ){
		
			//Update ALL timer
		sw_timebase_variable_sync();
		sw_timebase_counter_update_all();
		sw_timebase_counter_ss_update_all();
		sw_timebase->updateReq &= ~ COUNTER_UPDATE_REQ;
	}
	
	sw_timebase->sw_time.VariablesSync = TIMER_VARIABLE_SYNC_FALSE;
	
}



void sw_timebase_variable_sync(void){
	
	if(sw_timebase->sw_time.VariablesSync == TIMER_VARIABLE_SYNC_FALSE){
		
		sw_timebase_atomic_operation_start();
		sw_timebase->sw_time.sub_seconds = sw_timebase->sw_time.shadow_subseconds;
		sw_timebase->sw_time.seconds = sw_timebase->sw_time.shadow_seconds;
		sw_timebase_atomic_operation_end();
		sw_timebase->sw_time.VariablesSync = TIMER_VARIABLE_SYNC_TRUE;
	}
		
}

