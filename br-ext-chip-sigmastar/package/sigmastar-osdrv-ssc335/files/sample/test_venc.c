#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

#include <mi_sys.h>
#include <mi_sensor.h>
#include <mi_vif.h>
#include <mi_vpe.h>
#include <mi_venc.h>

#define GCI_SENSOR_PAD      E_MI_VIF_SNRPAD_ID_0
#define GCI_PIXEL_FORMAT    E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420
// #define GCI_VIF_WORK_MODE   E_MI_VIF_WORK_MODE_RGB_FRAMEMODE
#define GCI_VIF_WORK_MODE   E_MI_VIF_WORK_MODE_RGB_REALTIME

/// Initialize the MPI.
int32_t gci_sys_init(void) {
    // make sure that we're starting with a clean table
    MI_SYS_Exit();

    return MI_SYS_Init();
}

/// Release the MPI resources.
int32_t gci_sys_exit(void) {
    return MI_SYS_Exit();
}

/// Initialize the sensor.
int32_t gci_snr_init(uint8_t hdr, uint32_t resolution_id) {
    MI_S32 ret;

    MI_SNR_Disable(GCI_SENSOR_PAD);

    if ((ret = MI_SNR_SetPlaneMode(GCI_SENSOR_PAD, hdr ? TRUE : FALSE)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_SNR_SetRes(GCI_SENSOR_PAD, resolution_id)) != MI_SUCCESS) {
        return ret;
    }

    return MI_SNR_Enable(GCI_SENSOR_PAD);
}

/// Deinitialize the sensor.
int32_t gci_snr_deinit(void) {
    return MI_SNR_Disable(GCI_SENSOR_PAD);
}

/// Find the highest resolution offering at least a given FPS.
int32_t gci_snr_get_max_resolution(uint32_t fps, uint32_t* resolution_id) {
    MI_U32 count, i, current_res, max_res, max_res_index, max_res_found;
    MI_SNR_Res_t res;
    MI_S32 ret;

    if ((ret = MI_SNR_QueryResCount(GCI_SENSOR_PAD, &count)) != MI_SUCCESS) {
        return ret;
    }

    max_res = 0;
    max_res_found = 0;

    for (i = 0; i < count; i++) {
        if ((ret = MI_SNR_GetRes(GCI_SENSOR_PAD, i, &res)) != MI_SUCCESS) {
            return ret;
        }

        current_res = res.stOutputSize.u16Width * res.stOutputSize.u16Height;

        if (res.u32MaxFps < fps) {
            continue;
        } else if (max_res_found) {
            if (current_res > max_res) {
                max_res = current_res;
                max_res_index = i;
            }
        } else {
            max_res = current_res;
            max_res_index = i;
            max_res_found = 1;
        }
    }

    if (max_res_found) {
        *resolution_id = max_res_index;
    }

    return max_res_found ? MI_SUCCESS : MI_ERR_SNR_INVALID_PARA;
}

/// Get the maximum available FPS.
int32_t gci_snr_get_max_fps(uint32_t* fps) {
    MI_U32 count, i, max_fps;
    MI_SNR_Res_t res;
    MI_S32 ret;

    if ((ret = MI_SNR_QueryResCount(GCI_SENSOR_PAD, &count)) != MI_SUCCESS) {
        return ret;
    }

    max_fps = 0;

    for (i = 0; i < count; i++) {
        if ((ret = MI_SNR_GetRes(GCI_SENSOR_PAD, i, &res)) != MI_SUCCESS) {
            return ret;
        }

        if (res.u32MaxFps > max_fps) {
            max_fps = res.u32MaxFps;
        }
    }

    *fps = max_fps;

    return MI_SUCCESS;
}

/// Get current FPS.
int32_t gci_snr_get_fps(uint32_t* fps) {
    MI_SNR_Res_t res;
    MI_U8 index;
    MI_S32 ret;

    if ((ret = MI_SNR_GetCurRes(GCI_SENSOR_PAD, &index, &res)) != MI_SUCCESS) {
        return ret;
    }

    *fps = res.u32MaxFps;

    return MI_SUCCESS;
}

