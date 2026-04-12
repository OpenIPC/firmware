/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * matter-server.c — Matter 1.2-compatible IP camera server
 *
 */

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

/* POSIX */
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

/* ═══════════════════════════════════════════════════════════════════
 * Matter constants
 * ═══════════════════════════════════════════════════════════════════ */

#define MATTER_PORT_DEFAULT     5540
#define MATTER_MDNS_PORT        5353
#define MATTER_MDNS_GROUP       "224.0.0.251"

#define MATTER_VENDOR_ID        0xFFF1   /* OpenIPC / Test Vendor */
#define MATTER_PRODUCT_ID       0x8001
#define MATTER_DEVICE_TYPE      0x0043   /* Video Camera */
#define MATTER_PASSCODE         20202021
#define MATTER_DISCRIMINATOR    3840

/* Cluster IDs (Matter 1.2 spec) */
#define CLUSTER_BASIC_INFO      0x0028
#define CLUSTER_IDENTIFY        0x0003
#define CLUSTER_ONOFF           0x0006
#define CLUSTER_LEVEL_CONTROL   0x0008
#define CLUSTER_OCCUPANCY       0x0406

/* Attribute IDs */
#define ATTR_ONOFF_ONOFF        0x0000
#define ATTR_LEVEL_CURRENT      0x0000
#define ATTR_BASIC_VENDOR_NAME  0x0001
#define ATTR_BASIC_PRODUCT_NAME 0x0003
#define ATTR_BASIC_NODE_LABEL   0x0005
#define ATTR_BASIC_HW_VER_STR   0x000D
#define ATTR_BASIC_SW_VER_STR   0x000F
#define ATTR_OCC_OCCUPANCY      0x0000

/* Command IDs */
#define CMD_ONOFF_ON            0x01
#define CMD_ONOFF_OFF           0x00
#define CMD_ONOFF_TOGGLE        0x02
#define CMD_LEVEL_MOVE_TO_LEVEL 0x00
#define CMD_IDENTIFY_IDENTIFY   0x00

/* Buffers */
#define MAX_CLIENTS             16
#define RECV_BUF_SIZE           4096
#define SEND_BUF_SIZE           4096
#define RTSP_URL_MAX            256

/* 4-byte LE uint32 length prefix for TCP framing */
#define MATTER_MSG_LEN_PREFIX   4

/* ═══════════════════════════════════════════════════════════════════
 * Minimal TLV codec (Matter uses TLV encoding)
 * ═══════════════════════════════════════════════════════════════════ */

/* TLV types (simplified) */
#define TLV_TYPE_UINT8          0x04
#define TLV_TYPE_UINT16         0x05
#define TLV_TYPE_UINT32         0x06
#define TLV_TYPE_BOOL           0x08
#define TLV_TYPE_UTF8_1         0x0C
#define TLV_TYPE_STRUCT         0x15
#define TLV_TYPE_END_OF_CONT    0x18

typedef struct {
    uint8_t *buf;
    size_t   pos;
    size_t   cap;
} TlvWriter;

static void tlv_init(TlvWriter *w, uint8_t *buf, size_t cap) {
    w->buf = buf;
    w->pos = 0;
    w->cap = cap;
}

