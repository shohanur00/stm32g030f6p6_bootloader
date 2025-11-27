#ifndef SW_TIMEBASE_H
#define SW_TIMEBASE_H

#include "stm32g030xx.h"

#define TIMEBASE_COUNTER	25
#define TIMEBASE_COUNTER_SS	25


void sw_timebase_struct_init(void);
void sw_timebase_enable(uint32_t UpdateHz);
void sw_timebase_Init(uint32_t UpdateHz);
void sw_timebase_disable(uint8_t timer_num);
void sw_timebase_ISR_executables(void);
void sw_timebase_main_loop_executable(void);
void sw_timebase_variable_sync(void);







uint16_t sw_timebase_get_sub_seconds(void);
uint16_t sw_timebase_get_shadow_sub_seconds(void);
uint32_t sw_timebase_get_seconds(void);
uint32_t sw_timebase_get_shadow_seconds(void);
uint64_t sw_timebase_get_shadow_sub_seconds_uptime(void);

void sw_timebase_set_sub_seconds(uint16_t value);
void sw_timebase_set_shadow_sub_seconds(uint16_t value);
void sw_timebase_set_seconds(uint32_t value);
void sw_timebase_set_shadow_seconds(uint32_t value);
void sw_timebase_set_shadow_sub_seconds_uptime(uint64_t value);


uint16_t sw_timebase_get_shadow_sub_seconds_securely(void);
uint32_t sw_timebase_get_shadow_seconds_securely(void);
uint64_t sw_timebase_get_shadow_sub_seconds_uptime_securely(void);



uint32_t sw_timebase_counter_get_current_value(uint8_t	index);
void sw_timebase_counter_set_current_value(uint8_t	index, uint32_t	value);

uint8_t	sw_timebase_counter_get_status(uint8_t index);
void	sw_timebase_counter_set_status(uint8_t index,uint8_t	value);

uint32_t sw_timebase_counter_get_end_value(uint8_t	index);
void sw_timebase_counter_set_end_value(uint8_t	index, uint32_t	value);

uint32_t sw_timebase_counter_get_period_value(uint8_t	index);
void sw_timebase_counter_set_period_value(uint8_t	index, uint32_t	value);
uint8_t	sw_timebase_counter_get_period_status_flag(uint8_t index);
void	sw_timebase_counter_set_period_status_flag(uint8_t index,uint8_t	value);
void	sw_timebase_counter_reset_period_status_flag(uint8_t index);

uint32_t sw_timebase_counter_get_target_value(uint8_t	index);
void sw_timebase_counter_set_target_value(uint8_t	index, uint32_t	value);

uint32_t sw_timebase_counter_get_temporary_value(uint8_t	index);
void sw_timebase_counter_set_temporary_value(uint8_t	index, uint32_t	value);

uint32_t sw_timebase_counter_get_reload_value(uint8_t	index);
void sw_timebase_counter_set_reload_value(uint8_t	index, uint32_t	value);

void sw_timebase_counter_reset(uint8_t index);
void sw_timebase_counter_clear_flag(uint8_t	index);
void sw_timebase_counter_start(uint8_t	index);
void sw_timebase_counter_stop(uint8_t	index);
void sw_timebase_counter_set_securely(uint8_t index,uint32_t value);
void sw_timebase_counter_set_forcefully(uint8_t index,uint32_t value);
void sw_timebase_counter_update(uint8_t index);
uint8_t sw_timebase_counter_expired(uint8_t index);
uint8_t sw_timebase_counter_expired_event(uint8_t index);
uint8_t sw_timebase_counter_oneshot_expired_event(uint8_t index);
uint8_t sw_timebase_counter_continous_expired_event(uint8_t index);
void sw_timebase_counter_set_period_value_securely(uint8_t	index, uint32_t value);
uint32_t	sw_timebase_counter_get_remaining_period_value(uint8_t index);
uint8_t sw_timebase_counter_period_value_expired(uint8_t index);
uint8_t sw_timebase_counter_period_value_expired_event(uint8_t index);
void sw_timebase_counter_delay_seconds(uint32_t	delay);
void sw_timebase_counter_reset_all(void);
void sw_timebase_counter_update_all(void);






// ============================================================================
//  Sub Seconds
// ============================================================================

// --- STATUS FUNCTIONS ---
uint8_t sw_timebase_counter_ss_get_status(uint8_t index);
void    sw_timebase_counter_ss_set_status(uint8_t index, uint8_t value);

uint8_t sw_timebase_counter_ss_get_period_status_flag(uint8_t index);
void    sw_timebase_counter_ss_set_period_status_flag(uint8_t index, uint8_t value);
void    sw_timebase_counter_ss_reset_period_status_flag(uint8_t index);

// --- VALUE FUNCTIONS ---
uint32_t sw_timebase_counter_ss_get_current_value(uint8_t index);
void     sw_timebase_counter_ss_set_current_value(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_end_value(uint8_t index);
void     sw_timebase_counter_ss_set_end_value(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_end_value_sub_sec(uint8_t index);
void sw_timebase_counter_ss_set_end_value_sub_sec(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_period_value(uint8_t index);
void     sw_timebase_counter_ss_set_period_value(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_reload_value(uint8_t index);
void     sw_timebase_counter_ss_set_reload_value(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_target_value(uint8_t index);
void     sw_timebase_counter_ss_set_target_value(uint8_t index, uint32_t value);

uint32_t sw_timebase_counter_ss_get_temporary_value(uint8_t index);
void     sw_timebase_counter_ss_set_temporary_value(uint8_t index, uint32_t value);

// --- CORE COUNTER CONTROL ---
void sw_timebase_counter_ss_reset(uint8_t index);
void sw_timebase_counter_ss_clear_flag(uint8_t index);
void sw_timebase_counter_ss_start(uint8_t index);
void sw_timebase_counter_ss_stop(uint8_t index);
void sw_timebase_counter_ss_set_securely(uint8_t index, uint32_t value);
void sw_timebase_counter_ss_set_forcefully(uint8_t index, uint32_t value);
void sw_timebase_counter_ss_update(uint8_t index);

// --- EXPIRE FUNCTIONS ---
uint8_t sw_timebase_counter_ss_expired(uint8_t index);
uint8_t sw_timebase_counter_ss_expired_event(uint8_t index);
uint8_t sw_timebase_counter_ss_oneshot_expired_event(uint8_t index);
uint8_t sw_timebase_counter_ss_continous_expired_event(uint8_t index);

// --- PERIOD FUNCTIONS ---
void     sw_timebase_counter_ss_set_period_value_securely(uint8_t index, uint32_t value);
uint32_t sw_timebase_counter_ss_get_remaining_period_value(uint8_t index);
uint8_t  sw_timebase_counter_ss_period_value_expired(uint8_t index);
uint8_t  sw_timebase_counter_ss_period_value_expired_event(uint8_t index);


void sw_timebase_counter_delay_subseconds(uint32_t	delay);

// --- BULK OPERATIONS ---
void sw_timebase_counter_ss_update_all(void);
void sw_timebase_counter_ss_reset_all(void);











void sw_hardware_timer_disable(void);
void sw_hardware_timer_reset(void);
void sw_hardware_timer_interrupt_enable(void);
void sw_hardware_timer_interrupt_disable(void);


void sw_timebase_atomic_operation_start(void);
void sw_timebase_atomic_operation_end(void);

#endif