/// Initialize the video input device.
///
/// The sensor must be already initialized at this point.
static int32_t gci_vif_init_dev(void) {
    MI_SNR_PADInfo_t pad_info;
    MI_VIF_DevAttr_t dev_attrs;
    MI_S32 ret;

    if ((ret = MI_SNR_GetPadInfo(GCI_SENSOR_PAD, &pad_info)) != MI_SUCCESS) {
        return ret;
    }

    memset(&dev_attrs, 0, sizeof(dev_attrs));

    dev_attrs.eIntfMode = pad_info.eIntfMode;
    dev_attrs.eWorkMode = GCI_VIF_WORK_MODE;
    dev_attrs.eHDRType = E_MI_VIF_HDR_TYPE_OFF;

    if( dev_attrs.eIntfMode == E_MI_VIF_MODE_BT656) {
        dev_attrs.eClkEdge = pad_info.unIntfAttr.stBt656Attr.eClkEdge;
    } else {
        dev_attrs.eClkEdge = E_MI_VIF_CLK_EDGE_DOUBLE;
    }

    if (dev_attrs.eIntfMode == E_MI_VIF_MODE_MIPI) {
        dev_attrs.eDataSeq = pad_info.unIntfAttr.stMipiAttr.eDataYUVOrder;
    } else {
        dev_attrs.eDataSeq = E_MI_VIF_INPUT_DATA_YUYV;
    }

    if (dev_attrs.eIntfMode == E_MI_VIF_MODE_BT656) {
        memcpy(
            &dev_attrs.stSyncAttr,
            &pad_info.unIntfAttr.stBt656Attr.stSyncAttr,
            sizeof(MI_VIF_SyncAttr_t));
    }

    dev_attrs.eBitOrder = E_MI_VIF_BITORDER_NORMAL;

    if ((ret = MI_VIF_SetDevAttr(0, &dev_attrs)) != MI_SUCCESS) {
        return ret;
    }

    return MI_VIF_EnableDev(0);
}

/// Initialize the video input channel-port.
///
/// The video input device must be already initialized at this point.
static int32_t gci_vif_init_channel_port(void) {
    MI_SNR_PADInfo_t pad_info;
    MI_SNR_PlaneInfo_t plane_info;
    MI_VIF_ChnPortAttr_t port_attrs;
    MI_S32 ret;

    if ((ret = MI_SNR_GetPadInfo(GCI_SENSOR_PAD, &pad_info)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_SNR_GetPlaneInfo(GCI_SENSOR_PAD, 0, &plane_info)) != MI_SUCCESS) {
        return ret;
    }

    memset(&port_attrs, 0, sizeof(port_attrs));

    port_attrs.stCapRect.u16X = plane_info.stCapRect.u16X;
    port_attrs.stCapRect.u16Y = plane_info.stCapRect.u16Y;
    port_attrs.stCapRect.u16Width = plane_info.stCapRect.u16Width;
    port_attrs.stCapRect.u16Height = plane_info.stCapRect.u16Height;
    port_attrs.stDestSize.u16Width = plane_info.stCapRect.u16Width;
    port_attrs.stDestSize.u16Height = plane_info.stCapRect.u16Height;
    port_attrs.ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(
        plane_info.ePixPrecision,
        plane_info.eBayerId);

    if (pad_info.eIntfMode == E_MI_VIF_MODE_BT656) {
        port_attrs.eFrameRate = E_MI_VIF_FRAMERATE_FULL;
        port_attrs.eCapSel = E_MI_SYS_FIELDTYPE_BOTH;
        port_attrs.eScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
    }

    if ((ret = MI_VIF_SetChnPortAttr(0, 0, &port_attrs)) != MI_SUCCESS) {
        return ret;
    }

    return MI_VIF_EnableChnPort(0, 0);
}

