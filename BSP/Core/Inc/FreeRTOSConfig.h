/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stdio.h"
#include "stm32f4xx.h"

//針對不同的編譯器調用不同的stdint.h文件
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
extern uint32_t SystemCoreClock;
#endif

//extern uint32_t debug_timerTick;

//斷言
#define vAssertCalled(char, int) printf("Error:%s,%d\r\n", char, int)
#define configASSERT(x)                                                        \
  if ((x) == 0)                                                                \
  vAssertCalled(__FILE__, __LINE__)

/************************************************************************
 *               FreeRTOS基礎配置配置選項
 *********************************************************************/
/* 置1：RTOS使用搶佔式調度器；置0：RTOS使用協作式調度器（時間片）
 *
 * 注：在多工管理機制上，作業系統可以分為搶佔式和協作式兩種。
 * 協作式作業系統是任務主動釋放CPU後，切換到下一個任務。
 * 任務切換的時機完全取決於正在運行的任務。
 */
#define configUSE_PREEMPTION 1

// 1使能時間片調度(默認式使能的)
#define configUSE_TIME_SLICING 1

/* 某些運行FreeRTOS的硬體有兩種方法選擇下一個要執行的任務：
 * 通用方法和特定於硬體的方法（以下簡稱“特殊方法”）。
 *
 * 通用方法：
 *      1.configUSE_PORT_OPTIMISED_TASK_SELECTION 為 0
 * 或者硬體不支援這種特殊方法。 2.可以用於所有FreeRTOS支援的硬體
 *      3.完全用C實現，效率略低於特殊方法。
 *      4.不強制要求限制最大可用優先順序數目
 * 特殊方法：
 *      1.必須將configUSE_PORT_OPTIMISED_TASK_SELECTION設置為1。
 *      2.依賴一個或多個特定架構的彙編指令（一般是類似計算前置字元為零[CLZ]指令）。
 *      3.比通用方法更高效
 *      4.一般強制限定最大可用優先順序數目為32
 * 一般是硬體計算前置字元為零指令，如果所使用的，MCU沒有這些硬體指令的話此巨集應該設置為0！
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

/* 置1：使能低功耗tickless模式；置0：保持系統節拍（tick）中斷一直運行
 * 假設開啟低功耗的話可能會導致下載出現問題，因為程式在睡眠中,可用以下辦法解決
 *
 * 下載方法：
 *      1.將開發版正常連接好
 *      2.按住重定按鍵，點擊下載瞬間鬆開重定按鍵
 *
 *      1.通過跳線帽將 BOOT 0 接高電平(3.3V)
 *      2.重新上電，下載
 *
 *          1.使用FlyMcu擦除一下晶片，然後進行下載
 *          STMISP -> 清除晶片(z)
 */
#define configUSE_TICKLESS_IDLE 0

/*
 * 寫入實際的CPU內核時鐘頻率，也就是CPU指令執行頻率，通常稱為Fclk
 * Fclk為供給CPU內核的時鐘信號，我們所說的cpu主頻為 XX MHz，
 * 就是指的這個時鐘信號，相應的，1/Fclk即為cpu時鐘週期；
 */
#define configCPU_CLOCK_HZ (SystemCoreClock)

// RTOS系統節拍中斷的頻率。即一秒中斷的次數，每次中斷RTOS都會進行任務調度
#define configTICK_RATE_HZ ((TickType_t)1000)

//可使用的最大優先順序
#define configMAX_PRIORITIES (32)

//空閒任務使用的堆疊大小
#define configMINIMAL_STACK_SIZE ((unsigned short)512)

//任務名字字串長度
#define configMAX_TASK_NAME_LEN (16)

//系統節拍計數器變數資料類型，1表示為16位元無符號整形，0表示為32位元無符號整形
#define configUSE_16_BIT_TICKS 0

//空閒任務放棄CPU使用權給其他同優先順序的用戶任務
#define configIDLE_SHOULD_YIELD 1

//啟用佇列
#define configUSE_QUEUE_SETS 1

//開啟任務通知功能，默認開啟
#define configUSE_TASK_NOTIFICATIONS 1

//使用互斥信號量
#define configUSE_MUTEXES 1

//使用遞迴互斥信號量
#define configUSE_RECURSIVE_MUTEXES 1

//為1時使用計數信號量
#define configUSE_COUNTING_SEMAPHORES 1

/* 設置可以註冊的信號量和訊息佇列個數 */
#define configQUEUE_REGISTRY_SIZE 10

#define configUSE_APPLICATION_TASK_TAG 0

/*****************************************************************
              FreeRTOS與記憶體申請有關配置選項
*****************************************************************/
//支援動態記憶體申請
#define configSUPPORT_DYNAMIC_ALLOCATION 1
//支援靜態記憶體
#define configSUPPORT_STATIC_ALLOCATION 0
//系統所有總的堆大小
#define configTOTAL_HEAP_SIZE ((size_t)(32 * 1024))

