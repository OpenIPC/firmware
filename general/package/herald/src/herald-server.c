/*
 *
 * Copyright (c) OpenIPC  https://openipc.org  MIT License
 *
 * herald-server.c — mDNS/DNS-SD service announcer (RFC 6762 / RFC 6763)
 *
 */

/* Required for usleep() on embedded Linux targets (POSIX XSI extension) */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
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
#define MDNS_ADDR       "224.0.0.251"
#define MDNS_PORT       5353
#define HERALD_BUF_SIZE   4096
#define MAX_NAME_LEN    256
#define MAX_TXT_LEN     256
#define TTL_DEFAULT     4500   /* 75 minutes, typical for mDNS */
#define TTL_GOODBYE     0      /* TTL=0 means "goodbye" packet  */

/* ─── DNS record types / classes ──────────────────────────────────── */
#define DNS_TYPE_A      1
#define DNS_TYPE_PTR    12
#define DNS_TYPE_TXT    16
#define DNS_TYPE_SRV    33
#define DNS_CLASS_IN    1
#define DNS_CLASS_FLUSH 0x8000  /* cache-flush bit (authoritative) */
#define DNS_TYPE_ANY    255

/* ─── Responder state ─────────────────────────────────────────────── */
typedef struct {
    char     hostname[MAX_NAME_LEN];   /* e.g. "mycam.local"           */
    char     service[MAX_NAME_LEN];    /* e.g. "_rtsp._tcp.local"      */
    char     instance[MAX_NAME_LEN];   /* e.g. "mycam._rtsp._tcp.local"*/
    char     txt[MAX_TXT_LEN];         /* key=value TXT record         */
    uint16_t port;
    uint32_t ipv4;                     /* network byte order           */
} herald_ctx_t;

static herald_ctx_t  g_ctx;
static volatile sig_atomic_t g_running = 1;

/* ─── Signal handler ──────────────────────────────────────────────── */
static void on_signal(int sig)
{
    (void)sig;
    g_running = 0;
}

/* ─── Helpers: big-endian writers ────────────────────────────────── */
static inline void put16(uint8_t *b, int pos, uint16_t v)
{
    b[pos]     = (v >> 8) & 0xFF;
    b[pos + 1] =  v       & 0xFF;
}
static inline void put32(uint8_t *b, int pos, uint32_t v)
{
    b[pos]     = (v >> 24) & 0xFF;
    b[pos + 1] = (v >> 16) & 0xFF;
    b[pos + 2] = (v >>  8) & 0xFF;
    b[pos + 3] =  v        & 0xFF;
}
static inline uint16_t get16(const uint8_t *b, int pos)
{
    return (uint16_t)((b[pos] << 8) | b[pos + 1]);
}

/* ─── DNS name encoder ────────────────────────────────────────────── */
/* Encodes a dotted-label name into DNS wire format. Writes directly
 * from the input — callers must ensure name length < MAX_NAME_LEN. */
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