/// Initialize the video input.
int32_t gci_vif_init(void) {
    MI_U32 resolution;
    MI_U32 fps;
    MI_S32 ret;

    if ((ret = gci_snr_get_max_fps(&fps)) != MI_SUCCESS) {
        return ret;
    }

    // cap the FPS to 30 for querying the max resolution
    if (fps > 30) {
        fps = 30;
    }

    if ((ret = gci_snr_get_max_resolution(fps, &resolution)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = gci_snr_init(0, resolution)) != MI_SUCCESS) {
        return ret;
    }

    MI_VIF_DisableDev(0);

    if ((ret = gci_vif_init_dev()) != MI_SUCCESS) {
        goto dev_init_err;
    }

    if ((ret = gci_vif_init_channel_port()) != MI_SUCCESS) {
        goto channel_port_init_err;
    }

    return MI_SUCCESS;

channel_port_init_err:
    MI_VIF_DisableDev(0);

dev_init_err:
    gci_snr_deinit();

    return ret;
}

/// Deinitialize the video input.
int32_t gci_vif_deinit(void) {
    MI_S32 ret;

    if ((ret = MI_VIF_DisableChnPort(0, 0)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VIF_DisableDev(0)) != MI_SUCCESS) {
        return ret;
    }

    return gci_snr_deinit();
}

/// Get VPE channel attributes.
static int32_t gci_vpe_get_channel_attrs(MI_VPE_ChannelAttr_t* attrs) {
    MI_VIF_DevAttr_t vif_dev_attrs;
    MI_VIF_ChnPortAttr_t vif_port_attrs;
    MI_S32 ret;

    if ((ret = MI_VIF_GetDevAttr(0, &vif_dev_attrs)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VIF_GetChnPortAttr(0, 0, &vif_port_attrs)) != MI_SUCCESS) {
        return ret;
    }

    memset(attrs, 0, sizeof(MI_VPE_ChannelAttr_t));

    attrs->eRunningMode = vif_dev_attrs.eWorkMode == E_MI_VIF_WORK_MODE_RGB_REALTIME
        ? E_MI_VPE_RUN_REALTIME_MODE
        : E_MI_VPE_RUN_CAM_MODE;
    attrs->eHDRType = E_MI_VPE_HDR_TYPE_OFF;
    attrs->ePixFmt = vif_port_attrs.ePixFormat;
    attrs->u16MaxW = vif_port_attrs.stDestSize.u16Width;
    attrs->u16MaxH = vif_port_attrs.stDestSize.u16Height;
    attrs->bNrEn = FALSE;
    attrs->bEsEn = FALSE;
    attrs->bEdgeEn = FALSE;
    attrs->bUvInvert = FALSE;
    attrs->bContrastEn = FALSE;
    attrs->eSensorBindId = E_MI_VPE_SENSOR0;

    return MI_SUCCESS;
}

/// Get VPE port mode.
static int32_t gci_vpe_get_port_mode(MI_VPE_PortMode_t* mode) {
    MI_VPE_ChannelAttr_t attrs;
    MI_S32 ret;

    if ((ret = MI_VPE_GetChannelAttr(0, &attrs)) != MI_SUCCESS) {
        return ret;
    }

    memset(mode, 0, sizeof(MI_VPE_PortMode_t));

    mode->u16Width = attrs.u16MaxW;
    mode->u16Height = attrs.u16MaxH;
    mode->ePixelFormat = GCI_PIXEL_FORMAT;
    mode->eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;

    return MI_SUCCESS;
}

/// Bind VPE and VIF.
static int32_t gci_vpe_bind_channel(void) {
    MI_VIF_DevAttr_t vif_dev_attrs;
    MI_SYS_ChnPort_t vif_output_port;
    MI_SYS_ChnPort_t vpe_input_port;
    MI_U32 frame_rate;
    MI_S32 ret;

    if ((ret = gci_snr_get_fps(&frame_rate)) != MI_SUCCESS) {
        return ret;
    }
    
    if ((ret = MI_VIF_GetDevAttr(0, &vif_dev_attrs)) != MI_SUCCESS) {
        return ret;
    }

    memset(&vif_output_port, 0, sizeof(vif_output_port));
    memset(&vpe_input_port, 0, sizeof(vpe_input_port));

    vif_output_port.eModId = E_MI_MODULE_ID_VIF;
    vif_output_port.u32DevId = 0;
    vif_output_port.u32ChnId = 0;
    vif_output_port.u32PortId = 0;

    vpe_input_port.eModId = E_MI_MODULE_ID_VPE;
    vpe_input_port.u32DevId = 0;
    vpe_input_port.u32ChnId = 0;
    vpe_input_port.u32PortId = 0;

    return MI_SYS_BindChnPort2(
        &vif_output_port,
        &vpe_input_port,
        frame_rate,
        frame_rate,
        vif_dev_attrs.eWorkMode == E_MI_VIF_WORK_MODE_RGB_REALTIME
            ? E_MI_SYS_BIND_TYPE_REALTIME
            : E_MI_SYS_BIND_TYPE_FRAME_BASE,
        0);
}

