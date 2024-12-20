#include "sDBG_Debug.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sBSP_UART.h"

char fmt_buf[256];



//! 不能用,有bug
// void sBSP_UART_Debug_Printf(const char *fmt,...){
//     sBSP_UART_Debug_Printf("[WARN]:");
//     sBSP_UART_Debug_Printf(fmt);
//     sBSP_UART_Debug_Printf("\n");
// }

// void sDBG_Debug_Info(const char *fmt,...){
//     sBSP_UART_Debug_Printf("[INFO]:");
//     sBSP_UART_Debug_Printf(fmt);
//     sBSP_UART_Debug_Printf("\n");
// }



void Error_Handler(){
    __disable_irq();
    while (1){
        sBSP_UART_Debug_Printf("[ERROR] 错误: 禁用IRQ,死循环...\n");
        HAL_Delay(500);
    }
}

void Warning_Handler(uint8_t* file, uint32_t line){
    sBSP_UART_Debug_Printf("WARNING警告: 文件名:%s,行:%u\n",file,line);
}




void assert_failed(uint8_t* file, uint32_t line){
    __disable_irq();
    while (1){
        sBSP_UART_Debug_Printf("WARNING警告:断言失败! 文件名:%s,行:%u 禁用IRQ,死循环...\n",file,line);
        HAL_Delay(500);
    }
}

void vApplicationMallocFailedHook(){
    sBSP_UART_Debug_Printf("WARNING警告:内存申请失败! 禁用IRQ\n");
}

void vApplicationIdleHook(){
    
}   

void vApplicationTickHook(){
}


void vApplicationStackOverflowHook(TaskHandle_t xTask,char* pcTaskName){
    sBSP_UART_Debug_Printf("WARNING警告:触发栈金丝雀机制! 任务句柄:0x%X,任务名:%s\n",xTask,pcTaskName);
}