static bool tlv_put_uint8(TlvWriter *w, uint8_t tag, uint8_t val) {
    if (tag > 0x1F) return false;
    if (w->pos + 2 > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_UINT8;
    w->buf[w->pos++] = val;
    return true;
}

static bool tlv_put_uint16(TlvWriter *w, uint8_t tag, uint16_t val) {
    if (tag > 0x1F) return false;
    if (w->pos + 3 > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_UINT16;
    w->buf[w->pos++] = val & 0xFF;
    w->buf[w->pos++] = (val >> 8) & 0xFF;
    return true;
}

static bool tlv_put_bool(TlvWriter *w, uint8_t tag, bool val) {
    if (tag > 0x1F) return false;
    if (w->pos + 2 > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_BOOL;
    w->buf[w->pos++] = val ? 0x01 : 0x00;
    return true;
}

static bool tlv_put_uint32(TlvWriter *w, uint8_t tag, uint32_t val) {
    if (tag > 0x1F) return false;
    if (w->pos + 5 > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_UINT32;
    w->buf[w->pos++] = val & 0xFF;
    w->buf[w->pos++] = (val >> 8)  & 0xFF;
    w->buf[w->pos++] = (val >> 16) & 0xFF;
    w->buf[w->pos++] = (val >> 24) & 0xFF;
    return true;
}

static bool tlv_put_utf8(TlvWriter *w, uint8_t tag, const char *str) {
    if (tag > 0x1F) return false;
    size_t len = strlen(str);
    if (len > 255) return false;
    if (w->pos + 3 + len > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_UTF8_1;
    w->buf[w->pos++] = (uint8_t)len;
    memcpy(w->buf + w->pos, str, len);
    w->pos += len;
    return true;
}

static bool tlv_put_struct_begin(TlvWriter *w, uint8_t tag) {
    if (tag > 0x1F) return false;
    if (w->pos + 1 > w->cap) return false;
    w->buf[w->pos++] = (tag << 3) | TLV_TYPE_STRUCT;
    return true;
}

static bool tlv_put_end(TlvWriter *w) {
    if (w->pos + 1 > w->cap) return false;
    w->buf[w->pos++] = TLV_TYPE_END_OF_CONT;
    return true;
}

/* ═══════════════════════════════════════════════════════════════════
 * Matter Message Frame (simplified)
 * ═══════════════════════════════════════════════════════════════════
 *
 * Packet format (minimal, unencrypted — prototype only):
 *
 *  [0]     Flags (0x00 = unicast, unencrypted)
 *  [1-2]   Session ID (little-endian)
 *  [3]     Security flags
 *  [4-7]   Message counter (little-endian)
 *  --- Payload header ---
 *  [N]     Exchange flags
 *  [N+1]   Protocol opcode
 *  [N+2-3] Exchange ID
 *  [N+4-5] Protocol ID (little-endian)
 *  --- TLV payload ---
 */

#define MSG_FLAG_UNENCRYPTED    0x00
#define PROTO_ID_INTERACTION    0x0001  /* Interaction Model */
#define PROTO_ID_SECURE_CHANNEL 0x0000

/* Opcodes — Interaction Model */
#define IM_OP_READ_REQ          0x02
#define IM_OP_REPORT_DATA       0x05
#define IM_OP_WRITE_REQ         0x06
#define IM_OP_WRITE_RESP        0x07
#define IM_OP_INVOKE_REQ        0x08
#define IM_OP_INVOKE_RESP       0x09

/* Opcodes — Secure Channel */
#define SC_OP_PBKDF_PARAM_REQ   0x20
#define SC_OP_PBKDF_PARAM_RESP  0x21
#define SC_OP_PASE_PAKE1        0x22
#define SC_OP_PASE_PAKE2        0x23
#define SC_OP_PASE_PAKE3        0x24
#define SC_OP_STATUS_REPORT     0x40

typedef struct __attribute__((packed)) {
    uint8_t  flags;
    uint16_t session_id;
    uint8_t  security_flags;
    uint32_t msg_counter;
} MatterMsgHeader;

typedef struct __attribute__((packed)) {
    uint8_t  exch_flags;
    uint8_t  opcode;
    uint16_t exchange_id;
    uint16_t protocol_id;
} MatterPayloadHeader;

/* ═══════════════════════════════════════════════════════════════════
 * Device state (clusters)
 * ═══════════════════════════════════════════════════════════════════ */

typedef struct {
    /* OnOff cluster */
    bool     streaming;          /* OnOff::OnOff */

    /* LevelControl cluster */
    uint8_t  brightness;         /* LevelControl::CurrentLevel (0-254) */

    /* OccupancySensing cluster */
    bool     motion_detected;    /* OccupancySensing::Occupancy */

    /* Camera config */
    char     rtsp_url[RTSP_URL_MAX];

    /* BasicInformation */
    char     vendor_name[64];
    char     product_name[64];
    char     node_label[64];
    char     hw_version[32];
    char     sw_version[32];

    /* Statistics */
    uint32_t commands_received;
    uint32_t reads_served;
    time_t   uptime_start;

    pthread_mutex_t lock;
} CameraState;

/* ═══════════════════════════════════════════════════════════════════
 * Global variables
 * ═══════════════════════════════════════════════════════════════════ */

static CameraState          g_camera;
static volatile sig_atomic_t g_running = 1;
static int                  g_server_fd = -1;

static int                  g_active_clients = 0;
static pthread_mutex_t      g_clients_lock   = PTHREAD_MUTEX_INITIALIZER;

/* ═══════════════════════════════════════════════════════════════════
 * Logging
 * ═══════════════════════════════════════════════════════════════════ */

static void log_ts(const char *level, const char *fmt, ...) {
    char      tbuf[32];
    time_t    now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    strftime(tbuf, sizeof(tbuf), "%H:%M:%S", &tm_info);

    fprintf(stdout, "[%s] %-5s | ", tbuf, level);

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);

    fputc('\n', stdout);
    fflush(stdout);
}

#define LOG_INFO(...)  log_ts("INFO",  __VA_ARGS__)
#define LOG_WARN(...)  log_ts("WARN",  __VA_ARGS__)
#define LOG_ERR(...)   log_ts("ERROR", __VA_ARGS__)
#define LOG_CMD(...)   log_ts("CMD",   __VA_ARGS__)
#define LOG_EVENT(...) log_ts("EVENT", __VA_ARGS__)

/* ═══════════════════════════════════════════════════════════════════
 * Camera state initialization
 * ═══════════════════════════════════════════════════════════════════ */

static void camera_state_init(CameraState *cam, const char *rtsp_url) {
    memset(cam, 0, sizeof(*cam));

    cam->streaming       = false;
    cam->brightness      = 127;   /* 50% initial brightness */
    cam->motion_detected = false;
    cam->uptime_start    = time(NULL);

    strncpy(cam->rtsp_url, rtsp_url, RTSP_URL_MAX - 1);

    /* Each string field is populated from the corresponding environment
     * variable when set and non-empty, otherwise a compiled-in default
     * is used.  The init script exports all MATTER_* variables from
     * /etc/matter.conf before starting the daemon, so this is the primary
     * runtime configuration mechanism for embedded targets. */
#define ENV_STR(dst, var, def) do {                                 \
        const char *_e = getenv(var);                               \
        strncpy((dst), (_e && *_e) ? _e : (def), sizeof(dst) - 1); \
    } while (0)

    ENV_STR(cam->vendor_name,  "MATTER_VENDOR_NAME",  "OpenIPC");
    ENV_STR(cam->product_name, "MATTER_PRODUCT_NAME", "OpenIPC IP Camera");
    ENV_STR(cam->node_label,   "MATTER_NODE_LABEL",   "IP Camera");
    ENV_STR(cam->hw_version,   "MATTER_HW_VERSION",   "HW-1.0");
    ENV_STR(cam->sw_version,   "MATTER_SW_VERSION",   "SW-1.0.0");

#undef ENV_STR

    pthread_mutex_init(&cam->lock, NULL);
}

/* ═══════════════════════════════════════════════════════════════════
 * Cluster command handlers
 * ═══════════════════════════════════════════════════════════════════ */

static void handle_onoff_command(CameraState *cam, uint8_t cmd_id,
                                 const struct sockaddr_in *src) {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src->sin_addr, addr_str, sizeof(addr_str));

    bool    new_state = false;
    bool    unknown   = false;
    char    rtsp_copy[RTSP_URL_MAX] = {0};

    pthread_mutex_lock(&cam->lock);
    cam->commands_received++;
    switch (cmd_id) {
    case CMD_ONOFF_ON:
        cam->streaming = true;
        new_state = true;
        snprintf(rtsp_copy, sizeof(rtsp_copy), "%s", cam->rtsp_url);
        break;
    case CMD_ONOFF_OFF:
        cam->streaming = false;
        new_state = false;
        break;
    case CMD_ONOFF_TOGGLE:
        cam->streaming = !cam->streaming;
        new_state = cam->streaming;
        break;
    default:
        unknown = true;
        break;
    }
    pthread_mutex_unlock(&cam->lock);

    /* Log after releasing the lock */
    if (unknown) {
        LOG_WARN("OnOff: unknown command 0x%02X", cmd_id);
    } else if (cmd_id == CMD_ONOFF_ON) {
        LOG_CMD("OnOff::On  — stream started [client=%s]", addr_str);
        LOG_INFO("  RTSP: %s", rtsp_copy);
    } else if (cmd_id == CMD_ONOFF_OFF) {
        LOG_CMD("OnOff::Off — stream stopped [client=%s]", addr_str);
    } else {
        LOG_CMD("OnOff::Toggle — stream %s [client=%s]",
                new_state ? "started" : "stopped", addr_str);
    }
}

static void handle_level_command(CameraState *cam, uint8_t level,
                                 const struct sockaddr_in *src) {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src->sin_addr, addr_str, sizeof(addr_str));

    pthread_mutex_lock(&cam->lock);
    cam->brightness = level;
    cam->commands_received++;
    pthread_mutex_unlock(&cam->lock);

    LOG_CMD("LevelControl::MoveToLevel — brightness=%u/254 (%.1f%%) [client=%s]",
            level, (level / 254.0f) * 100.0f, addr_str);
}

static void handle_identify_command(CameraState *cam,
                                    const struct sockaddr_in *src) {
    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &src->sin_addr, addr_str, sizeof(addr_str));

    pthread_mutex_lock(&cam->lock);
    cam->commands_received++;
    pthread_mutex_unlock(&cam->lock);

    LOG_CMD("Identify — camera is blinking indicator [client=%s]", addr_str);
}