/// Unbind VPE and VIF.
static int32_t gci_vpe_unbind_channel(void) {
    MI_SYS_ChnPort_t vif_output_port;
    MI_SYS_ChnPort_t vpe_input_port;

    memset(&vif_output_port, 0, sizeof(vif_output_port));
    memset(&vpe_input_port, 0, sizeof(vpe_input_port));

    vif_output_port.eModId = E_MI_MODULE_ID_VIF;
    vif_output_port.u32DevId = 0;
    vif_output_port.u32ChnId = 0;
    vif_output_port.u32PortId = 0;

    vpe_input_port.eModId = E_MI_MODULE_ID_VPE;
    vpe_input_port.u32DevId = 0;
    vpe_input_port.u32ChnId = 0;
    vpe_input_port.u32PortId = 0;

    return MI_SYS_UnBindChnPort(&vif_output_port, &vpe_input_port);
}

/// Initialize the Video Processing Engine.
int32_t gci_vpe_init(void) {
    MI_VPE_ChannelAttr_t channel_attrs;
    MI_VPE_PortMode_t port_mode;
    MI_S32 ret;

    if ((ret = gci_vpe_get_channel_attrs(&channel_attrs)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VPE_CreateChannel(0, &channel_attrs)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VPE_StartChannel(0)) != MI_SUCCESS) {
        goto destroy_channel;
    }

    if ((ret = gci_vpe_bind_channel()) != MI_SUCCESS) {
        goto stop_channel;
    }

    if ((ret = gci_vpe_get_port_mode(&port_mode)) != MI_SUCCESS) {
        goto unbind_channel;
    }

    if ((ret = MI_VPE_SetPortMode(0, 0, &port_mode)) != MI_SUCCESS) {
        goto unbind_channel;
    }

    if ((ret = MI_VPE_EnablePort(0, 0)) != MI_SUCCESS) {
        goto unbind_channel;
    }

    return MI_SUCCESS;

unbind_channel:
    gci_vpe_unbind_channel();

stop_channel:
    MI_VPE_StopChannel(0);

destroy_channel:
    MI_VPE_DestroyChannel(0);

    return ret;
}

/// Deinitialize the Video Processing Engine.
int32_t gci_vpe_deinit(void) {
    MI_S32 ret;

    if ((ret = MI_VPE_DisablePort(0, 0)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = gci_vpe_unbind_channel()) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VPE_StopChannel(0)) != MI_SUCCESS) {
        return ret;
    }

    return MI_VPE_DestroyChannel(0);
}

/// Create an MJPEG encoding channel.
static int32_t gci_venc_init_mjpeg(uint32_t dst_frame_rate) {
    MI_VPE_ChannelAttr_t vpe_attrs;
    MI_VENC_ChnAttr_t attrs;
    MI_S32 ret;

    if ((ret = MI_VPE_GetChannelAttr(0, &vpe_attrs)) != MI_SUCCESS) {
        return ret;
    }

    memset(&attrs, 0, sizeof(attrs));

    attrs.stVeAttr.eType = E_MI_VENC_MODTYPE_JPEGE;
    attrs.stVeAttr.stAttrJpeg.u32MaxPicWidth = vpe_attrs.u16MaxW;
    attrs.stVeAttr.stAttrJpeg.u32MaxPicHeight = vpe_attrs.u16MaxH;
    attrs.stVeAttr.stAttrJpeg.u32PicWidth = vpe_attrs.u16MaxW;
    attrs.stVeAttr.stAttrJpeg.u32PicHeight = vpe_attrs.u16MaxH;
    attrs.stVeAttr.stAttrJpeg.bByFrame = TRUE;
    attrs.stRcAttr.eRcMode = E_MI_VENC_RC_MODE_MJPEGCBR;
    attrs.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateNum = dst_frame_rate;
    attrs.stRcAttr.stAttrMjpegCbr.u32SrcFrmRateDen = 1;
    attrs.stRcAttr.stAttrMjpegCbr.u32BitRate = 4000000;

    return MI_VENC_CreateChn(0, &attrs);
}

