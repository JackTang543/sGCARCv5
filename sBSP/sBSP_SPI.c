#include "sBSP_SPI.h"

#include "sDBG_Debug.h"

#include "sGCARC_Def.h"

//sBSP_SPI.c 20241009 v1
//used for sGCARC
//20241120 v1.1 sGCARCv5



/*SPI1 -> OLED*/
SPI_HandleTypeDef hspi1;
#define OLED_SPI_HANDLE         hspi1
DMA_HandleTypeDef hdma_spi1_tx;
volatile bool oled_spi_tx_busy;

//PA15 -> CS
#define OLED_CS_GPIO_CLK_EN      __GPIOA_CLK_ENABLE
#define OLED_CS_GPIO             GPIOA
#define OLED_CS_GPIO_PIN         GPIO_PIN_15


/*SPI2 -> IMU*/
SPI_HandleTypeDef hspi2;
#define IMU_SPI_HANDLE          hspi2

/*SPI3 -> TRACK 或者 PS2手柄*/
SPI_HandleTypeDef hspi3;
#define TRACK_SPI_HANDLE        hspi3



int sBSP_SPI_OLED_Init(uint32_t SPI_BAUDRATE){
    OLED_SPI_HANDLE.Instance              = SPI1;
    OLED_SPI_HANDLE.Init.Mode             = SPI_MODE_MASTER;
    OLED_SPI_HANDLE.Init.Direction        = SPI_DIRECTION_2LINES;
    OLED_SPI_HANDLE.Init.DataSize         = SPI_DATASIZE_8BIT;
    OLED_SPI_HANDLE.Init.CLKPolarity      = SPI_POLARITY_LOW;
    OLED_SPI_HANDLE.Init.CLKPhase         = SPI_PHASE_1EDGE;
    OLED_SPI_HANDLE.Init.NSS              = SPI_NSS_SOFT;
    OLED_SPI_HANDLE.Init.BaudRatePrescaler= SPI_BAUDRATE;
    OLED_SPI_HANDLE.Init.FirstBit         = SPI_FIRSTBIT_MSB;
    OLED_SPI_HANDLE.Init.TIMode           = SPI_TIMODE_DISABLE;
    OLED_SPI_HANDLE.Init.CRCCalculation   = SPI_CRCCALCULATION_DISABLE;
    OLED_SPI_HANDLE.Init.CRCPolynomial    = 10;

    if (HAL_SPI_Init(&OLED_SPI_HANDLE) != HAL_OK){
        Error_Handler();
    }

    //初始化CS
    OLED_CS_GPIO_CLK_EN();
    GPIO_InitTypeDef gpio;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pin = OLED_CS_GPIO_PIN;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_CS_GPIO,&gpio);

    return 0;
}

inline void sBSP_SPI_OLED_SetCS(bool cs_lv){
    HAL_GPIO_WritePin(OLED_CS_GPIO,OLED_CS_GPIO_PIN,(GPIO_PinState)cs_lv);
}

void sBSP_SPI_OLED_SetEN(uint8_t en){
    en ? __HAL_SPI_ENABLE(&OLED_SPI_HANDLE) : __HAL_SPI_DISABLE(&OLED_SPI_HANDLE);
}

inline void sBSP_SPI_OLED_SendByte(uint8_t byte){
    HAL_SPI_Transmit(&OLED_SPI_HANDLE,&byte,1,100);
}

inline uint8_t sBSP_SPI_OLED_RecvByte(){
    uint8_t send_byte = 0;
    HAL_SPI_Receive (&OLED_SPI_HANDLE,&send_byte,1,100);
    return send_byte;
}

inline void sBSP_SPI_OLED_SendBytes(uint8_t *pData,uint16_t Size){
    oled_spi_tx_busy = 1;
    
    HAL_SPI_Transmit_DMA(&OLED_SPI_HANDLE,pData,Size);
    // while(HAL_SPI_GetState(&OLED_SPI_HANDLE) != HAL_SPI_STATE_READY);
}


inline bool sBSP_SPI_OLED_IsIdle(){
    if(oled_spi_tx_busy == 0){
        return true;
    }else{
        return false;
    }
}

inline void sBSP_SPI_OLED_RecvBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Receive(&OLED_SPI_HANDLE,pData,Size,1000);
}


//SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
void sBSP_SPI_IMU_Init(uint32_t SPI_BAUDRATE){
    IMU_SPI_HANDLE.Instance              = SPI2;
    IMU_SPI_HANDLE.Init.Mode             = SPI_MODE_MASTER;
    IMU_SPI_HANDLE.Init.Direction        = SPI_DIRECTION_2LINES;
    IMU_SPI_HANDLE.Init.DataSize         = SPI_DATASIZE_8BIT;
    IMU_SPI_HANDLE.Init.CLKPolarity      = SPI_POLARITY_LOW;
    IMU_SPI_HANDLE.Init.CLKPhase         = SPI_PHASE_1EDGE;
    IMU_SPI_HANDLE.Init.NSS              = SPI_NSS_SOFT;
    IMU_SPI_HANDLE.Init.BaudRatePrescaler= SPI_BAUDRATE;
    IMU_SPI_HANDLE.Init.FirstBit         = SPI_FIRSTBIT_MSB;
    IMU_SPI_HANDLE.Init.TIMode           = SPI_TIMODE_DISABLE;
    IMU_SPI_HANDLE.Init.CRCCalculation   = SPI_CRCCALCULATION_DISABLE;
    IMU_SPI_HANDLE.Init.CRCPolynomial    = 10;

    if (HAL_SPI_Init(&IMU_SPI_HANDLE) != HAL_OK){
        Error_Handler();
    }
}

