/*
 * drv_reset.c
 *
 *  Created on: Jul 21, 2022
 *      Author: aelmod
 */

#include "drv_reset.h"
#include "main.h"

void perform_system_reset(void) {
    __disable_irq();
    NVIC_SystemReset();
}

/*
 * solution - https://stackoverflow.com/questions/26891432/jump-to-bootloader-in-stm32-through-application-i-e-using-boot-0-and-boot-1-pins
 *
 * reset after bootloader:
 * python C:/Users/aelmod/dfu-util/pydfu.py -x
 * https://techoverflow.net/2021/10/06/how-to-exit-reset-stm32-dfu-bootloader/
 */
void machine_bootloader(void) {
    __enable_irq();
    HAL_RCC_DeInit();
    HAL_DeInit();

    SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
//    HAL_RCC_DeInit();
//    HAL_DeInit();

    __HAL_REMAPMEMORY_SYSTEMFLASH();

    // arm-none-eabi-gcc 4.9.0 does not correctly inline this
    // MSP function, so we write it out explicitly here.
    //__set_MSP(*((uint32_t*) 0x00000000));
    __ASM volatile ("movs r3, #0\nldr r3, [r3, #0]\nMSR msp, r3\n" : : : "r3", "sp");

    ((void (*)(void)) *((uint32_t *) 0x00000004))();

//    while (1);
    perform_system_reset();
}

//void jump_to_bootloader( void )
//{
//    __enable_irq();
//    HAL_RCC_DeInit();
//    HAL_DeInit();
//    SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
//    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
//
//    const uint32_t p = (*((uint32_t *) 0x1FFF0000));
//    __set_MSP( p );
//
//    void (*SysMemBootJump)(void);
//    SysMemBootJump = (void (*)(void)) (*((uint32_t *) 0x1FFF0004));
//    SysMemBootJump();
//
//    while( 1 ) {}
//}