/* ═══════════════════════════════════════════════════════════════════
 * Response packet builders
 * ═══════════════════════════════════════════════════════════════════ */

static ssize_t build_invoke_response(uint8_t *out, size_t outsz,
                                     uint16_t exchange_id,
                                     uint32_t msg_counter,
                                     bool success) {
    if (outsz < sizeof(MatterMsgHeader) + sizeof(MatterPayloadHeader) + 16)
        return -1;

    MatterMsgHeader *mh = (MatterMsgHeader *)out;
    mh->flags          = MSG_FLAG_UNENCRYPTED;
    mh->session_id     = 0x0000;
    mh->security_flags = 0x00;
    mh->msg_counter    = msg_counter + 1;

    MatterPayloadHeader *ph = (MatterPayloadHeader *)(out + sizeof(*mh));
    ph->exch_flags  = 0x01; /* IsInitiator=false */
    ph->opcode      = IM_OP_INVOKE_RESP;
    ph->exchange_id = exchange_id;
    ph->protocol_id = PROTO_ID_INTERACTION;

    uint8_t *tlv     = out + sizeof(*mh) + sizeof(*ph);
    size_t   tlv_cap = outsz - sizeof(*mh) - sizeof(*ph);

    TlvWriter w;
    tlv_init(&w, tlv, tlv_cap);
    tlv_put_struct_begin(&w, 0);  /* InvokeResponseMessage */
        tlv_put_bool(&w, 0, false);   /* SuppressResponse */
        tlv_put_struct_begin(&w, 1);  /* InvokeResponses[] */
            tlv_put_struct_begin(&w, 0); /* CommandStatus */
                tlv_put_uint8(&w, 0, success ? 0x00 : 0x01); /* Status */
            tlv_put_end(&w);
        tlv_put_end(&w);
    tlv_put_end(&w);

    return (ssize_t)(sizeof(*mh) + sizeof(*ph) + w.pos);
}

static ssize_t build_read_response(uint8_t *out, size_t outsz,
                                   uint16_t exchange_id,
                                   uint32_t msg_counter,
                                   uint16_t cluster_id,
                                   uint16_t attr_id,
                                   CameraState *cam) {
    if (outsz < sizeof(MatterMsgHeader) + sizeof(MatterPayloadHeader) + 128)
        return -1;

    MatterMsgHeader *mh = (MatterMsgHeader *)out;
    mh->flags          = MSG_FLAG_UNENCRYPTED;
    mh->session_id     = 0x0000;
    mh->security_flags = 0x00;
    mh->msg_counter    = msg_counter + 1;

    MatterPayloadHeader *ph = (MatterPayloadHeader *)(out + sizeof(*mh));
    ph->exch_flags  = 0x00;
    ph->opcode      = IM_OP_REPORT_DATA;
    ph->exchange_id = exchange_id;
    ph->protocol_id = PROTO_ID_INTERACTION;

    uint8_t *tlv     = out + sizeof(*mh) + sizeof(*ph);
    size_t   tlv_cap = outsz - sizeof(*mh) - sizeof(*ph);

    TlvWriter w;
    tlv_init(&w, tlv, tlv_cap);
    tlv_put_struct_begin(&w, 0); /* ReportDataMessage */
        tlv_put_struct_begin(&w, 1); /* AttributeReportIBs */
            tlv_put_struct_begin(&w, 0); /* AttributeReportIB */
                tlv_put_struct_begin(&w, 0); /* AttributeDataIB */
                    /* AttributePath */
                    tlv_put_struct_begin(&w, 0);
                        tlv_put_uint16(&w, 3, cluster_id);
                        tlv_put_uint16(&w, 4, attr_id);
                    tlv_put_end(&w);
                    /* Data */
                    pthread_mutex_lock(&cam->lock);
                    if (cluster_id == CLUSTER_ONOFF && attr_id == ATTR_ONOFF_ONOFF) {
                        tlv_put_bool(&w, 1, cam->streaming);
                    } else if (cluster_id == CLUSTER_LEVEL_CONTROL &&
                               attr_id == ATTR_LEVEL_CURRENT) {
                        tlv_put_uint8(&w, 1, cam->brightness);
                    } else if (cluster_id == CLUSTER_OCCUPANCY &&
                               attr_id == ATTR_OCC_OCCUPANCY) {
                        tlv_put_uint8(&w, 1, cam->motion_detected ? 1 : 0);
                    } else if (cluster_id == CLUSTER_BASIC_INFO) {
                        switch (attr_id) {
                        case ATTR_BASIC_VENDOR_NAME:
                            tlv_put_utf8(&w, 1, cam->vendor_name); break;
                        case ATTR_BASIC_PRODUCT_NAME:
                            tlv_put_utf8(&w, 1, cam->product_name); break;
                        case ATTR_BASIC_NODE_LABEL:
                            tlv_put_utf8(&w, 1, cam->node_label); break;
                        case ATTR_BASIC_HW_VER_STR:
                            tlv_put_utf8(&w, 1, cam->hw_version); break;
                        case ATTR_BASIC_SW_VER_STR:
                            tlv_put_utf8(&w, 1, cam->sw_version); break;
                        default:
                            tlv_put_uint8(&w, 1, 0);
                        }
                    } else {
                        tlv_put_uint8(&w, 1, 0xFF); /* Unknown */
                    }
                    cam->reads_served++;
                    pthread_mutex_unlock(&cam->lock);
                tlv_put_end(&w); /* AttributeDataIB */
            tlv_put_end(&w); /* AttributeReportIB */
        tlv_put_end(&w); /* AttributeReportIBs */
    tlv_put_end(&w); /* ReportDataMessage */

    return (ssize_t)(sizeof(*mh) + sizeof(*ph) + w.pos);
}

