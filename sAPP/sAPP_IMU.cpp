#include "sAPP_IMU.hpp"


sAPP_IMU imu;



#include "sGCARC_Def.h"
#include "sBSP_SPI.h"


/**
 * sAPP_AHRS.cpp
 * 用于获取IMU的数据
 * 
 */


//#define USE_ICM42688
#define USE_ICM45686


sAPP_IMU::sAPP_IMU(){

}

sAPP_IMU::~sAPP_IMU(){
    
}


int sAPP_IMU::init(){

    this->icm  = &g_icm;
    this->icm45686 = &g_icm45686;
    this->lis3 = &g_lis3;

    /*把IMU的2个CS都上拉*/
    __GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio.Pin   = ICM_CS_Pin | LIS3_CS_Pin;
    HAL_GPIO_Init(GPIOC,&gpio);
    HAL_GPIO_WritePin(GPIOC,ICM_CS_Pin ,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,LIS3_CS_Pin,GPIO_PIN_SET);

    /*SPI2 <-> IMU_SPI*/
    sBSP_SPI_IMU_Init(SPI_BAUDRATEPRESCALER_4);   //11.25MBits/s

    #ifdef USE_ICM42688
        if(sDRV_ICM_Init() != 0){
            return -1;
        }
    #endif
    #ifdef USE_ICM45686
        if(sDRV_ICM45686_Init() != 0){
            return -1;
        }
    #endif
    

    if(sDRV_LIS3_Init() != 0){
        return -2;
    }

    return 0;
}


int sAPP_IMU::update(){
    
    #ifdef USE_ICM42688
        sDRV_ICM_GetData();
    #endif
    #ifdef USE_ICM45686
        sDRV_ICM45686_GetData();
    #endif

    sDRV_LIS3_GetData();


    #ifdef USE_ICM42688
        acc_x = icm->acc_x;
        acc_y = icm->acc_y;
        acc_z = icm->acc_z;
        gyr_x = icm->gyro_x;
        gyr_y = icm->gyro_y;
        gyr_z = icm->gyro_z;
        icm_temp = icm->temp;
    #endif
    #ifdef USE_ICM45686
        acc_x = icm45686->acc_x;
        acc_y = icm45686->acc_y;
        acc_z = icm45686->acc_z;
        gyr_x = icm45686->gyro_x;
        gyr_y = icm45686->gyro_y;
        gyr_z = icm45686->gyro_z;
        icm_temp = icm45686->temp;
    #endif

    mag_x = lis3->mag_x;
    mag_y = lis3->mag_y;
    mag_z = lis3->mag_z;
    lis3_temp = lis3->temp;
    tick = icm->tick;


    return 0;
}


