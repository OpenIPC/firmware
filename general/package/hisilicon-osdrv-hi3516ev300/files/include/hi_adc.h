/*
*
* Copyright (c) 2017 Hisilicon Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*
*/

#ifndef __HI_ADC_H__
#define __HI_ADC_H__


#define LSADC_IOCTL_BASE 'A'

typedef enum hiIOC_NR_LSADC_E
{
    IOC_NR_LSADC_MODEL_SEL = 0,
    IOC_NR_LSADC_CHN_ENABLE,
    IOC_NR_LSADC_CHN_DISABLE,
    IOC_NR_LSADC_START,
    IOC_NR_LSADC_STOP,
    IOC_NR_LSADC_GET_CHNVAL,
    IOC_NR_LSADC_BUTT
}IOC_NR_LSADC_E;

#define LSADC_IOC_MODEL_SEL         _IOWR(LSADC_IOCTL_BASE, IOC_NR_LSADC_MODEL_SEL, int)
#define LSADC_IOC_CHN_ENABLE        _IOW(LSADC_IOCTL_BASE, IOC_NR_LSADC_CHN_ENABLE, int)
#define LSADC_IOC_CHN_DISABLE       _IOW(LSADC_IOCTL_BASE, IOC_NR_LSADC_CHN_DISABLE, int)
#define LSADC_IOC_START             _IO(LSADC_IOCTL_BASE, IOC_NR_LSADC_START)
#define LSADC_IOC_STOP              _IO(LSADC_IOCTL_BASE, IOC_NR_LSADC_STOP)
#define LSADC_IOC_GET_CHNVAL        _IOWR(LSADC_IOCTL_BASE, IOC_NR_LSADC_GET_CHNVAL, int)




#endif /* __HI_ADC_H__ */