/* ═══════════════════════════════════════════════════════════════════
 * TCP framing helper
 * ═══════════════════════════════════════════════════════════════════ */

/* Send exactly 'len' bytes with a 4-byte LE length prefix. Returns 0 on success, -1 on error. */
static int send_framed(int fd, const uint8_t *buf, size_t len) {
    uint8_t prefix[4];
    prefix[0] = (uint8_t)(len         & 0xFF);
    prefix[1] = (uint8_t)((len >>  8) & 0xFF);
    prefix[2] = (uint8_t)((len >> 16) & 0xFF);
    prefix[3] = (uint8_t)((len >> 24) & 0xFF);

    /* Send prefix */
    size_t sent = 0;
    while (sent < 4) {
        ssize_t n = send(fd, prefix + sent, 4 - sent, MSG_NOSIGNAL);
        if (n <= 0) return -1;
        sent += (size_t)n;
    }
    /* Send payload */
    sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
        if (n <= 0) return -1;
        sent += (size_t)n;
    }
    return 0;
}

/* ═══════════════════════════════════════════════════════════════════
 * Incoming packet parser and dispatcher
 * ═══════════════════════════════════════════════════════════════════ */

static void dispatch_packet(int client_fd,
                            const uint8_t *buf, ssize_t len,
                            const struct sockaddr_in *src) {
    if (len < (ssize_t)(sizeof(MatterMsgHeader) + sizeof(MatterPayloadHeader))) {
        LOG_WARN("Packet too short (%zd bytes)", len);
        return;
    }

    const MatterMsgHeader     *mh = (const MatterMsgHeader *)buf;
    const MatterPayloadHeader *ph =
        (const MatterPayloadHeader *)(buf + sizeof(*mh));

    uint8_t  resp_buf[SEND_BUF_SIZE];
    ssize_t  resp_len = -1;

    /* Secure Channel — simplified PASE stub */
    if (ph->protocol_id == PROTO_ID_SECURE_CHANNEL) {
        if (ph->opcode == SC_OP_PBKDF_PARAM_REQ) {
            LOG_INFO("PASE: received PBKDFParamRequest — responding with PBKDFParamResponse");
            MatterMsgHeader *rmh = (MatterMsgHeader *)resp_buf;
            rmh->flags          = MSG_FLAG_UNENCRYPTED;
            rmh->session_id     = 0x0000;
            rmh->security_flags = 0x00;
            rmh->msg_counter    = mh->msg_counter + 1;

            MatterPayloadHeader *rph =
                (MatterPayloadHeader *)(resp_buf + sizeof(*rmh));
            rph->exch_flags  = 0x00;
            rph->opcode      = SC_OP_STATUS_REPORT;
            rph->exchange_id = ph->exchange_id;
            rph->protocol_id = PROTO_ID_SECURE_CHANNEL;

            uint8_t *tlv = resp_buf + sizeof(*rmh) + sizeof(*rph);
            TlvWriter w;
            tlv_init(&w, tlv, sizeof(resp_buf) - sizeof(*rmh) - sizeof(*rph));
            tlv_put_uint16(&w, 0, 0x0000);     /* GeneralCode SUCCESS */
            tlv_put_uint32(&w, 1, 0x00000000); /* ProtocolId */
            tlv_put_uint16(&w, 2, 0x0000);     /* ProtocolCode */

            resp_len = (ssize_t)(sizeof(*rmh) + sizeof(*rph) + w.pos);
        }
        goto send_response;
    }

    /* Interaction Model */
    if (ph->protocol_id != PROTO_ID_INTERACTION) {
        LOG_WARN("Unknown protocol_id=0x%04X", ph->protocol_id);
        return;
    }

    switch (ph->opcode) {

    /* ── InvokeRequest ────────────────────────────────────────────── */
    case IM_OP_INVOKE_REQ: {
        const uint8_t *payload = buf + sizeof(*mh) + sizeof(*ph);
        ssize_t plen = len - (ssize_t)(sizeof(*mh) + sizeof(*ph));

        uint16_t cluster_id = 0xFFFF;
        uint8_t  command_id = 0xFF;
        uint8_t  level_val  = 127;

        for (ssize_t i = 0; i + 1 < plen; i++) {
            uint16_t w16 = (uint16_t)(payload[i] | (payload[i+1] << 8));
            if (w16 == CLUSTER_ONOFF ||
                w16 == CLUSTER_LEVEL_CONTROL ||
                w16 == CLUSTER_IDENTIFY) {
                cluster_id = w16;
                if (i + 2 < plen) command_id = payload[i + 2];
                if (cluster_id == CLUSTER_LEVEL_CONTROL && i + 3 < plen)
                    level_val = payload[i + 3];
                break;
            }
        }

        bool ok = true;
        if (cluster_id == CLUSTER_ONOFF) {
            handle_onoff_command(&g_camera, command_id, src);
        } else if (cluster_id == CLUSTER_LEVEL_CONTROL) {
            handle_level_command(&g_camera, level_val, src);
        } else if (cluster_id == CLUSTER_IDENTIFY) {
            handle_identify_command(&g_camera, src);
        } else {
            LOG_WARN("InvokeReq: unknown cluster=0x%04X", cluster_id);
            ok = false;
        }

        resp_len = build_invoke_response(resp_buf, sizeof(resp_buf),
                                         ph->exchange_id,
                                         mh->msg_counter, ok);
        break;
    }

    /* ── ReadRequest ──────────────────────────────────────────────── */
    case IM_OP_READ_REQ: {
        const uint8_t *payload = buf + sizeof(*mh) + sizeof(*ph);
        ssize_t plen = len - (ssize_t)(sizeof(*mh) + sizeof(*ph));

        uint16_t cluster_id = CLUSTER_ONOFF;
        uint16_t attr_id    = ATTR_ONOFF_ONOFF;

        for (ssize_t i = 0; i + 3 < plen; i++) {
            uint16_t cid = (uint16_t)(payload[i]   | (payload[i+1] << 8));
            uint16_t aid = (uint16_t)(payload[i+2] | (payload[i+3] << 8));
            if (cid == CLUSTER_ONOFF         ||
                cid == CLUSTER_LEVEL_CONTROL ||
                cid == CLUSTER_OCCUPANCY     ||
                cid == CLUSTER_BASIC_INFO) {
                cluster_id = cid;
                attr_id    = aid;
                break;
            }
        }

        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src->sin_addr, addr_str, sizeof(addr_str));
        LOG_INFO("ReadReq cluster=0x%04X attr=0x%04X [client=%s]",
                 cluster_id, attr_id, addr_str);

        resp_len = build_read_response(resp_buf, sizeof(resp_buf),
                                       ph->exchange_id,
                                       mh->msg_counter,
                                       cluster_id, attr_id,
                                       &g_camera);
        break;
    }

    /* ── WriteRequest ─────────────────────────────────────────────── */
    case IM_OP_WRITE_REQ: {
        const uint8_t *payload = buf + sizeof(*mh) + sizeof(*ph);
        ssize_t plen = len - (ssize_t)(sizeof(*mh) + sizeof(*ph));

        uint16_t cluster_id = 0xFFFF;
        uint16_t attr_id    = 0xFFFF;

        /* Primitive scan for cluster + attribute IDs */
        for (ssize_t i = 0; i + 3 < plen; i++) {
            uint16_t cid = (uint16_t)(payload[i]   | (payload[i+1] << 8));
            uint16_t aid = (uint16_t)(payload[i+2] | (payload[i+3] << 8));
            if (cid == CLUSTER_ONOFF         ||
                cid == CLUSTER_LEVEL_CONTROL ||
                cid == CLUSTER_BASIC_INFO) {
                cluster_id = cid;
                attr_id    = aid;
                break;
            }
        }

        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src->sin_addr, addr_str, sizeof(addr_str));

        bool write_ok = false;
        if (cluster_id == CLUSTER_ONOFF && attr_id == ATTR_ONOFF_ONOFF) {
            for (ssize_t i = 0; i + 4 < plen; i++) {
                uint16_t cid = (uint16_t)(payload[i] | (payload[i+1] << 8));
                uint16_t aid = (uint16_t)(payload[i+2] | (payload[i+3] << 8));
                if (cid == CLUSTER_ONOFF && aid == ATTR_ONOFF_ONOFF) {
                    bool val = (payload[i+4] != 0);
                    pthread_mutex_lock(&g_camera.lock);
                    g_camera.streaming = val;
                    pthread_mutex_unlock(&g_camera.lock);
                    LOG_CMD("WriteAttr OnOff::OnOff = %s [client=%s]",
                            val ? "true" : "false", addr_str);
                    write_ok = true;
                    break;
                }
            }
        } else if (cluster_id == CLUSTER_LEVEL_CONTROL && attr_id == ATTR_LEVEL_CURRENT) {
            for (ssize_t i = 0; i + 4 < plen; i++) {
                uint16_t cid = (uint16_t)(payload[i] | (payload[i+1] << 8));
                uint16_t aid = (uint16_t)(payload[i+2] | (payload[i+3] << 8));
                if (cid == CLUSTER_LEVEL_CONTROL && aid == ATTR_LEVEL_CURRENT) {
                    uint8_t val = payload[i+4];
                    pthread_mutex_lock(&g_camera.lock);
                    g_camera.brightness = val;
                    pthread_mutex_unlock(&g_camera.lock);
                    LOG_CMD("WriteAttr LevelControl::CurrentLevel = %u [client=%s]",
                            val, addr_str);
                    write_ok = true;
                    break;
                }
            }
        } else if (cluster_id == CLUSTER_BASIC_INFO && attr_id == ATTR_BASIC_NODE_LABEL) {
            for (ssize_t i = 0; i + 5 < plen; i++) {
                uint16_t cid = (uint16_t)(payload[i] | (payload[i+1] << 8));
                uint16_t aid = (uint16_t)(payload[i+2] | (payload[i+3] << 8));
                if (cid == CLUSTER_BASIC_INFO && aid == ATTR_BASIC_NODE_LABEL) {
                    uint8_t slen = payload[i+4];
                    if ((ssize_t)(i + 5 + slen) <= plen) {
                        size_t copy_len = (slen < (uint8_t)(sizeof(g_camera.node_label) - 1))
                                        ? slen
                                        : (uint8_t)(sizeof(g_camera.node_label) - 1);
                        pthread_mutex_lock(&g_camera.lock);
                        memcpy(g_camera.node_label, payload + i + 5, copy_len);
                        g_camera.node_label[sizeof(g_camera.node_label) - 1] = '\0';
                        pthread_mutex_unlock(&g_camera.lock);
                        LOG_CMD("WriteAttr BasicInfo::NodeLabel = \"%.*s\" [client=%s]",
                                (int)slen, payload + i + 5, addr_str);
                        write_ok = true;
                    }
                    break;
                }
            }
        } else {
            LOG_WARN("WriteReq: unsupported cluster=0x%04X attr=0x%04X [client=%s]",
                     cluster_id, attr_id, addr_str);
        }

        /* Build WriteResponse */
        MatterMsgHeader *rmh = (MatterMsgHeader *)resp_buf;
        rmh->flags          = MSG_FLAG_UNENCRYPTED;
        rmh->session_id     = 0x0000;
        rmh->security_flags = 0x00;
        rmh->msg_counter    = mh->msg_counter + 1;

        MatterPayloadHeader *rph =
            (MatterPayloadHeader *)(resp_buf + sizeof(*rmh));
        rph->exch_flags  = 0x01;
        rph->opcode      = IM_OP_WRITE_RESP;
        rph->exchange_id = ph->exchange_id;
        rph->protocol_id = PROTO_ID_INTERACTION;

        uint8_t *tlv = resp_buf + sizeof(*rmh) + sizeof(*rph);
        TlvWriter ww;
        tlv_init(&ww, tlv, sizeof(resp_buf) - sizeof(*rmh) - sizeof(*rph));
        tlv_put_struct_begin(&ww, 0); /* WriteResponseMessage */
            tlv_put_struct_begin(&ww, 0); /* WriteResponses[] */
                tlv_put_uint8(&ww, 0, write_ok ? 0x00 : 0x01); /* Status */
            tlv_put_end(&ww);
        tlv_put_end(&ww);

        resp_len = (ssize_t)(sizeof(*rmh) + sizeof(*rph) + ww.pos);
        break;
    }

    default:
        LOG_WARN("IM: unknown opcode=0x%02X", ph->opcode);
        return;
    }

