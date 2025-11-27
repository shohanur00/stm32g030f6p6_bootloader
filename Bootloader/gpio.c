#include "gpio.h"

void GPIO_Init(GPIO_TypeDef *GPIOx, uint16_t pin,
               GPIO_Mode_t mode, GPIO_OType_t otype,
               GPIO_Pull_t pull, GPIO_Speed_t speed)
{
    // Mode
    GPIOx->MODER &= ~(0x3U << (pin * 2));
    GPIOx->MODER |=  ((mode & 0x3U) << (pin * 2));

    // Output type
    if ((mode == GPIO_MODE_OUTPUT) || (mode == GPIO_MODE_AF)) {
        if (otype == GPIO_OTYPE_OD)
            GPIOx->OTYPER |=  (1U << pin);
        else
            GPIOx->OTYPER &= ~(1U << pin);
    }

    // Speed
    GPIOx->OSPEEDR &= ~(0x3U << (pin * 2));
    GPIOx->OSPEEDR |=  ((speed & 0x3U) << (pin * 2));

    // Pull
    GPIOx->PUPDR &= ~(0x3U << (pin * 2));
    GPIOx->PUPDR |=  ((pull & 0x3U) << (pin * 2));
}

void GPIO_SetPin(GPIO_TypeDef *GPIOx, uint16_t pin){
    GPIOx->BSRR = (1U << pin);
}

void GPIO_ResetPin(GPIO_TypeDef *GPIOx, uint16_t pin){
    GPIOx->BSRR = (1U << (pin + 16));
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t pin){
    GPIOx->ODR ^= (1U << pin);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t pin)
{
    return ( (GPIOx->IDR >> pin) & 0x1U );
}
