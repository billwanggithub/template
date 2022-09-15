/**
 * @file my_script.c
 * @author your name (you@domain.com)
 * @brief Emulate the Adruino script
 * @version 0.1
 * @date 2022-08-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "my_script.h"

#include "FreeRTOS.h"
#include "HardwareConfig.h"
#include "hardware_capture.h"
#include "my_adc.h"
#include "my_gpio.h"
#include "my_i2c.h"
#include "my_led_buzzer.h"
#include "my_pwm.h"
#include "my_task.h"
#include "my_uart.h"
#include "portmacro.h"
#include "stdio.h"
#include "stream_buffer.h"
#include "task.h"
#include "uart_programmer.h"
#include "usb_device.h"
#include "usbd_custom_hid_if.h"

#if 1                                                   // Buzzer Configuration
Indicator led1{LED1_PORT, LED1_PIN, 1, 255, 50, 1000};  // port, pin, bar, count, high, low

void led1_task() { led1.task(); }

void led1_set(uint8_t cnt, uint32_t hi, uint32_t low) { led1.set(cnt, hi, low); }

void led1_enable(uint8_t enable) { led1.enable = enable; }
#endif

#if 1                                                       // Buzzer Configuration
Indicator buzzer{BUZZER_PORT, BUZZER_PIN, 0, 0, 50, 1000};  // port, pin, bar, count, high, low

void buzzer_task() { buzzer.task(); }

void buzzer_set(uint8_t cnt, uint32_t hi, uint32_t low) { buzzer.set(cnt, hi, low); }

void buzzer_enable(uint8_t enable) { buzzer.enable = enable; }
#endif

#if 1  // Capture Stuff
/* Capture stuff */
// capture data structure with header
// for Packing data structures see
// https://developer.arm.com/documentation/100748/0616/Writing-Optimized-Code/Packing-data-structures
volatile CaptureData_t capture1_data;
extern StreamBufferHandle_t CAPTURE_StreamBuffer_Handle;

/**
 * @brief
 * place this into void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * if change the system tick, it will appear in main.c,
 * else just override here
 * @param htim
 */
void Capture32::capture_callback(TIM_HandleTypeDef* htim)
{
  if (htim->Instance == _htim->Instance)
  {
    // Active edge?
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
      // digitalToggle(TEST0_PORT, TEST0_PIN);
      //  Save Period
      period = _timx->CCR1 + 1;

      capture1_data.period = period;
      capture1_data.width = width;
      capture1_data.mA = 0xAAAA;
      capture1_data.mV = 0xBBBB;
      capture1_data.time_ms = HAL_GetTick();
      UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();  // Disable interrupt      
      xStreamBufferSendFromISR(CAPTURE_StreamBuffer_Handle, (void*)&capture1_data, sizeof(capture1_data), pdFALSE);
      taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
      width = _timx->CCR2 + 1;
    }

    // Clear Update Event
    htim->Instance->SR &= ~TIM_SR_UIF;
  }
}

// place this into HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim),
// usually in main.c call HAL_TIM_Base_Start_IT(htim) to enable update
// interrupt
void Capture32::update_callback(TIM_HandleTypeDef* htim)
{
  if (htim->Instance == _htim->Instance)
  {
    // digitalToggle(TEST0_PORT, TEST0_PIN);
    capture1_data.period = overflow + 1;
    capture1_data.width = overflow + 1;
    capture1_data.mA = 0xCCCC;
    capture1_data.mV = 0xDDDD;
    capture1_data.time_ms = HAL_GetTick();
    UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();  // Disable interrupt
    xStreamBufferSendFromISR(CAPTURE_StreamBuffer_Handle, (void*)&capture1_data, sizeof(capture1_data), pdFALSE);
    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);

    // vTaskDelay(1); //
    //		 SetARR(this->overflow);
    //		 GenerateUpdateEvent(false); // no interrupt

    // if (enable_capture_debug)
    //   My_GPIO::SetOutput(TEST0_PORT, TEST0_PIN, 1);  // PA9
    // if (enable_capture_debug) My_GPIO::SetOutput(PA, 4, 1);

    // // overflow
    // cap1_data.first_fg_time = 0xFFFFFFFF;

    // // check capture input state
    // if (capture1.edge == 0)
    // {
    //   if (My_GPIO::GetInput(CAPIN1_PORT, CAPIN1_PIN) == 0)
    //   {
    //     // if low
    //     push_measurementQueue(overflow + 1, overflow, INA228_vshunt,
    //                           INA228_vshunt, timer1);
    //   }
    //   else
    //   {
    //     // if high
    //     push_measurementQueue(overflow + 1, 0, INA228_vshunt, INA228_vshunt,
    //                           timer1);
    //   }
    // }
    // else
    // {
    //   if (My_GPIO::GetInput(CAPIN1_PORT, CAPIN1_PIN) == 1)
    //   {
    //     // if high
    //     push_measurementQueue(overflow + 1, overflow, INA228_vshunt,
    //                           INA228_vshunt, timer1);
    //   }
    //   else
    //   {
    //     // if low
    //     push_measurementQueue(overflow + 1, 0, INA228_vshunt, INA228_vshunt,
    //                           timer1);
    //   }
    // }
    // if (enable_capture_debug)
    // {
    //   My_GPIO::SetOutput(TEST0_PORT, TEST0_PIN, 0);  // PA9
    //   My_GPIO::SetOutput(PA, 4, 0);
    // }
  }
}

