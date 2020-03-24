#ifndef APPMAIN_H
#define APPMAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_system.h"

void AppMain(void);

void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif

