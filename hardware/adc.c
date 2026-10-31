#include "adc.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;

void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {

  }

  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {

  }

}

void MX_ADC2_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc2.Instance = ADC2;
    hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc2.Init.Resolution = ADC_RESOLUTION_12B;
    hadc2.Init.ScanConvMode = DISABLE;
    hadc2.Init.ContinuousConvMode = DISABLE;
    hadc2.Init.DiscontinuousConvMode = DISABLE;
    hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc2.Init.NbrOfConversion = 1;
    hadc2.Init.DMAContinuousRequests = DISABLE;
    hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc2) != HAL_OK)
    {
        // Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; // 你要求最大采样周期
    if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
    {
        // Error_Handler();
    }
}

void MX_ADC3_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.ScanConvMode = DISABLE;
    hadc3.Init.ContinuousConvMode = DISABLE;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.NbrOfConversion = 1;
    hadc3.Init.DMAContinuousRequests = DISABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc3) != HAL_OK)
    {
        // Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        // Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN6
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  if(adcHandle->Instance == ADC2)
  {
      __HAL_RCC_ADC2_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();

      /* PA0 -> ADC2_IN0 */
      GPIO_InitStruct.Pin = GPIO_PIN_0;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  if(adcHandle->Instance == ADC3)
    {
        __HAL_RCC_ADC3_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        /* PC0 -> ADC3_IN10 */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {

    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN6
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
  }
  else if(adcHandle->Instance == ADC2)
  {
      __HAL_RCC_ADC2_CLK_DISABLE();
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
  }else if(adcHandle->Instance == ADC3)
  {
      __HAL_RCC_ADC3_CLK_DISABLE();
      HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
  }
}

uint16_t get_adc(){

    HAL_ADC_Start(&hadc1);

    HAL_ADC_PollForConversion(&hadc1,100);

    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC)){

       return HAL_ADC_GetValue(&hadc1);
    }
    return 0;
}

uint16_t get_adc2_in0(void)
{
    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, 100);

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
    {
        return HAL_ADC_GetValue(&hadc2);
    }
    return 0;
}

//VCC=(4095raw​⋅3.3)⋅21
float get_VBUS_V(){
    uint16_t adc2 = get_adc2_in0();
    float u = adc2/4095 * 3.3f * 21;
    return u;
}

uint16_t get_adc3_in10(void)
{
    HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, 100);

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc3), HAL_ADC_STATE_REG_EOC))
    {
        return HAL_ADC_GetValue(&hadc3);
    }
    return 0;
}

//I=50⋅0(系数).005(电阻5mΩ)(4095raw​⋅3.3)​=4⋅(4095raw​⋅3.3)
float get_IBUS_S(){
    uint16_t adc3 = get_adc3_in10();
    float i = adc3/4095 * 3.3f * 4;
    return i;
}