send_response:
    if (resp_len > 0) {
        if (send_framed(client_fd, resp_buf, (size_t)resp_len) < 0)
            LOG_ERR("send_framed() failed: %s", strerror(errno));
    }
}

/* ═══════════════════════════════════════════════════════════════════
 * Client connection thread
 * ═══════════════════════════════════════════════════════════════════ */

typedef struct {
    int                fd;
    struct sockaddr_in addr;
} ClientThreadArg;

static void *client_thread(void *arg) {
    ClientThreadArg *cta = (ClientThreadArg *)arg;
    int                fd   = cta->fd;
    struct sockaddr_in addr = cta->addr;
    free(cta);

    char addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, addr_str, sizeof(addr_str));
    LOG_INFO("New connection: %s:%d", addr_str, ntohs(addr.sin_port));

    /* Accumulation buffer for TCP stream reassembly */
    uint8_t acc[RECV_BUF_SIZE * 2];
    size_t  acc_len = 0;

    while (g_running) {
        ssize_t n = recv(fd, acc + acc_len, sizeof(acc) - acc_len, 0);
        if (n <= 0) {
            if (n == 0)
                LOG_INFO("Client disconnected: %s", addr_str);
            else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
                LOG_ERR("recv() error [%s]: %s", addr_str, strerror(errno));
            break;
        }
        acc_len += (size_t)n;

        /* Process all complete framed messages in the accumulation buffer */
        while (acc_len >= MATTER_MSG_LEN_PREFIX) {
            /* Read 4-byte LE message length */
            uint32_t msg_len = (uint32_t)acc[0]
                             | ((uint32_t)acc[1] <<  8)
                             | ((uint32_t)acc[2] << 16)
                             | ((uint32_t)acc[3] << 24);

            /* Sanity check on message length */
            if (msg_len == 0 || msg_len > RECV_BUF_SIZE) {
                LOG_WARN("Invalid framed message length %u from %s — closing",
                         msg_len, addr_str);
                goto client_done;
            }

            /* Wait until we have the full message */
            if (acc_len < MATTER_MSG_LEN_PREFIX + (size_t)msg_len)
                break;

            /* Dispatch the complete message */
            dispatch_packet(fd,
                            acc + MATTER_MSG_LEN_PREFIX,
                            (ssize_t)msg_len,
                            &addr);

            /* Consume this message from the accumulation buffer */
            size_t consumed = MATTER_MSG_LEN_PREFIX + (size_t)msg_len;
            memmove(acc, acc + consumed, acc_len - consumed);
            acc_len -= consumed;
        }
    }