/***************************************************************
             FreeRTOS與鉤子函數有關的配置選項
**************************************************************/
/* 置1：使用空閒鉤子（Idle Hook類似於回呼函數）；置0：忽略空閒鉤子
 *
 * 空閒任務鉤子是一個函數，這個函數由使用者來實現，
 * FreeRTOS規定了函數的名字和參數：void vApplicationIdleHook(void )，
 * 這個函數在每個空閒任務週期都會被調用
 * 對於已經刪除的RTOS任務，空閒任務可以釋放分配給它們的堆疊記憶體。
 * 因此必須保證空閒任務可以被CPU執行
 * 使用空閒鉤子函數設置CPU進入省電模式是很常見的
 * 不可以調用會引起空閒任務阻塞的API函數
 */
#define configUSE_IDLE_HOOK 0

/* 置1：使用時間片鉤子（Tick Hook）；置0：忽略時間片鉤子
 *
 *
 * 時間片鉤子是一個函數，這個函數由使用者來實現，
 * FreeRTOS規定了函數的名字和參數：void vApplicationTickHook(void )
 * 時間片中斷可以週期性的調用
 * 函數必須非常短小，不能大量使用堆疊，
 * 不能調用以”FromISR" 或 "FROM_ISR”結尾的API函數
 */
/*xTaskIncrementTick函數是在xPortSysTickHandler中斷函數中被調用的。因此，vApplicationTickHook()函數執行的時間必須很短才行*/
#define configUSE_TICK_HOOK 0

//使用記憶體申請失敗鉤子函數
#define configUSE_MALLOC_FAILED_HOOK 0

/*
 * 大於0時啟用堆疊溢位檢測功能，如果使用此功能
 * 使用者必須提供一個棧溢出鉤子函數，如果使用的話
 * 此值可以為1或者2，因為有兩種棧溢出檢測方法 */
#define configCHECK_FOR_STACK_OVERFLOW 0

/********************************************************************
          FreeRTOS與執行時間和任務狀態收集有關的配置選項
**********************************************************************/
/* 調試時使用,在工程完成時需要遮罩掉,因為會增加系統開銷 */
#define configUSE_TRACE_FACILITY 0      //為1啟用視覺化跟蹤調試
#define configGENERATE_RUN_TIME_STATS 0 //為1時啟用執行時間統計功能
#define configUSE_STATS_FORMATTING_FUNCTIONS 0 //與宏configUSE_TRACE_FACILITY同時為1時會編譯下面3個函數
// prvWriteNameToBuffer(),vTaskList(),
// vTaskGetRunTimeStats()
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() (debug_timerTick = 0)
//#define portGET_RUN_TIME_COUNTER_VALUE() debug_timerTick

/********************************************************************
                FreeRTOS與協程有關的配置選項
*********************************************************************/
//啟用協程，啟用協程以後必須添加檔croutine.c
#define configUSE_CO_ROUTINES 0
//協程的有效優先順序數目
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/***********************************************************************
                FreeRTOS與軟體計時器有關的配置選項
**********************************************************************/
//啟用軟體計時器
#define configUSE_TIMERS 0
//軟體計時器優先順序
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
//軟體計時器佇列長度
#define configTIMER_QUEUE_LENGTH 10
//軟體計時器任務堆疊大小
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2)

/************************************************************
            FreeRTOS可選函數配置選項
************************************************************/
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_vTaskPrioritySet 1
#define INCLUDE_uxTaskPriorityGet 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskCleanUpResources 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xTimerPendFunctionCall 0
//#define INCLUDE_xTaskGetCurrentTaskHandle       1
//#define INCLUDE_uxTaskGetStackHighWaterMark     0
//#define INCLUDE_xTaskGetIdleTaskHandle          0

/******************************************************************
            FreeRTOS與中斷有關的配置選項
******************************************************************/
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif
//中斷最低優先順序
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15

//系統可管理的最高中斷優先順序
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 1

#define configKERNEL_INTERRUPT_PRIORITY                                        \
  (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS)) /* 240 */

#define configMAX_SYSCALL_INTERRUPT_PRIORITY                                   \
  (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/****************************************************************
            FreeRTOS與中斷服務函數有關的配置選項
****************************************************************/
#define xPortPendSVHandler PendSV_Handler
#define vPortSVCHandler SVC_Handler
#define xPortSysTickHandler SysTick_handler

/* 以下為使用Percepio Tracealyzer需要的東西，不需要時將 configUSE_TRACE_FACILITY
 * 定義為 0 */
#if (configUSE_TRACE_FACILITY == 1)
//#include "trcRecorder.h"
#define INCLUDE_xTaskGetCurrentTaskHandle                                      \
  1 // 啟用一個可選函數（該函數被 Trace源碼使用，默認該值為0 表示不用）
#endif

#endif /* FREERTOS_CONFIG_H */
