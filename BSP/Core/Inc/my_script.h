/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_SCRIPT_H__
#define __MY_SCRIPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "my_adc.h"
#include "uart_programmer.h"

#define USE_FREERTOS

#ifdef USE_FREERTOS
#define DELAY(x) vTaskDelay(x)
#else
#define DELAY(x) HAL_Delay(x)
#endif

  typedef struct __attribute__((packed))
  {
    //__attribute__((packed))uint32_t period;
    uint32_t period;
    uint32_t width;
    uint16_t mV;
    uint16_t mA;
    //__attribute__((packed))uint32_t time_ms;
    uint32_t time_ms;
  } CaptureData_t;

  extern UART_HandleTypeDef huart1;
  extern uint8_t programming_mode;
  extern __IO uint16_t ADC_ConvertedValue[ADC_NOFCHANEL];

  void setup();
  void loop();

  void led1_task();
  void led1_set(uint8_t cnt, uint32_t hi, uint32_t low);
  void led1_enable(uint8_t enable);

  void buzzer_task();
  void buzzer_set(uint8_t cnt, uint32_t hi, uint32_t low);
  void buzzer_enable(uint8_t enable);

  void tim_update_callback(TIM_HandleTypeDef* htim);

  void Capture_Start();
  void Capture_Stop();

#ifdef __cplusplus
}
#endif

#endif /* __MY_SCRIPT_H__ */