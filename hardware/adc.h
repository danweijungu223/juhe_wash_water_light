#ifndef HARDWARE_ADC_H_
#define HARDWARE_ADC_H_

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_rcc_ex.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);
void MX_ADC3_Init(void);

uint16_t get_adc();
uint16_t get_adc2_in0();
uint16_t get_adc3_in10();

#endif /* HARDWARE_ADC_H_ */