client_done:
    close(fd);
    pthread_mutex_lock(&g_clients_lock);
    g_active_clients--;
    pthread_mutex_unlock(&g_clients_lock);
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════
 * mDNS announcement (_matter._tcp) — PTR + SRV + TXT records
 * ═══════════════════════════════════════════════════════════════════ */

/* Build a minimal mDNS response with PTR + SRV + TXT records for Matter commissioning.
 * Returns the number of bytes written to buf, or 0 on error. */
static size_t build_mdns_packet(uint8_t *buf, size_t bufsz, int port) {
    if (bufsz < 256) return 0;
    size_t p = 0;

#define PUT8(v)  do { if (p >= bufsz) return 0; buf[p++] = (uint8_t)(v); } while(0)
#define PUT16(v) do { PUT8((unsigned)(v) >> 8); PUT8((unsigned)(v) & 0xFF); } while(0)
#define PUT32(v) do { PUT16((unsigned long)(v) >> 16); PUT16((unsigned long)(v) & 0xFFFF); } while(0)

    /* DNS Header (12 bytes) */
    PUT16(0x0000);  /* Transaction ID */
    PUT16(0x8400);  /* Flags: QR=1 Response, AA=1 */
    PUT16(0);       /* Questions */
    PUT16(3);       /* Answer RRs: PTR + SRV + TXT */
    PUT16(0);       /* Authority RRs */
    PUT16(0);       /* Additional RRs */

    /* === PTR Record ===
     * Name:  _matter._tcp.local
     * RData: OpenIPC-Camera._matter._tcp.local */
    PUT8(7);  memcpy(buf+p, "_matter", 7); p += 7;
    PUT8(4);  memcpy(buf+p, "_tcp",    4); p += 4;
    PUT8(5);  memcpy(buf+p, "local",   5); p += 5;
    PUT8(0);
    PUT16(0x000C);  /* Type PTR */
    PUT16(0x0001);  /* Class IN */
    PUT32(120);     /* TTL 120s */
    /* Placeholder for RDLENGTH — fill in after writing rdata */
    size_t rdlen_pos = p;
    PUT16(0);
    size_t rdata_start = p;
    PUT8(14); memcpy(buf+p, "OpenIPC-Camera", 14); p += 14;
    PUT8(7);  memcpy(buf+p, "_matter",         7); p += 7;
    PUT8(4);  memcpy(buf+p, "_tcp",            4); p += 4;
    PUT8(5);  memcpy(buf+p, "local",           5); p += 5;
    PUT8(0);
    {
        uint16_t rdlen = (uint16_t)(p - rdata_start);
        buf[rdlen_pos]   = (uint8_t)(rdlen >> 8);
        buf[rdlen_pos+1] = (uint8_t)(rdlen & 0xFF);
    }

    /* === SRV Record ===
     * Name:   OpenIPC-Camera._matter._tcp.local
     * RData:  priority=0, weight=0, port=port, target=openipc-cam.local */
    PUT8(14); memcpy(buf+p, "OpenIPC-Camera", 14); p += 14;
    PUT8(7);  memcpy(buf+p, "_matter",         7); p += 7;
    PUT8(4);  memcpy(buf+p, "_tcp",            4); p += 4;
    PUT8(5);  memcpy(buf+p, "local",           5); p += 5;
    PUT8(0);
    PUT16(0x0021);  /* Type SRV */
    PUT16(0x0001);  /* Class IN */
    PUT32(120);     /* TTL */
    rdlen_pos   = p;
    PUT16(0);
    rdata_start = p;
    PUT16(0);                /* Priority */
    PUT16(0);                /* Weight */
    PUT16((uint16_t)port);   /* Port */
    PUT8(11); memcpy(buf+p, "openipc-cam", 11); p += 11;
    PUT8(5);  memcpy(buf+p, "local",        5); p += 5;
    PUT8(0);
    {
        uint16_t rdlen = (uint16_t)(p - rdata_start);
        buf[rdlen_pos]   = (uint8_t)(rdlen >> 8);
        buf[rdlen_pos+1] = (uint8_t)(rdlen & 0xFF);
    }

    /* === TXT Record ===
     * Name:   OpenIPC-Camera._matter._tcp.local
     * RData:  CM=1, D=<discriminator>, VP=<vendor>+<product>, DT=<type> */
    PUT8(14); memcpy(buf+p, "OpenIPC-Camera", 14); p += 14;
    PUT8(7);  memcpy(buf+p, "_matter",         7); p += 7;
    PUT8(4);  memcpy(buf+p, "_tcp",            4); p += 4;
    PUT8(5);  memcpy(buf+p, "local",           5); p += 5;
    PUT8(0);
    PUT16(0x0010);  /* Type TXT */
    PUT16(0x0001);  /* Class IN */
    PUT32(120);     /* TTL */
    rdlen_pos   = p;
    PUT16(0);
    rdata_start = p;
    /* CM=1 */
    PUT8(4); memcpy(buf+p, "CM=1", 4); p += 4;
    /* D=<discriminator> */
    {
        char tmp[32];
        int n = snprintf(tmp, sizeof(tmp), "D=%d", MATTER_DISCRIMINATOR);
        PUT8((uint8_t)n);
        memcpy(buf+p, tmp, (size_t)n); p += (size_t)n;
    }
    /* VP=<vendor_id>+<product_id> */
    {
        char tmp[32];
        int n = snprintf(tmp, sizeof(tmp), "VP=%u+%u",
                         MATTER_VENDOR_ID, MATTER_PRODUCT_ID);
        PUT8((uint8_t)n);
        memcpy(buf+p, tmp, (size_t)n); p += (size_t)n;
    }
    /* DT=<device_type> */
    {
        char tmp[32];
        int n = snprintf(tmp, sizeof(tmp), "DT=%u", MATTER_DEVICE_TYPE);
        PUT8((uint8_t)n);
        memcpy(buf+p, tmp, (size_t)n); p += (size_t)n;
    }
    {
        uint16_t rdlen = (uint16_t)(p - rdata_start);
        buf[rdlen_pos]   = (uint8_t)(rdlen >> 8);
        buf[rdlen_pos+1] = (uint8_t)(rdlen & 0xFF);
    }

#undef PUT8
#undef PUT16
#undef PUT32

    return p;
}

