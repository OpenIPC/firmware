/*
 * Automatically generated C config: don't edit
 * Busybox version: 
 */
#define AUTOCONF_TIMESTAMP "2019-10-18 18:20:56 CST"


/*
 * General Setup
 */
#define CONFIG_HI3516EV200 1
#define CONFIG_HI_CHIP_TYPE 0x3516E200
#define CONFIG_HI_ARCH "hi3516ev200"
#define CONFIG_HI_SUBARCH ""
#define CONFIG_HI_SUBCHIP_TYPE 0x
#define CONFIG_SUBCHIP_HI3516EV200 1
#define CONFIG_HI_SUBARCH ""
#define CONFIG_HI_SUBCHIP_TYPE 0x
#define CONFIG_UP 1
#define CONFIG_ARM_ARCH_TYPE "up"
#define CONFIG_A7 1
#define CONFIG_CPU_TYPE "a7"
#define CONFIG_VERSION_ASIC 1
#define CONFIG_HI_FPGA "n"
#define CONFIG_LINUX_OS 1
#define CONFIG_OS_TYPE "linux"
#define CONFIG_LINUX_4_9_y 1
#define CONFIG_KERNEL_VERSION "linux-4.9.y"
#define CONFIG_KERNEL_ARM_HIMIX100_LINUX 1
#define CONFIG_HI_CROSS "arm-himix100-linux-"
#define CONFIG_LIBC_TYPE "uclibc"
#define CONFIG_KERNEL_BIT "KERNEL_BIT_32"
#define CONFIG_USER_ARM_HIMIX100_LINUX 1
#define CONFIG_HI_CROSS_LIB "arm-himix100-linux-"
#define CONFIG_USER_BIT "USER_BIT_32"
#define CONFIG_RELEASE_TYPE_RELEASE 1
#define CONFIG_HI_RLS_MODE "HI_RELEASE"
#define CONFIG_CUSTOMER_VERSION_COMMON 1
#define CONFIG_CUSTOMER_VERSION "COMMON"

/*
 * Media Modules Setup
 */

/*
 * media base config
 */

/*
 * media sys config
 */
#define CONFIG_HI_SYS_SUPPORT 1
#define CONFIG_HI_SYS_SCALE_COEF_SUPPORT 1

/*
 * media vi config
 */
#define CONFIG_HI_VI_SUPPORT 1
#define CONFIG_HI_VI_BT656 1
#define CONFIG_HI_VI_MIPI 1
#define CONFIG_HI_ISP_2DOF_DIS_SUPPORT 1
#define CONFIG_HI_VI_LDCV3_SUPPORT 1
#define CONFIG_HI_VI_USERPIC_SUPPORT 1
#define CONFIG_HI_VI_COVEREX_OVERLAYEX_SUPPORT 1
#define CONFIG_HI_VI_LUMA_SUPPORT 1
#define CONFIG_HI_VI_FPN_SUPPORT 1
#define CONFIG_HI_VI_EXTCHN_SUPPORT 1

/*
 * media isp config
 */
#define CONFIG_HI_ISP_SUPPORT 1
#define CONFIG_HI_ISP_AF_SUPPORT 1
#define CONFIG_HI_ISP_CR_SUPPORT 1
#define CONFIG_HI_ISP_PREGAMMA_SUPPORT 1
#define CONFIG_HI_ISP_CA_SUPPORT 1

/*
 * media dis config
 */

/*
 * media vpss config
 */
#define CONFIG_HI_VPSS_SUPPORT 1
#define CONFIG_HI_VPSS_3DNR 1
#define CONFIG_HI_VPSS_CHN0_BUF_WRAP 1
#define CONFIG_HI_VPSS_LUMA_STAT_SUPPORT 1
#define CONFIG_HI_VPSS_VGS_LDC_SUPPORT 1
#define CONFIG_HI_VPSS_VGS_ROTATION_SUPPORT 1
#define CONFIG_HI_VPSS_COVER_SUPPORT 1
#define CONFIG_HI_VPSS_MOSAIC_SUPPORT 1
#define CONFIG_HI_VPSS_BUFFER_REUSE_SUPPORT 1

/*
 * media avs config
 */

/*
 * media gdc config
 */

/*
 * media vgs config
 */
#define CONFIG_HI_VGS_SUPPORT 1
#define CONFIG_HI_VGS_IVE_PRE_SUPPORT 1
#define CONFIG_HI_VGS_LUMA_STAT_SUPPORT 1

/*
 * media chnl config
 */
#define CONFIG_HI_CHNL_SUPPORT 1

/*
 * media venc config
 */
