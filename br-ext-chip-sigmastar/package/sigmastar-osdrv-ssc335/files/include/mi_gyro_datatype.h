#ifndef _MI_GYRO_DATATYPE_H_
#define _MI_GYRO_DATATYPE_H_

#include <cam_os_wrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FIFO_MAX_CNT  (512)

typedef enum
{
    E_GYROSENSOR_ALL_ACCEL_FIFO_EN = 0x08,
    E_GYROSENSOR_ZG_FIFO_EN        = 0x10,
    E_GYROSENSOR_YG_FIFO_EN        = 0x20,
    E_GYROSENSOR_XG_FIFO_EN        = 0x40,
    E_GYROSENSOR_TEMP_FIFO_EN      = 0x80,
    E_GYROSENSOR_FIFO_MAX_EN       = 0xFF,
}GyroSensorEnFifoMode_e;

typedef enum
{
    E_GYROSENSOR_GYRO_RANGE_125    =0x00,
    E_GYROSENSOR_GYRO_RANGE_250    =0x08,
    E_GYROSENSOR_GYRO_RANGE_500    =0x10,
    E_GYROSENSOR_GYRO_RANGE_MASK   =0x18,
}GyroSensorGyroRange_e;

typedef enum
{
    E_GYROSENSOR_ACCEL_RANGE_2G    =0x00,
    E_GYROSENSOR_ACCEL_RANGE_4G    =0x08,
    E_GYROSENSOR_ACCEL_RANGE_8G    =0x10,
    E_GYROSENSOR_ACCEL_RANGE_16G   =0x18,
}GyroSensorAccelRange_e;

typedef struct MI_Gyro_InitParam_s
{
    MI_U32 u32DevId;
    MI_U8 *u8Data;
} MI_Gyro_InitParam_t;

#ifdef __cplusplus
}
#endif

#endif