static void *mdns_thread(void *arg) {
    int port = arg ? *(int *)arg : MATTER_PORT_DEFAULT;
    free(arg);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        LOG_ERR("mDNS: socket() failed: %s", strerror(errno));
        return NULL;
    }

    int ttl = 255;
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(MATTER_MDNS_PORT);
    inet_pton(AF_INET, MATTER_MDNS_GROUP, &dest.sin_addr);

    uint8_t mdns_pkt[512];
    size_t  mdns_pkt_len = build_mdns_packet(mdns_pkt, sizeof(mdns_pkt), port);
    if (mdns_pkt_len == 0) {
        LOG_ERR("mDNS: failed to build announcement packet");
        close(sock);
        return NULL;
    }

    LOG_INFO("mDNS: announcing _matter._tcp.local every 30 seconds");

    while (g_running) {
        ssize_t sent = sendto(sock, mdns_pkt, mdns_pkt_len, 0,
                              (struct sockaddr *)&dest, sizeof(dest));
        if (sent < 0)
            LOG_WARN("mDNS: sendto() failed: %s", strerror(errno));
        else
            LOG_INFO("mDNS: announcement sent (%zd bytes)", sent);

        for (int i = 0; i < 30 && g_running; i++)
            sleep(1);
    }

    close(sock);
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════
 * Motion detection simulation thread
 * ═══════════════════════════════════════════════════════════════════ */