/// Bind a given VENC channel to the VPE.
static int32_t gci_venc_bind_channel(uint32_t dst_frame_rate) {
    MI_SYS_ChnPort_t vpe_output_port;
    MI_SYS_ChnPort_t venc_input_port;
    MI_U32 src_frame_rate;
    MI_U32 venc_dev;
    MI_S32 ret;

    if ((ret = gci_snr_get_fps(&src_frame_rate)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VENC_GetChnDevid(0, &venc_dev)) != MI_SUCCESS) {
        return ret;
    }

    memset(&vpe_output_port, 0, sizeof(vpe_output_port));
    memset(&venc_input_port, 0, sizeof(venc_input_port));

    vpe_output_port.eModId = E_MI_MODULE_ID_VPE;
    vpe_output_port.u32DevId = 0;
    vpe_output_port.u32ChnId = 0;
    vpe_output_port.u32PortId = 0;

    venc_input_port.eModId = E_MI_MODULE_ID_VENC;
    venc_input_port.u32DevId = venc_dev;
    venc_input_port.u32ChnId = 0;
    venc_input_port.u32PortId = 0;

    return MI_SYS_BindChnPort2(
        &vpe_output_port,
        &venc_input_port,
        src_frame_rate,
        dst_frame_rate,
        E_MI_SYS_BIND_TYPE_FRAME_BASE,
        0);
}

/// Unbind a given VENC channel.
static int32_t gci_venc_unbind_channel() {
    MI_SYS_ChnPort_t vpe_output_port;
    MI_SYS_ChnPort_t venc_input_port;
    MI_U32 venc_dev;
    MI_S32 ret;

    if ((ret = MI_VENC_GetChnDevid(0, &venc_dev)) != MI_SUCCESS) {
        return ret;
    }

    memset(&vpe_output_port, 0, sizeof(vpe_output_port));
    memset(&venc_input_port, 0, sizeof(venc_input_port));

    vpe_output_port.eModId = E_MI_MODULE_ID_VPE;
    vpe_output_port.u32DevId = 0;
    vpe_output_port.u32ChnId = 0;
    vpe_output_port.u32PortId = 0;

    venc_input_port.eModId = E_MI_MODULE_ID_VENC;
    venc_input_port.u32DevId = venc_dev;
    venc_input_port.u32ChnId = 0;
    venc_input_port.u32PortId = 0;

    return MI_SYS_UnBindChnPort(&vpe_output_port, &venc_input_port);
}

/// Initialize an encoder channel and bind it to VPE.
int32_t gci_venc_init(uint32_t dst_frame_rate) {
    MI_S32 ret;

    if ((ret = gci_venc_init_mjpeg(dst_frame_rate)) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VENC_StartRecvPic(0)) != MI_SUCCESS) {
        goto destroy_channel;
    }

    if ((ret = gci_venc_bind_channel(dst_frame_rate)) != MI_SUCCESS) {
        goto stop_channel;
    }

    return MI_SUCCESS;

stop_channel:
    MI_VENC_StopRecvPic(0);

destroy_channel:
    MI_VENC_DestroyChn(0);

    return ret;
}

/// Deinitialize a given encoder channel.
int32_t gci_venc_deinit(void) {
    MI_S32 ret;

    if ((ret = gci_venc_unbind_channel()) != MI_SUCCESS) {
        return ret;
    }

    if ((ret = MI_VENC_StopRecvPic(0)) != MI_SUCCESS) {
        return ret;
    }

    return MI_VENC_DestroyChn(0);
}

