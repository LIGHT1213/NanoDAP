#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "main.h"
#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include "system_stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_customhid.h"
void DEVICE_Init(void);
#define LED_CONNECTED_Pin   GPIO_PIN_13
#define LED_GPIO_Port       GPIOC
#endif

