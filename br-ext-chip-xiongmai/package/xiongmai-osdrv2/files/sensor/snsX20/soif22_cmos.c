#include "xm_type.h"
#include "xm_comm_isp.h"
#include "xm_comm_sns.h"
#include "xm_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "xm_awb_comm.h"

#include "xm_print.h"
#include "XAx_cmos.h"

#define STATIC	static
extern GAIN_MODE_DT gstGainModeDt;


STATIC XM_S32 cmos_set_mirror_flip(XM_U8 u8Mirror,XM_U8 u8Flip)
{
	XM_U32 u32Tmp;
	XM_U16 u16OfstH,u16OfsV;
	u32Tmp = sensor_read_register(0x12);
	u32Tmp = u32Tmp&0xCF;
	if(u8Mirror)
	{
		u32Tmp |= 0x20;
		u16OfstH = 1;
	}
	else
	{
		u32Tmp &= 0xDF;
		u16OfstH = 0;
	}
		
	if(u8Flip)
	{
		u32Tmp |= 0x10;
		u16OfsV = 1;
	}
	else
	{
		u32Tmp &= 0xEF;
		u16OfsV = 0;
	}
	sensor_write_register(0x12, u32Tmp);
	VI_WinSet(1, u8Mirror, u8Flip, u16OfstH, u16OfsV);
	return XM_SUCCESS;
}

STATIC XM_VOID cmos_again_calc_table(XM_U32 u32InTimes,AE_SENSOR_GAININFO_S *pstAeSnsGainInfo)
{
	int i;
	XM_U32 u32MinGain;
	if(XM_NULL == pstAeSnsGainInfo)
	{
		ERR("null pointer when get ae sensor gain info  value!\n");
		return;
	}
	u32MinGain = 1<<gstGainModeDt.stLinear.u8AgainMin;
	pstAeSnsGainInfo->u32GainDb = 0;
	pstAeSnsGainInfo->u32SnsTimes = 1024;

	u32InTimes = u32InTimes<<((gstGainModeDt.stLinear.u8AgainSft>>4)&0x0F);
	u32InTimes = u32InTimes>>(gstGainModeDt.stLinear.u8AgainSft&0x0F);
	u32InTimes = u32InTimes < u32MinGain? u32MinGain: u32InTimes;
	
	for(i = 0; i < 10; i++)
	{
		if(u32InTimes < (u32MinGain<<1))
		{
			break;
		}
		u32InTimes >>= 1;
	}
	
	pstAeSnsGainInfo->u32GainDb = (i<<gstGainModeDt.stLinear.u8AgainMin)+u32InTimes-u32MinGain; 
	u32InTimes = u32InTimes<<i;

	u32InTimes = u32InTimes<<(gstGainModeDt.stLinear.u8AgainSft&0x0F);
	u32InTimes = u32InTimes>>((gstGainModeDt.stLinear.u8AgainSft>>4)&0x0F);
	pstAeSnsGainInfo->u32SnsTimes = u32InTimes;
	return;
}






STATIC XM_VOID cmos_inttime_update(XM_U32 u32IntTime)
{
#if 0
	sensor_write_register(0x01, u32IntTime&0xFF);
	sensor_write_register(0x02, (u32IntTime>>8)&0xFF);
#else
	XM_U32 u32Tmp;
	u32Tmp = sensor_read_register(0x1F);
	sensor_write_register(0xC0, 0x01);
	sensor_write_register(0xC1, u32IntTime&0xFF);
	sensor_write_register(0xC2, 0x02);
	sensor_write_register(0xC3, (u32IntTime>>8)&0xFF);
	sensor_write_register(0x1F, u32Tmp|0x80);
#endif
	return;
}

STATIC XM_VOID cmos_gains_update(XM_U32 u32Again, XM_U32 u32Dgain)
{
	sensor_write_register(0x00, u32Again);
	return;
}



/* the function of sensor set fps */
STATIC XM_VOID cmos_fps_set(XM_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
	XM_U32 au32Tmp[3];
	switch(u8Fps)
	{
		case 30:
			// Change the frame rate via changing the vertical blanking
			au32Tmp[0] = 0x4C;
			au32Tmp[1] = 0x04;
			au32Tmp[2] = 0x54;
			break;
		case 25:
		default:
			// Change the frame rate via changing the vertical blanking
			au32Tmp[0] = 0x28;
			au32Tmp[1] = 0x05;
			au32Tmp[2] = 0x14;
			break;
	}		
	pstAeSnsDft->u32MaxIntTime = gu16FullLines-gu8MaxShutterOfst;
	pstAeSnsDft->u32FullLinesStd = gu16FullLines;
	pstAeSnsDft->u32LinesPer500ms = gu16FullLines * u8Fps / 2;
	sensor_write_register(0x20,au32Tmp[0]);
	sensor_write_register(0x21,au32Tmp[1]);
	sensor_write_register(0x0D,au32Tmp[2]);
	return;
}





XM_S32 cmos_init_sensor_exp_function_f22(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
#if 0
	pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
	pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
	pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
	pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
#endif
	pstSensorExpFunc->pfn_cmos_set_mirror_flip = cmos_set_mirror_flip;
	return 0;
}

XM_S32 cmos_init_ae_exp_function_f22(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
	//pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
	if((gstGainModeDt.stLinear.u8AgainSft==0xFF) && (gstGainModeDt.stLinear.u8AgainMin==0xFF))
	{
		gstGainModeDt.stLinear.u8AgainSft = 6;
		gstGainModeDt.stLinear.u8AgainMin = 4;
	}
	pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
	pstExpFuncs->pfn_cmos_slow_framerate_set= NULL;    
	pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
	pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;
	pstExpFuncs->pfn_cmos_again_calc_table  = cmos_again_calc_table;
	pstExpFuncs->pfn_cmos_dgain_calc_table  = cmos_gain_calc_table_com;
	pstExpFuncs->pfn_cmos_shut_calc_table = NULL;
	return 0;
}

