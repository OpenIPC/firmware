/*
 * copyright (c) hisilicon technologies co., ltd. 2016-2019. all rights reserved.
 * description: thine_241a242_serdes.h
 * author: hisilicon multimedia software group
 * create: 2019-05-17
 */

#ifndef __THINE_241A242_SERDES_H__
#define __THINE_241A242_SERDES_H__

#include <linux/delay.h>

#include "hi_serdes.h"
#include "serdes_comm.h"
#include "type.h"

#define SERDES_DEBUG            0
#define THINE_PLL_VALUE_NUM     7

typedef struct
{
    HI_U8 pll_value[THINE_PLL_VALUE_NUM];
} serdes_config;

serdes_config imx307_4_lane_linear_12_bit_30_fps_cfg =
{
    .pll_value = {0x13, 0x1b, 0x66, 0x01, 0x41, 0x00, 0x2b},
};

serdes_config imx307_4_lane_wdr_12_bit_30_fps_cfg =
{
    .pll_value = {0x13, 0x18, 0x44, 0x02, 0x21, 0x00, 0x2b},
};

serdes_config imx307_2_lane_linear_12_bit_30_fps_cfg =
{
    .pll_value = {0x11, 0x1b, 0x66, 0x01, 0x21, 0x02, 0x29},
};

serdes_config imx307_2_lane_wdr_12_bit_30_fps_cfg =
{
    .pll_value = {0x11, 0x18, 0x44, 0x02, 0x11, 0x02, 0x29},
};

#endif /* end of #ifdef __THINE_241A242_SERDES_H__ */
