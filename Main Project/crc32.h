#ifndef __CRC32_H
#define __CRC32_H

#include "stm32g0xx.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief  Initializes the CRC hardware.
 *         Enables CRC clock and resets CRC unit.
 */
void CRC32_Init(void);

/**
 * @brief  Calculates CRC32 of a data buffer (word aligned).
 * @param  data: Pointer to 32-bit data array.
 * @param  length: Number of 32-bit words.
 * @retval Computed CRC32 value.
 */
uint32_t CRC32_Calculate(const uint32_t *data, uint32_t length);

/**
 * @brief  Calculates CRC32 over a memory range (e.g., flash region).
 * @param  startAddr: Start address of memory block.
 * @param  length: Length in bytes.
 * @retval CRC32 value.
 */
uint32_t CRC32_MemoryRange(uint32_t startAddr, uint32_t length);

#endif /* __CRC32_H */