static void *motion_sim_thread(void *arg) {
    unsigned seed = (unsigned)time(NULL) ^ (unsigned)(uintptr_t)arg;

    while (g_running) {
        sleep(15);
        if (!g_running) break;

        pthread_mutex_lock(&g_camera.lock);
        bool prev = g_camera.motion_detected;
        /* 30% probability of motion detection */
        g_camera.motion_detected = ((rand_r(&seed) % 10) < 3);
        bool cur = g_camera.motion_detected;
        pthread_mutex_unlock(&g_camera.lock);

        if (cur != prev) {
            if (cur)
                LOG_EVENT("OccupancySensing: MOTION DETECTED!");
            else
                LOG_EVENT("OccupancySensing: motion stopped");
        }
    }
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════
 * Statistics thread
 * ═══════════════════════════════════════════════════════════════════ */

static void *stats_thread(void *arg) {
    (void)arg;

    while (g_running) {
        for (int i = 0; i < 60 && g_running; i++)
            sleep(1);
        if (!g_running) break;

        pthread_mutex_lock(&g_camera.lock);
        time_t uptime = time(NULL) - g_camera.uptime_start;
        LOG_INFO("── Status ──────────────────────────────");
        LOG_INFO("  Streaming:     %s", g_camera.streaming ? "ON" : "OFF");
        LOG_INFO("  RTSP URL:      %s", g_camera.rtsp_url);
        LOG_INFO("  Brightness:    %u/254",  g_camera.brightness);
        LOG_INFO("  Motion:        %s",  g_camera.motion_detected ? "YES" : "no");
        LOG_INFO("  Commands:      %u",  g_camera.commands_received);
        LOG_INFO("  Read requests: %u",  g_camera.reads_served);
        LOG_INFO("  Uptime:        %ld sec", (long)uptime);
        LOG_INFO("───────────────────────────────────────");
        pthread_mutex_unlock(&g_camera.lock);
    }
    return NULL;
}

/* ═══════════════════════════════════════════════════════════════════
 * Signal handler
 * ═══════════════════════════════════════════════════════════════════ */

static void sig_handler(int sig) {
    (void)sig;
    g_running = 0;
    if (g_server_fd >= 0) {
        shutdown(g_server_fd, SHUT_RDWR);
        close(g_server_fd);
        g_server_fd = -1;
    }
}

/* ═══════════════════════════════════════════════════════════════════
 * main()
 * ═══════════════════════════════════════════════════════════════════ */

int main(int argc, char *argv[]) {
    /* Priority for each parameter: CLI argument > env variable > compiled-in default */

    /* RTSP URL: env fallback */
    const char *env_rtsp = getenv("MATTER_RTSP_URL");
    const char *rtsp_url = (env_rtsp && *env_rtsp)
                           ? env_rtsp
                           : "rtsp://127.0.0.1:554/stream";

    /* Port: env fallback */
    int port = MATTER_PORT_DEFAULT;
    const char *env_port = getenv("MATTER_PORT");
    if (env_port && *env_port) {
        char *ep;
        errno = 0;
        long pv = strtol(env_port, &ep, 10);
        if (errno == 0 && *ep == '\0' && pv >= 1 && pv <= 65535)
            port = (int)pv;
    }

    /* CLI arguments override env */
    if (argc >= 2) rtsp_url = argv[1];
    if (argc >= 3) {
        char *endptr;
        errno = 0;
        long p = strtol(argv[2], &endptr, 10);
        if (errno != 0 || *endptr != '\0' || p < 1 || p > 65535) {
            fprintf(stderr, "Invalid port: %s (must be 1-65535)\n", argv[2]);
            return 1;
        }
        port = (int)p;
    }

    /* Initialization */
    camera_state_init(&g_camera, rtsp_url);

    /* Signals */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGPIPE, &(struct sigaction){ .sa_handler = SIG_IGN }, NULL);

    printf("\n");
    printf("  ┌─────────────────────────────────────────┐\n");
    printf("  │   OpenIPC Matter IP Camera Server v1.1  │\n");
    printf("  ├─────────────────────────────────────────┤\n");
    printf("  │  Vendor ID:     0x%04X                   │\n", MATTER_VENDOR_ID);
    printf("  │  Product ID:    0x%04X                   │\n", MATTER_PRODUCT_ID);
    printf("  │  Device Type:   0x%04X (VideoCamera)    │\n", MATTER_DEVICE_TYPE);
    printf("  │  Discriminator: %-5d                    │\n", MATTER_DISCRIMINATOR);
    printf("  │  Passcode:      %-8d                 │\n", MATTER_PASSCODE);
    printf("  │  Port:          %-5d                    │\n", port);
    printf("  │  RTSP:          %-30s │\n", rtsp_url);
    printf("  └─────────────────────────────────────────┘\n\n");

    /* TCP server */
    g_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_fd < 0) {
        LOG_ERR("socket(): %s", strerror(errno));
        return 1;
    }

    int opt = 1;
    setsockopt(g_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family      = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port        = htons((uint16_t)port);

    if (bind(g_server_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
        LOG_ERR("bind(): %s", strerror(errno));
        return 1;
    }

    if (listen(g_server_fd, MAX_CLIENTS) < 0) {
        LOG_ERR("listen(): %s", strerror(errno));
        return 1;
    }

    LOG_INFO("Matter server listening on port %d/TCP", port);

    /* Start auxiliary threads */
    pthread_t mdns_tid, motion_tid, stats_tid;

    int *mdns_port_arg = malloc(sizeof(int));
    if (!mdns_port_arg) {
        LOG_ERR("malloc(mdns_port_arg): %s", strerror(errno));
        return 1;
    }
    *mdns_port_arg = port;

    if (pthread_create(&mdns_tid, NULL, mdns_thread, mdns_port_arg) != 0) {
        LOG_ERR("pthread_create(mdns): %s", strerror(errno));
        free(mdns_port_arg);
        return 1;
    }
    if (pthread_create(&motion_tid, NULL, motion_sim_thread, NULL) != 0) {
        LOG_ERR("pthread_create(motion): %s", strerror(errno));
        return 1;
    }
    if (pthread_create(&stats_tid, NULL, stats_thread, NULL) != 0) {
        LOG_ERR("pthread_create(stats): %s", strerror(errno));
        return 1;
    }

    pthread_detach(mdns_tid);
    pthread_detach(motion_tid);
    pthread_detach(stats_tid);

    LOG_INFO("Waiting for Matter controllers...");

    /* Main accept() loop */
    while (g_running) {
        struct sockaddr_in cli_addr;
        socklen_t cli_len = sizeof(cli_addr);

        /* select() with timeout to check g_running */
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(g_server_fd, &rfds);
        struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };

        int r = select(g_server_fd + 1, &rfds, NULL, NULL, &tv);
        if (r < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (r == 0) continue; /* timeout */

        int cli_fd = accept(g_server_fd, (struct sockaddr *)&cli_addr, &cli_len);
        if (cli_fd < 0) {
            if (errno == EINTR || errno == EBADF) break;
            LOG_ERR("accept(): %s", strerror(errno));
            continue;
        }

        /* Format client address for potential rejection log */
        char cli_addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &cli_addr.sin_addr, cli_addr_str, sizeof(cli_addr_str));

        /* Check client limit */
        pthread_mutex_lock(&g_clients_lock);
        if (g_active_clients >= MAX_CLIENTS) {
            pthread_mutex_unlock(&g_clients_lock);
            LOG_WARN("Connection limit (%d) reached, rejecting %s",
                     MAX_CLIENTS, cli_addr_str);
            close(cli_fd);
            continue;
        }
        g_active_clients++;
        pthread_mutex_unlock(&g_clients_lock);

        /* Set 30-second receive timeout to avoid hung connections */
        struct timeval rcv_tv = { .tv_sec = 30, .tv_usec = 0 };
        setsockopt(cli_fd, SOL_SOCKET, SO_RCVTIMEO, &rcv_tv, sizeof(rcv_tv));

        /* Spawn a thread for the client */
        ClientThreadArg *cta = malloc(sizeof(*cta));
        if (!cta) {
            pthread_mutex_lock(&g_clients_lock);
            g_active_clients--;
            pthread_mutex_unlock(&g_clients_lock);
            close(cli_fd);
            continue;
        }
        cta->fd   = cli_fd;
        cta->addr = cli_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, cta) != 0) {
            LOG_ERR("pthread_create(): %s", strerror(errno));
            pthread_mutex_lock(&g_clients_lock);
            g_active_clients--;
            pthread_mutex_unlock(&g_clients_lock);
            close(cli_fd);
            free(cta);
        } else {
            pthread_detach(tid);
        }
    }

    LOG_INFO("Server shutting down...");
    pthread_mutex_destroy(&g_camera.lock);
    return 0;
}