/* ─── DNS name decoder (handles compression) ──────────────────────── */
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

        if ((c & 0xC0) == 0xC0) {
            if (pos + 1 >= buf_len) return -1;
            if (!jumped) ret_pos = pos + 2;
            pos = ((c & 0x3F) << 8) | buf[pos + 1];
            jumped = 1;
            continue;
        }
        if (c == 0) {
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

/* ═══════════════════════════════════════════════════════════════════
 *  RESPONSE BUILDER
 * ═══════════════════════════════════════════════════════════════════ */

/*
 * Append a PTR record:
 *   <service_type>  PTR  <instance_name>
 */
static int append_ptr(uint8_t *buf, int pos, const herald_ctx_t *ctx, uint32_t ttl)
{
    pos = encode_name(buf, pos, ctx->service);
    put16(buf, pos, DNS_TYPE_PTR);  pos += 2;
    put16(buf, pos, DNS_CLASS_IN);  pos += 2;
    put32(buf, pos, ttl);           pos += 4;

    /* rdlength placeholder */
    int rdlen_pos = pos; pos += 2;
    int rdata_start = pos;
    pos = encode_name(buf, pos, ctx->instance);
    put16(buf, rdlen_pos, (uint16_t)(pos - rdata_start));
    return pos;
}

/*
 * Append an SRV record:
 *   <instance_name>  SRV  priority=0 weight=0 port=<port> <hostname>
 */
static int append_srv(uint8_t *buf, int pos, const herald_ctx_t *ctx, uint32_t ttl)
{
    pos = encode_name(buf, pos, ctx->instance);
    put16(buf, pos, DNS_TYPE_SRV);               pos += 2;
    put16(buf, pos, DNS_CLASS_IN | DNS_CLASS_FLUSH); pos += 2;
    put32(buf, pos, ttl);                         pos += 4;

    int rdlen_pos = pos; pos += 2;
    int rdata_start = pos;
    put16(buf, pos, 0); pos += 2;   /* priority */
    put16(buf, pos, 0); pos += 2;   /* weight   */
    put16(buf, pos, ctx->port); pos += 2;
    pos = encode_name(buf, pos, ctx->hostname);
    put16(buf, rdlen_pos, (uint16_t)(pos - rdata_start));
    return pos;
}

/*
 * Append a TXT record:
 *   <instance_name>  TXT  "key=value"
 */
static int append_txt(uint8_t *buf, int pos, const herald_ctx_t *ctx, uint32_t ttl)
{
    pos = encode_name(buf, pos, ctx->instance);
    put16(buf, pos, DNS_TYPE_TXT);               pos += 2;
    put16(buf, pos, DNS_CLASS_IN | DNS_CLASS_FLUSH); pos += 2;
    put32(buf, pos, ttl);                         pos += 4;

    uint8_t txt_len = (uint8_t)strlen(ctx->txt);
    put16(buf, pos, (uint16_t)(1 + txt_len));  pos += 2;  /* rdlength */
    buf[pos++] = txt_len;
    memcpy(buf + pos, ctx->txt, txt_len);
    pos += txt_len;
    return pos;
}

/*
 * Append an A record:
 *   <hostname>  A  <ipv4>
 */
static int append_a(uint8_t *buf, int pos, const herald_ctx_t *ctx, uint32_t ttl)
{
    pos = encode_name(buf, pos, ctx->hostname);
    put16(buf, pos, DNS_TYPE_A);                 pos += 2;
    put16(buf, pos, DNS_CLASS_IN | DNS_CLASS_FLUSH); pos += 2;
    put32(buf, pos, ttl);                         pos += 4;
    put16(buf, pos, 4);                           pos += 2;  /* rdlength */
    memcpy(buf + pos, &ctx->ipv4, 4);             pos += 4;
    return pos;
}

/*
 * Build a complete mDNS response with PTR + SRV + TXT + A.
 * Returns packet length.
 */
static int build_response(uint8_t *buf, const herald_ctx_t *ctx, uint32_t ttl)
{
    memset(buf, 0, 12);
    /* Flags: QR=1 (response), AA=1 (authoritative) */
    put16(buf, 2, 0x8400);
    /* ancount = 4 records */
    put16(buf, 6, 4);

    int pos = 12;
    pos = append_ptr(buf, pos, ctx, ttl);
    pos = append_srv(buf, pos, ctx, ttl);
    pos = append_txt(buf, pos, ctx, ttl);
    pos = append_a  (buf, pos, ctx, ttl);
    return pos;
}

/* ═══════════════════════════════════════════════════════════════════
 *  SOCKET
 * ═══════════════════════════════════════════════════════════════════ */

static int create_socket(void)
{
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) { log_err("socket: %s", strerror(errno)); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_REUSEPORT
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif

    struct sockaddr_in addr = {0};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(MDNS_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        log_err("bind: %s", strerror(errno)); close(fd); return -1;
    }

    /* Join multicast on every UP non-loopback interface */
    struct ifaddrs *ifas = NULL, *ifa;
    if (getifaddrs(&ifas) < 0) {
        log_warn("getifaddrs: %s", strerror(errno));
        /* continue without multicast join — fallback below */
    }
    int joined = 0;
    for (ifa = ifas; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        if (!(ifa->ifa_flags & IFF_UP) || (ifa->ifa_flags & IFF_LOOPBACK)) continue;
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(MDNS_ADDR);
        mreq.imr_interface = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       &mreq, sizeof(mreq)) == 0)
            joined++;
    }
    if (ifas) freeifaddrs(ifas);

    if (!joined) {
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(MDNS_ADDR);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    }

    uint8_t ttl = 255;
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    /* Disable loopback so we don't receive our own packets */
    uint8_t loop = 0;
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

    return fd;
}