static int32_t poll_fd(int32_t fd, int timeout) {
    struct pollfd fds;
    int ret;

    fds.fd = fd;
    fds.events = POLLIN;

    ret = poll(&fds, 1, timeout);

    if (ret == 0) {
        return ETIMEDOUT;
    } else if (ret < 0) {
        return errno;
    }

    return 0;
}

static int32_t read_venc_frame(void) {
    MI_VENC_ChnStat_t stat;
    MI_VENC_Stream_t stream;
    MI_VENC_Pack_t* packet;
    MI_S32 ret;
    MI_U32 i;
    MI_S32 fd;
    FILE* fout;

    if ((fout = fopen("snapshot.jpg", "wb")) == NULL) {
        printf("  unable to open output file\n");
    }

    fd = MI_VENC_GetFd(0);

    printf("  waiting for a frame...\n");
    ret = poll_fd(fd, 2000);
    if (ret == ETIMEDOUT) {
        printf("  no frame received in 2000ms\n");
        return MI_SUCCESS;
    } else if (ret != 0) {
        printf("  poll error\n");
        return MI_SUCCESS;
    }

    if ((ret = MI_VENC_Query(0, &stat)) != MI_SUCCESS) {
        return ret;
    }

    if (stat.u32CurPacks == 0) {
        return MI_SUCCESS;
    }

    memset(&stream, 0, sizeof(stream));

    stream.u32PackCount = stat.u32CurPacks;
    stream.pstPack = malloc(sizeof(MI_VENC_Pack_t) * stat.u32CurPacks);

    if (!stream.pstPack) {
        return MI_ERR_VENC_NOMEM;
    }

    if ((ret = MI_VENC_GetStream(0, &stream, 0)) != MI_SUCCESS) {
        goto end;
    }

    if (stream.u32PackCount) {
        printf("  frame #%u\n", stream.u32Seq);
        printf("  frame pts: %llu\n", stream.pstPack[0].u64PTS);
        printf("  packets: %u\n", stream.u32PackCount);
    }

    for (i = 0; i < stream.u32PackCount; i++) {
        packet = &stream.pstPack[i];

        printf("  packet #%u size: %u\n", i, packet->u32Len - packet->u32Offset);
        fwrite(packet->pu8Addr + packet->u32Offset, 1, packet->u32Len - packet->u32Offset, fout);
    }

    ret = MI_VENC_ReleaseStream(0, &stream);

end:
    free(stream.pstPack);
    fclose(fout);

    return ret;
}

int main(void) {
    MI_S32 ret;
    MI_U32 i;

    printf("initializing...\n");
    if ((ret = gci_sys_init()) != MI_SUCCESS) {
        goto end;
    }
    printf("  ok\n");

    printf("initializing video input...\n");
    if ((ret = gci_vif_init()) != MI_SUCCESS) {
        goto deinit_sys;
    }
    printf("  ok\n");

    printf("initializing video processing engine...\n");
    if ((ret = gci_vpe_init()) != MI_SUCCESS) {
        goto deinit_vif;
    }
    printf("  ok\n");

    printf("initializing video encoder...\n");
    if ((ret = gci_venc_init(2)) != MI_SUCCESS) {
        goto deinit_vpe;
    }
    printf("  ok\n");

    // let's read a few frames to give AE and AWB some time to adjust to the
    // scene
    for (i = 0; i < 20; i++) {
        printf("reading a frame...\n");
        if ((ret = read_venc_frame()) != MI_SUCCESS) {
            goto deinit_venc;
        }
        printf("  ok\n");
    }

deinit_venc:
    gci_venc_deinit();

deinit_vpe:
    gci_vpe_deinit();

deinit_vif:
    gci_vif_deinit();

deinit_sys:
    gci_sys_exit();

end:
    if (ret != MI_SUCCESS) {
        printf("  error: %x\n", ret);
    }

    return ret == MI_SUCCESS ? 0 : 1;
}