void sBSP_SPI_IMU_SetEN(uint8_t en){
    en ? __HAL_SPI_ENABLE(&IMU_SPI_HANDLE) : __HAL_SPI_DISABLE(&IMU_SPI_HANDLE);
}

void sBSP_SPI_IMU_SendByte(uint8_t byte){
    HAL_SPI_Transmit(&IMU_SPI_HANDLE,&byte,1,100);
}

uint8_t sBSP_SPI_IMU_RecvByte(){
    uint8_t send_byte = 0;
    HAL_SPI_Receive (&IMU_SPI_HANDLE,&send_byte,1,100);
    return send_byte;
}

void sBSP_SPI_IMU_SendBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Transmit(&IMU_SPI_HANDLE,pData,Size,1000);
}

void sBSP_SPI_IMU_RecvBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Receive(&IMU_SPI_HANDLE,pData,Size,1000);
}

void sBSP_SPI_TRACK_Init(uint32_t SPI_BAUDRATE){
    //! 未测试
    TRACK_SPI_HANDLE.Instance              = SPI3;
    TRACK_SPI_HANDLE.Init.Mode             = SPI_MODE_MASTER;
    TRACK_SPI_HANDLE.Init.Direction        = SPI_DIRECTION_2LINES;
    TRACK_SPI_HANDLE.Init.DataSize         = SPI_DATASIZE_8BIT;
    TRACK_SPI_HANDLE.Init.CLKPolarity      = SPI_POLARITY_HIGH;
    TRACK_SPI_HANDLE.Init.CLKPhase         = SPI_PHASE_2EDGE;
    TRACK_SPI_HANDLE.Init.NSS              = SPI_NSS_SOFT;
    TRACK_SPI_HANDLE.Init.BaudRatePrescaler= SPI_BAUDRATE;
    TRACK_SPI_HANDLE.Init.FirstBit         = SPI_FIRSTBIT_MSB;
    TRACK_SPI_HANDLE.Init.TIMode           = SPI_TIMODE_DISABLE;
    TRACK_SPI_HANDLE.Init.CRCCalculation   = SPI_CRCCALCULATION_DISABLE;
    TRACK_SPI_HANDLE.Init.CRCPolynomial    = 10;

    if (HAL_SPI_Init(&TRACK_SPI_HANDLE) != HAL_OK){
        Error_Handler();
    }
}

void sBSP_SPI_TRACK_SetEN(uint8_t en){
    en ? __HAL_SPI_ENABLE(&TRACK_SPI_HANDLE) : __HAL_SPI_DISABLE(&TRACK_SPI_HANDLE);
}

void sBSP_SPI_TRACK_SendByte(uint8_t byte){
    HAL_SPI_Transmit(&TRACK_SPI_HANDLE,&byte,1,100);
}

uint8_t sBSP_SPI_TRACK_RecvByte(){
    uint8_t byte = 0;
    HAL_SPI_Receive (&TRACK_SPI_HANDLE,&byte,1,100);
    return byte;
}

void sBSP_SPI_TRACK_SendBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Transmit(&TRACK_SPI_HANDLE,pData,Size,1000);
}

void sBSP_SPI_TRACK_RecvBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Receive(&TRACK_SPI_HANDLE,pData,Size,1000);
}


void sBSP_SPI_PS2_Init(uint32_t SPI_BAUDRATE){
    TRACK_SPI_HANDLE.Instance              = SPI3;
    TRACK_SPI_HANDLE.Init.Mode             = SPI_MODE_MASTER;
    TRACK_SPI_HANDLE.Init.Direction        = SPI_DIRECTION_2LINES;
    TRACK_SPI_HANDLE.Init.DataSize         = SPI_DATASIZE_8BIT;
    TRACK_SPI_HANDLE.Init.CLKPolarity      = SPI_POLARITY_HIGH; //! CPOL=1
    TRACK_SPI_HANDLE.Init.CLKPhase         = SPI_PHASE_2EDGE; //! PS2手柄 2EDGE CPHA=1
    TRACK_SPI_HANDLE.Init.NSS              = SPI_NSS_SOFT;
    TRACK_SPI_HANDLE.Init.BaudRatePrescaler= SPI_BAUDRATE;
    TRACK_SPI_HANDLE.Init.FirstBit         = SPI_FIRSTBIT_LSB;  //! PS2 LSB
    TRACK_SPI_HANDLE.Init.TIMode           = SPI_TIMODE_DISABLE;
    TRACK_SPI_HANDLE.Init.CRCCalculation   = SPI_CRCCALCULATION_DISABLE;
    TRACK_SPI_HANDLE.Init.CRCPolynomial    = 10;

    if (HAL_SPI_Init(&TRACK_SPI_HANDLE) != HAL_OK){
        Error_Handler();
    }
}

void sBSP_SPI_PS2_SetEN(uint8_t en){
    en ? __HAL_SPI_ENABLE(&TRACK_SPI_HANDLE) : __HAL_SPI_DISABLE(&TRACK_SPI_HANDLE);
}

uint8_t sBSP_SPI_PS2_TransferByte(uint8_t send){
    uint8_t recv = 0;
    HAL_SPI_TransmitReceive(&TRACK_SPI_HANDLE,&send,&recv,1,1000);
    return recv;
}





void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
    
    if(hspi->Instance == SPI1){
        //sBSP_UART_Debug_Printf("S\n");
        oled_spi_tx_busy = 0;
        
        //恢复CS默认高电平(用于DMA异步处理)
        sBSP_SPI_OLED_SetCS(1);
    }
}