#define CONFIG_HI_VENC_SUPPORT 1
#define CONFIG_HI_H265E_SUPPORT 1
#define CONFIG_HI_H265E_USERDATA_SUPPORT 1
#define CONFIG_HI_H265E_INTRA_REFRESH_SUPPORT 1
#define CONFIG_HI_H264E_SUPPORT 1
#define CONFIG_HI_H264E_SVC_SUPPORT 1
#define CONFIG_HI_H264E_USERDATA_SUPPORT 1
#define CONFIG_HI_H264E_INTRA_REFRESH_SUPPORT 1
#define CONFIG_HI_JPEGE_SUPPORT 1
#define CONFIG_HI_MJPEGE_SUPPORT 1
#define CONFIG_HI_JPEGE_DCF_SUPPORT 1
#define CONFIG_HI_JPEGE_USERDATA_SUPPORT 1
#define CONFIG_HI_VENC_LOWDELAY_SUPPORT 1
#define CONFIG_HI_VENC_FRAMEBUF_RECYCLE_SUPPORT 1
#define CONFIG_HI_VENC_MPF_VGS_SUPPORT 1
#define CONFIG_HI_VENC_VGS_SUPPORT 1
#define CONFIG_HI_VENC_SMARTP_SUPPORT 1
#define CONFIG_HI_VENC_DUALP_SUPPORT 1
#define CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT 1
#define CONFIG_HI_VENC_DEBREATH_SUPPORT 1
#define CONFIG_HI_VENC_SKIPREF_SUPPORT 1
#define CONFIG_HI_VENC_SCENE0_SUPPORT 1
#define CONFIG_HI_VENC_SCENE1_SUPPORT 1
#define CONFIG_HI_VENC_SCENE2_SUPPORT 1
#define CONFIG_HI_RC_AVBR_SUPPORT 1
#define CONFIG_HI_RC_QPMAP_SUPPORT 1
#define CONFIG_HI_RC_QVBR_SUPPORT 1
#define CONFIG_HI_RC_CVBR_SUPPORT 1

/*
 * media vdec config
 */
#define CONFIG_VDEC_IP ""

/*
 * media vo config
 */
#define CONFIG_HI_VO_SUPPORT 1
#define CONFIG_HI_VO_COVER_OSD_SUPPORT 1
#define CONFIG_HI_VO_VGS 1
#define CONFIG_HI_VO_GRAPH 1

/*
 * media region config
 */
#define CONFIG_HI_REGION_SUPPORT 1
#define CONFIG_HI_RGN_DIFFPIXOSD_SUPPORT 1

/*
 * media audio config
 */
#define CONFIG_HI_AUDIO_SUPPORT 1
#define CONFIG_HI_ACODEC_SUPPORT 1
#define CONFIG_HI_ACODEC_VERSION "V750"
#define CONFIG_HI_ACODEC_MAX_GAIN 56
#define CONFIG_HI_ACODEC_MIN_GAIN 0
#define CONFIG_HI_ACODEC_GAIN_STEP 3
#define CONFIG_HI_ACODEC_FAST_POWER_SUPPORT 1
#define CONFIG_HI_DOWNVQE_SUPPORT 1
#define CONFIG_HI_TALKVQE_SUPPORT 1
#define CONFIG_HI_RECORDVQE_SUPPORT 1
#define CONFIG_HI_AUDIO_STATIC_REGISTER_SUPPORT 1
#define CONFIG_HI_AUDIO_BCD_SUPPORT 1
#define CONFIG_HI_AUDIO_3518EV300_SUPPORT_BCD 1

/*
 * media hdr config
 */

/*
 * media mcf config
 */

/*
 * Device Driver Setup
 */

/*
 * drv config
 */
#define CONFIG_DRV 1
#define CONFIG_EXTDRV 1
#define CONFIG_INTERDRV 1
#define CONFIG_CIPHER 1
#define CONFIG_HIUSER 1
#define CONFIG_MIPI_RX 1
#define CONFIG_HI_WDG 1
#define CONFIG_HI_SYSCFG 1
#define CONFIG_HI_LSADC 1

/*
 * Component Setup
 */

/*
 * Component hdmi Config
 */

/*
 * Component hifb Config
 */
#define CONFIG_HI_HIFB_SUPPORT 1
#define CONFIG_HI_HIFB_VGS 1

/*
 * Component svp Config
 */
#define CONFIG_HI_SVP_IVE 1
#define CONFIG_HI_SVP_IVE_NORM_GRAD 1
#define CONFIG_HI_SVP_IVE_ST_CANDI_CORNER 1
#define CONFIG_HI_SVP_IVE_CNN_EX 1
#define CONFIG_HI_SVP_IVP 1
#define CONFIG_HI_SVP_MD 1
#define CONFIG_HI_SVP_QR 1

/*
 * Component photo Config
 */

/*
 * Component tde Config
 */
#define CONFIG_HI_TDE_SUPPORT 1

/*
 * Component pciv Config
 */

/*
 * Component avs lut Config
 */

/*
 * Component pos_query Config
 */

/*
 * Component tzasc Config
 */

/*
 * Component motionfusion config
 */

/*
 * Component pm Config
 */
#define CONFIG_HI_PM_SUPPORT 1

/*
 * HISYSLINK Setup
 */

/*
 * hisyslink config
 */

/*
 * Debug Config
 */
#define CONFIG_HI_GDB_NO 1
#define CONFIG_HI_GDB "n"
#define CONFIG_HI_LOG_TRACE_SUPPORT 1
#define CONFIG_HI_LOG_TRACE_ALL 1
#define CONFIG_HI_LOG_TRACE_LEVEL 7

/*
 * Test Config
 */