/* ─── Send packet to mDNS multicast group ─────────────────────────── */
static void herald_send(int fd, const uint8_t *pkt, int len)
{
    struct sockaddr_in dst = {0};
    dst.sin_family      = AF_INET;
    dst.sin_port        = htons(MDNS_PORT);
    dst.sin_addr.s_addr = inet_addr(MDNS_ADDR);
    if (sendto(fd, pkt, len, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0)
        log_err("sendto: %s", strerror(errno));
}

/* ═══════════════════════════════════════════════════════════════════
 *  QUERY PARSER — decide what to answer
 * ═══════════════════════════════════════════════════════════════════ */

typedef enum {
    MATCH_NONE = 0,
    MATCH_PTR,     /* someone asking for our service type */
    MATCH_SRV,     /* someone asking for our instance    */
    MATCH_A        /* someone asking for our hostname    */
} match_t;

static match_t classify_query(const uint8_t *buf, int len,
                               const herald_ctx_t *ctx)
{
    if (len < 12) return MATCH_NONE;

    uint16_t flags   = get16(buf, 2);
    if (flags & 0x8000) return MATCH_NONE;   /* it's a response, ignore */

    uint16_t qdcount = get16(buf, 4);
    int pos = 12;
    match_t best = MATCH_NONE;

    for (int i = 0; i < qdcount && pos > 0 && pos < len; i++) {
        char name[MAX_NAME_LEN];
        int next = decode_name(buf, len, pos, name, sizeof(name));
        if (next < 0 || next + 4 > len) break;

        uint16_t qtype  = get16(buf, next);
        /* uint16_t qclass = get16(buf, next + 2); */
        pos = next + 4;

        /* Lowercase compare (mDNS names are case-insensitive) */
        /* Simple check: compare directly (names already lowercase typical) */
        if (strcasecmp(name, ctx->service) == 0) {
            if (qtype == DNS_TYPE_PTR || qtype == DNS_TYPE_ANY)
                best = MATCH_PTR;
        } else if (strcasecmp(name, ctx->instance) == 0) {
            if (qtype == DNS_TYPE_SRV || qtype == DNS_TYPE_ANY)
                best = MATCH_SRV;
        } else if (strcasecmp(name, ctx->hostname) == 0) {
            if (qtype == DNS_TYPE_A || qtype == DNS_TYPE_ANY)
                best = MATCH_A;
        }
    }

    return best;
}

/* ═══════════════════════════════════════════════════════════════════
 *  PROBING (RFC 6762 §8.1) — conflict detection before announcing
 * ═══════════════════════════════════════════════════════════════════ */

/*
 * Return 1 if an mDNS response packet contains any record whose owner
 * name matches hostname (case-insensitive).  Used to filter out unrelated
 * multicast responses that arrive during the probe window.
 */
static int response_contains_name(const uint8_t *buf, int len,
                                   const char *hostname)
{
    if (len < 12) return 0;

    uint16_t qdcount = get16(buf, 4);
    uint16_t ancount = get16(buf, 6);
    uint16_t nscount = get16(buf, 8);
    uint16_t arcount = get16(buf, 10);

    int pos = 12;

    /* Skip the question section */
    for (int i = 0; i < qdcount && pos > 0 && pos < len; i++) {
        char tmp[MAX_NAME_LEN];
        pos = decode_name(buf, len, pos, tmp, sizeof(tmp));
        if (pos < 0 || pos + 4 > len) return 0;
        pos += 4;   /* QTYPE + QCLASS */
    }

    /* Scan answer / authority / additional records */
    int total = (int)ancount + (int)nscount + (int)arcount;
    for (int i = 0; i < total && pos > 0 && pos < len; i++) {
        char name[MAX_NAME_LEN];
        int next = decode_name(buf, len, pos, name, sizeof(name));
        if (next < 0 || next + 10 > len) return 0;
        if (strcasecmp(name, hostname) == 0) return 1;
        uint16_t rdlen = get16(buf, next + 8);
        pos = next + 10 + (int)rdlen;
        if (pos > len) return 0;
    }
    return 0;
}

static int probe_hostname(int fd, const herald_ctx_t *ctx)
{
    /* Use HERALD_BUF_SIZE — a max-length hostname encodes to ~258 bytes,
     * so the full probe packet (header + question + authority) can exceed
     * the naive 512-byte allocation when MAX_NAME_LEN names are used. */
    uint8_t  pkt[HERALD_BUF_SIZE];
    memset(pkt, 0, 12);
    put16(pkt, 4, 1);   /* qdcount = 1 */
    put16(pkt, 10, 1);  /* arcount = 1 (authority: our proposed A record) */

    int pos = 12;
    pos = encode_name(pkt, pos, ctx->hostname);
    put16(pkt, pos, DNS_TYPE_ANY); pos += 2;
    put16(pkt, pos, DNS_CLASS_IN); pos += 2;

    /* Authority section: our A record */
    pos = encode_name(pkt, pos, ctx->hostname);
    put16(pkt, pos, DNS_TYPE_A);   pos += 2;
    put16(pkt, pos, DNS_CLASS_IN | DNS_CLASS_FLUSH); pos += 2;
    put32(pkt, pos, TTL_DEFAULT);  pos += 4;
    put16(pkt, pos, 4);            pos += 2;
    memcpy(pkt + pos, &ctx->ipv4, 4); pos += 4;

    uint8_t        rbuf[HERALD_BUF_SIZE];
    struct pollfd  pfd = { .fd = fd, .events = POLLIN };

    log_info("[probe] probing %s", ctx->hostname);

    for (int attempt = 0; attempt < 3; attempt++) {
        herald_send(fd, pkt, pos);

        /* Drain all packets arriving in the 250ms window (RFC 6762 §8.1).
         * A single recvfrom could miss a conflict if another packet
         * arrived first, so we keep reading until the window closes. */
        int ms_left = 250;
        while (ms_left > 0) {
            struct timespec t0, t1;
            clock_gettime(CLOCK_MONOTONIC, &t0);

            int r = poll(&pfd, 1, ms_left);

            clock_gettime(CLOCK_MONOTONIC, &t1);
            ms_left -= (int)((t1.tv_sec  - t0.tv_sec)  * 1000 +
                             (t1.tv_nsec - t0.tv_nsec) / 1000000);

            if (r <= 0) break;
            if (!(pfd.revents & POLLIN)) break;

            struct sockaddr_in src;
            socklen_t slen = sizeof(src);
            ssize_t n = recvfrom(fd, rbuf, sizeof(rbuf), 0,
                                 (struct sockaddr *)&src, &slen);
            if (n > 6) {
                uint16_t flags = get16(rbuf, 2);
                /* Only treat it as a conflict if the response actually
                 * contains a record for our hostname (RFC 6762 §8.1).
                 * Unrelated multicast responses must not be misread as
                 * conflicts — that would be a false positive. */
                if ((flags & 0x8000) &&
                    response_contains_name(rbuf, (int)n, ctx->hostname)) {
                    log_warn("[probe] conflict detected for %s, another device responded",
                             ctx->hostname);
                    return -1;
                }
            }
        }
    }

    log_info("[probe] %s is free, no conflict", ctx->hostname);
    return 0;
}

/* ═══════════════════════════════════════════════════════════════════
 *  ANNOUNCEMENT (RFC 6762 §8.3)
 * ═══════════════════════════════════════════════════════════════════ */

static void announce(int fd, const herald_ctx_t *ctx, uint32_t ttl)
{
    uint8_t pkt[HERALD_BUF_SIZE];
    int len = build_response(pkt, ctx, ttl);
    herald_send(fd, pkt, len);
}

/* ═══════════════════════════════════════════════════════════════════
 *  FIRST IP ON A NON-LOOPBACK INTERFACE
 * ═══════════════════════════════════════════════════════════════════ */

static uint32_t detect_local_ip(void)
{
    struct ifaddrs *ifas = NULL, *ifa;
    uint32_t ip = htonl(INADDR_LOOPBACK);

    if (getifaddrs(&ifas) < 0) {
        log_err("getifaddrs: %s", strerror(errno));
        return ip;
    }
    for (ifa = ifas; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        if (!(ifa->ifa_flags & IFF_UP) || (ifa->ifa_flags & IFF_LOOPBACK)) continue;
        ip = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
        break;
    }
    if (ifas) freeifaddrs(ifas);

    if (ip == htonl(INADDR_LOOPBACK))
        log_warn("no non-loopback interface found, advertising 127.0.0.1 (peers won't connect)");

    return ip;
}

/* ═══════════════════════════════════════════════════════════════════
 *  MAIN LOOP
 * ═══════════════════════════════════════════════════════════════════ */

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr,
            "Usage: %s <hostname> <service_type> <port> [txt]\n"
            "  hostname     — e.g. mycam  (without .local)\n"
            "  service_type — e.g. _rtsp._tcp.local\n"
            "  port         — e.g. 554\n"
            "  txt          — optional TXT record, e.g. path=/stream\n"
            "\nExamples:\n"
            "  %s mycam     _rtsp._tcp.local 554  path=/stream\n"
            "  %s myrouter  _http._tcp.local 80   admin=true\n",
            argv[0], argv[0], argv[0]);
        return 1;
    }

    herald_ctx_t *ctx = &g_ctx;
    memset(ctx, 0, sizeof(*ctx));

    /* hostname → "name.local" */
    snprintf(ctx->hostname, sizeof(ctx->hostname), "%s.local", argv[1]);

    /* service type (already fully qualified, e.g. _rtsp._tcp.local) */
    strncpy(ctx->service, argv[2], sizeof(ctx->service) - 1);
    ctx->service[sizeof(ctx->service) - 1] = '\0';

    /* Validate port number */
    int port_arg = atoi(argv[3]);
    if (port_arg <= 0 || port_arg > 65535) {
        fprintf(stderr, "Error: port must be 1–65535 (got \"%s\")\n", argv[3]);
        return 1;
    }
    ctx->port = (uint16_t)port_arg;

    /* instance = "hostname._service_type"
     * e.g. "mycam._rtsp._tcp.local"                                    */
    snprintf(ctx->instance, sizeof(ctx->instance), "%s.%s", argv[1], argv[2]);

    strncpy(ctx->txt,
            argc > 4 ? argv[4] : "txtvers=1",
            sizeof(ctx->txt) - 1);
    ctx->txt[sizeof(ctx->txt) - 1] = '\0';

    ctx->ipv4 = detect_local_ip();

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ctx->ipv4, ip_str, sizeof(ip_str));

    openlog("herald-server", LOG_PID | LOG_CONS, LOG_DAEMON);

    /* Print startup banner to stdout; log a single summary line to syslog */
    printf("=== Herald Responder ===\n");
    printf("  Hostname  : %s\n", ctx->hostname);
    printf("  Service   : %s\n", ctx->service);
    printf("  Instance  : %s\n", ctx->instance);
    printf("  Address   : %s\n", ip_str);
    printf("  Port      : %u\n", ctx->port);
    printf("  TXT       : %s\n", ctx->txt);
    printf("  Multicast : %s:%d\n", MDNS_ADDR, MDNS_PORT);
    printf("======================\n");
    syslog(LOG_NOTICE, "starting: host=%s service=%s addr=%s port=%u",
           ctx->hostname, ctx->service, ip_str, ctx->port);

    int fd = create_socket();
    if (fd < 0) { closelog(); return 1; }

    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    /* Seed PRNG used for RFC 6762 §6 response delay jitter */
    srand((unsigned)time(NULL));

    /* ── Probe (conflict detection) ── */
    if (probe_hostname(fd, ctx) < 0)
        log_warn("[probe] hostname conflict, continuing with unique-suffix recommended");

    /* ── Initial announcements × 3 (RFC 6762 §8.3) ── */
    log_info("[announce] starting");
    for (int i = 0; i < 3; i++) {
        announce(fd, ctx, TTL_DEFAULT);
        if (i < 2) usleep(250 * 1000);
    }
    log_info("[ready] listening on %s:%d", MDNS_ADDR, MDNS_PORT);

    /* ── Main event loop ── */
    uint8_t       buf[HERALD_BUF_SIZE];
    struct pollfd pfd = { .fd = fd, .events = POLLIN };

    while (g_running) {
        int r = poll(&pfd, 1, 1000);
        if (r < 0) {
            if (errno == EINTR) continue;
            log_err("poll: %s", strerror(errno)); break;
        }
        if (r == 0) continue;   /* idle tick */

        if (!(pfd.revents & POLLIN)) continue;

        struct sockaddr_in src;
        socklen_t src_len = sizeof(src);
        ssize_t n = recvfrom(fd, buf, sizeof(buf), 0,
                             (struct sockaddr *)&src, &src_len);
        if (n <= 0) continue;

        match_t m = classify_query(buf, (int)n, ctx);
        if (m == MATCH_NONE) continue;

        char src_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &src.sin_addr, src_ip, sizeof(src_ip));
        log_info("[query] from %s match=%s", src_ip,
               m == MATCH_PTR ? "PTR" :
               m == MATCH_SRV ? "SRV" : "A");

        /* Small random delay 20-120ms to avoid answer storms (RFC 6762 §6) */
        usleep((20 + (rand() % 100)) * 1000);

        uint8_t resp[HERALD_BUF_SIZE];
        int     resp_len = build_response(resp, ctx, TTL_DEFAULT);
        herald_send(fd, resp, resp_len);
    }

    /* ── Goodbye packets × 2 (RFC 6762 §11.3, TTL=0) ── */
    log_info("[goodbye] sending TTL=0 packets");
    for (int i = 0; i < 2; i++) {
        announce(fd, ctx, TTL_GOODBYE);
        usleep(100 * 1000);
    }

    close(fd);
    log_info("[done]");
    closelog();
    return 0;
}