extern TIM_HandleTypeDef htim2;
Capture32 capture1;
void Init_Capture(uint8_t edge, uint32_t ov)
{
  // capture timer
  capture1.ticks_per_second = 1000000;  // 1us
  capture1.psc = (SystemCoreClock / capture1.ticks_per_second) - 1;
  capture1.filter = 5;

  capture1.edge = edge;
  capture1.overflow = ov;
  capture1.Init(&htim2, edge, ov - 1);
  capture1.Stop();
  // capture1.Start();
}

void Capture_Start() { capture1.Start(); }
void Capture_Stop() { capture1.Stop(); }

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) { capture1.capture_callback(htim); }

// wrap for C
void tim_update_callback(TIM_HandleTypeDef* htim) { capture1.update_callback(htim); }

#endif

#if 1  // FRTOS
extern TaskHandle_t AppTaskCreate_Handle;
BaseType_t Init_FreeRTOS()
{
  BaseType_t xReturn = pdPASS; /* 定義一個創建資訊返回值,預設為pdPASS */

  /* 開發板硬體初始化 */
  // UART_printf("FreeRTOS Init\r\n");
  /* 創建AppTaskCreate任務 */
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,         /* 任務入口函數 */
                        (const char*)"AppTaskCreate",          /* 任務名字 */
                        (uint16_t)512,                         /* 任務棧大小 */
                        (void*)NULL,                           /* 任務入口函數參數 */
                        (UBaseType_t)1,                        /* 任務的優先順序 */
                        (TaskHandle_t*)&AppTaskCreate_Handle); /* 任務控制塊指標 */
  return xReturn;
}

/**
 * @brief  if task overflow, it will run here.
 * @param  [IN]task handle.
 * @param  [IN]task name string pointer.
 * @retval None
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
{
  __disable_irq();

  // TODO: Indicate Overflow with LEDs

  for (;;)
  {
  }
}
#endif

#if 1  // ARDUINO Script Calls
void setup()
{
  // turn off buffers, this costs 1032 bytes of ram
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  pinMode(TRIGGER_PORT, TRIGGER_PIN, GPIO_MODE_OUTPUT_PP, 0);  // TRIGGER
  pinMode(BUZZER_PORT, BUZZER_PIN, GPIO_MODE_OUTPUT_PP, 0);    // BUZZER
  pinMode(BUTTON_PORT, BUTTON_PIN, INPUT, GPIO_PULLUP);        // BUTTON
  pinMode(TEST0_PORT, TEST0_PIN, GPIO_MODE_OUTPUT_PP, 0);      // TEST0_PORT

  UART_GPIO_DeInit();

  PWM_TIMx_Init(PWM1_PORT, PWM1_PIN, PWM1_ID, PWM1_CHANNEL, 1000, 0, 0xFFFF);
  PWM_TIMx_Init(PWM2_PORT, PWM2_PIN, PWM2_ID, PWM2_CHANNEL, 1000, 0, 0xFFFF);

  Init_Capture(0, 1000000);

  MX_USB_DEVICE_Init();

#ifdef USE_FREERTOS
  BaseType_t xReturn = Init_FreeRTOS();
  /* 啟動任務調度 */
  if (pdPASS == xReturn)
  {
    vTaskStartScheduler(); /* Startup freeRTOS */
  }
#endif
}

void loop()
{
#ifndef USE_FREERTOS
  digitalToggle(LED1_PORT, LED1_PIN);
  HAL_Delay(100);
#endif
}

#endif