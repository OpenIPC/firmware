/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * herald-client.c — mDNS/DNS-SD service browser (RFC 6762 / RFC 6763)
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <poll.h>
#include <syslog.h>

/* Log to both stdout and syslog. Format strings must NOT include a trailing \n. */
#define log_info(fmt, ...) \
    do { printf(fmt "\n", ##__VA_ARGS__); \
         syslog(LOG_INFO,    fmt, ##__VA_ARGS__); } while(0)
#define log_warn(fmt, ...) \
    do { fprintf(stderr, "WARNING: " fmt "\n", ##__VA_ARGS__); \
         syslog(LOG_WARNING, fmt, ##__VA_ARGS__); } while(0)
#define log_err(fmt, ...) \
    do { fprintf(stderr, "ERROR: " fmt "\n", ##__VA_ARGS__); \
         syslog(LOG_ERR,     fmt, ##__VA_ARGS__); } while(0)

/* ─── mDNS constants ──────────────────────────────────────────────── */
#define MDNS_ADDR        "224.0.0.251"
#define MDNS_PORT        5353
#define HERALD_BUF_SIZE    4096
#define MAX_RESULTS      64
#define MAX_NAME_LEN     256

/* ─── DNS record types ────────────────────────────────────────────── */
#define DNS_TYPE_A       1
#define DNS_TYPE_PTR     12
#define DNS_TYPE_TXT     16
#define DNS_TYPE_SRV     33
#define DNS_CLASS_IN     1

/* ─── Structures ──────────────────────────────────────────────────── */
typedef struct {
    char     name[MAX_NAME_LEN];
    char     target[MAX_NAME_LEN];
    char     ipv4[INET_ADDRSTRLEN];
    uint16_t port;
    int      has_addr;
} herald_device_t;

/* ─── Globals ─────────────────────────────────────────────────────── */
static herald_device_t g_devices[MAX_RESULTS];
static int           g_dev_count = 0;

/* ─── DNS name encoder ────────────────────────────────────────────── */
/* Encodes a dotted-label name (e.g. "_http._tcp.local") into DNS wire format.
 * Works directly on the input — no intermediate copy needed. */
static int encode_name(uint8_t *buf, int pos, const char *name)
{
    const char *p = name;
    while (*p) {
        const char *dot = strchr(p, '.');
        int len = dot ? (int)(dot - p) : (int)strlen(p);
        if (len > 0) {
            buf[pos++] = (uint8_t)len;
            memcpy(buf + pos, p, len);
            pos += len;
        }
        if (!dot) break;
        p = dot + 1;
    }
    buf[pos++] = 0;   /* root label */
    return pos;
}

/* ─── DNS name decoder (handles compression pointers) ─────────────── */
/* Writes directly into out — no intermediate buffer needed. */
static int decode_name(const uint8_t *buf, int buf_len,
                       int pos, char *out, int out_len)
{
    int jumped  = 0;
    int ret_pos = -1;
    int steps   = 0;
    int out_pos = 0;

    while (pos < buf_len && steps++ < 128) {
        uint8_t c = buf[pos];

        if ((c & 0xC0) == 0xC0) {           /* compression pointer */
            if (pos + 1 >= buf_len) return -1;
            if (!jumped) ret_pos = pos + 2;
            pos = ((c & 0x3F) << 8) | buf[pos + 1];
            jumped = 1;
            continue;
        }

        if (c == 0) {                        /* end of name */
            if (!jumped) ret_pos = pos + 1;
            break;
        }

        int label_len = c;
        pos++;
        if (pos + label_len > buf_len) return -1;

        if (out_pos > 0 && out_pos < out_len - 1)
            out[out_pos++] = '.';
        if (out_pos + label_len >= out_len) return -1;
        memcpy(out + out_pos, buf + pos, label_len);
        out_pos += label_len;
        pos += label_len;
    }

    out[out_pos] = '\0';
    return ret_pos;
}

/* ─── Build mDNS PTR query ────────────────────────────────────────── */
static int build_query(uint8_t *buf, const char *service)
{
    memset(buf, 0, 12);
    /* id=0, flags=0 (standard query), qdcount=1 */
    buf[5] = 1;
    int pos = 12;
    pos = encode_name(buf, pos, service);
    /* QTYPE = PTR, QCLASS = IN (unicast-preferred: bit 15 set) */
    buf[pos++] = 0x00; buf[pos++] = DNS_TYPE_PTR;
    buf[pos++] = 0x80; buf[pos++] = DNS_CLASS_IN;
    return pos;
}

/* ─── Find or create device entry by name ─────────────────────────── */
static herald_device_t *get_device(const char *name)
{
    for (int i = 0; i < g_dev_count; i++)
        if (strcmp(g_devices[i].name, name) == 0)
            return &g_devices[i];

    if (g_dev_count >= MAX_RESULTS) return NULL;
    herald_device_t *d = &g_devices[g_dev_count++];
    memset(d, 0, sizeof(*d));
    strncpy(d->name, name, MAX_NAME_LEN - 1);
    return d;
}

/* ─── Parse one DNS resource record ──────────────────────────────── */
static int parse_rr(const uint8_t *buf, int buf_len, int pos)
{
    char name[MAX_NAME_LEN];
    pos = decode_name(buf, buf_len, pos, name, sizeof(name));
    if (pos < 0 || pos + 10 > buf_len) return -1;

    uint16_t rtype  = (buf[pos] << 8) | buf[pos + 1];
    uint16_t rdlen  = (buf[pos + 8] << 8) | buf[pos + 9];
    int      rdata  = pos + 10;

    if (rdata + rdlen > buf_len) return -1;

    if (rtype == DNS_TYPE_PTR) {
        char ptr_name[MAX_NAME_LEN];
        if (decode_name(buf, buf_len, rdata, ptr_name, sizeof(ptr_name)) >= 0)
            get_device(ptr_name);   /* register new device */
    }
    else if (rtype == DNS_TYPE_SRV && rdlen >= 7) {
        uint16_t port = (buf[rdata + 4] << 8) | buf[rdata + 5];
        char target[MAX_NAME_LEN];
        decode_name(buf, buf_len, rdata + 6, target, sizeof(target));
        herald_device_t *d = get_device(name);
        if (d) {
            d->port = port;
            strncpy(d->target, target, MAX_NAME_LEN - 1);
        }
    }
    else if (rtype == DNS_TYPE_A && rdlen == 4) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, buf + rdata, ip, sizeof(ip));
        /* Associate A record with matching device by target hostname */
        for (int i = 0; i < g_dev_count; i++) {
            if (strcmp(g_devices[i].target, name) == 0 ||
                strcmp(g_devices[i].name,   name) == 0) {
                strncpy(g_devices[i].ipv4, ip, INET_ADDRSTRLEN - 1);
                g_devices[i].has_addr = 1;
            }
        }
    }

    return rdata + rdlen;
}

/* ─── Parse full mDNS response packet ────────────────────────────── */
static void parse_response(const uint8_t *buf, int len)
{
    if (len < 12) return;

    uint16_t flags   = (buf[2] << 8) | buf[3];
    if (!(flags & 0x8000)) return;   /* not a response */

    uint16_t qdcount = (buf[4]  << 8) | buf[5];
    uint16_t ancount = (buf[6]  << 8) | buf[7];
    uint16_t nscount = (buf[8]  << 8) | buf[9];
    uint16_t arcount = (buf[10] << 8) | buf[11];

    int pos = 12;

    /* skip questions */
    for (int i = 0; i < qdcount && pos > 0 && pos < len; i++) {
        char tmp[MAX_NAME_LEN];
        pos = decode_name(buf, len, pos, tmp, sizeof(tmp));
        if (pos < 0 || pos + 4 > len) return;
        pos += 4;   /* QTYPE + QCLASS */
    }

    int total_rr = ancount + nscount + arcount;
    for (int i = 0; i < total_rr && pos > 0 && pos < len; i++)
        pos = parse_rr(buf, len, pos);
}

/* ─── Create + configure multicast socket ────────────────────────── */
/* ifas: pre-enumerated interface list from main (avoids redundant getifaddrs). */
static int create_socket(struct ifaddrs *ifas)
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) { log_err("socket: %s", strerror(errno)); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_REUSEPORT
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif

    /* Bind to mDNS port */
    struct sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(MDNS_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log_err("bind: %s", strerror(errno)); close(fd); return -1;
    }

    /* Join multicast group on all suitable interfaces */
    in_addr_t mdns_group = inet_addr(MDNS_ADDR);
    int joined = 0;
    for (struct ifaddrs *ifa = ifas; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        if (!(ifa->ifa_flags & IFF_UP) || (ifa->ifa_flags & IFF_LOOPBACK)) continue;

        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = mdns_group;
        mreq.imr_interface = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       &mreq, sizeof(mreq)) == 0) {
            joined++;
        }
    }
    if (!joined) {
        /* fallback: join on INADDR_ANY */
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = mdns_group;
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    }

    /* TTL = 255 as per RFC 6762 */
    uint8_t ttl = 255;
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    return fd;
}

/* ─── Send query on all active interfaces ─────────────────────────── */
/* ifas: pre-enumerated interface list from main (avoids redundant getifaddrs). */
static void send_query(int fd, const uint8_t *pkt, int pkt_len,
                       struct ifaddrs *ifas)
{
    struct sockaddr_in dst = {0};
    dst.sin_family      = AF_INET;
    dst.sin_port        = htons(MDNS_PORT);
    dst.sin_addr.s_addr = inet_addr(MDNS_ADDR);

    for (struct ifaddrs *ifa = ifas; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        if (!(ifa->ifa_flags & IFF_UP) || (ifa->ifa_flags & IFF_LOOPBACK)) continue;

        struct in_addr local = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &local, sizeof(local));
        if (sendto(fd, pkt, pkt_len, 0,
                   (struct sockaddr *)&dst, sizeof(dst)) < 0)
            log_err("sendto: %s", strerror(errno));
    }
}

/* ─── Print results ───────────────────────────────────────────────── */
static void print_results(void)
{
    int shown = 0;
    printf("\n%-40s %-22s %s\n", "NAME", "IP", "PORT");
    printf("%-40s %-22s %s\n",
           "----------------------------------------",
           "----------------------", "-----");

    for (int i = 0; i < g_dev_count; i++) {
        herald_device_t *d = &g_devices[i];
        /* Skip pure PTR entries without address */
        if (!d->has_addr && d->port == 0) continue;
        printf("%-40s %-22s %u\n",
               d->name,
               d->has_addr ? d->ipv4 : "(no addr)",
               d->port);
        shown++;
    }

    if (!shown)
        printf("  (no devices found)\n");
    printf("\nTotal: %d device(s)\n", shown);

    /* Log a compact summary to syslog (avoids flooding with table rows) */
    syslog(LOG_INFO, "scan complete: %d device(s) found", shown);
}

/* ─── Main ────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    const char *service  = (argc > 1) ? argv[1] : "_services._dns-sd._udp.local";
    int         timeout  = (argc > 2) ? atoi(argv[2]) : 5;
    if (timeout <= 0) timeout = 5;

    /* Guard against a service name so long it would overflow the query buffer */
    if (strlen(service) >= MAX_NAME_LEN) {
        fprintf(stderr, "Error: service name too long (max %d chars)\n",
                MAX_NAME_LEN - 1);
        return 1;
    }

    openlog("herald-client", LOG_PID | LOG_CONS, LOG_DAEMON);
    log_info("searching for: %s (timeout: %ds)", service, timeout);

    /* Enumerate interfaces once; reuse for socket setup and query sending. */
    struct ifaddrs *ifas = NULL;
    if (getifaddrs(&ifas) < 0)
        log_warn("getifaddrs: %s", strerror(errno));
        /* ifas stays NULL — create_socket/send_query handle NULL gracefully */

    int fd = create_socket(ifas);
    if (fd < 0) { if (ifas) freeifaddrs(ifas); closelog(); return 1; }

    uint8_t query[HERALD_BUF_SIZE];
    int     query_len = build_query(query, service);

    /* Send query twice for reliability (RFC 6762 §6) */
    send_query(fd, query, query_len, ifas);
    usleep(200 * 1000);
    send_query(fd, query, query_len, ifas);

    if (ifas) freeifaddrs(ifas);

    uint8_t        buf[HERALD_BUF_SIZE];
    struct pollfd  pfd = { .fd = fd, .events = POLLIN };
    time_t         deadline = time(NULL) + timeout;

    time_t now;
    while ((now = time(NULL)) < deadline) {
        int ms_left = (int)((deadline - now) * 1000);

        int r = poll(&pfd, 1, ms_left);
        if (r < 0) { log_err("poll: %s", strerror(errno)); break; }
        if (r == 0) break;   /* timeout */

        if (pfd.revents & POLLIN) {
            struct sockaddr_in src;
            socklen_t src_len = sizeof(src);
            ssize_t n = recvfrom(fd, buf, sizeof(buf), 0,
                                 (struct sockaddr *)&src, &src_len);
            if (n > 0)
                parse_response(buf, (int)n);
        }
    }

    close(fd);
    print_results();
    closelog();
    return 0;
}
