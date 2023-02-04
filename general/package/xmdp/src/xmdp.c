#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "cjson/cJSON.h"
#include "netip.h"
#include "utils.h"

#define SERVERPORT 34569
// send broadcast packets periodically
#define TIMEOUT 5 // seconds

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static int scansec = 0;

const char brpkt[] =
    "\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xfa\x05"
    "\x00\x00\x00\x00";
static const char *Reset = "\x1b[0m";
static const char *FgRed = "\x1b[31m";
static const char *FgBrightRed = "\033[31;1m";

static const char *color(enum ConnectStatus status) {
  switch (status) {
  case CONNECT_OK:
    return FgRed;
  case CONNECT_ERR:
    return FgBrightRed;
  default:
    return "";
  }
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int ipaddr_from32bit(char *dst, size_t dlen, const char *coded) {
  if (strlen(coded) != 10)
    return -1;

  unsigned char octet[4];
  char cnvt[5] = "0x01";
  for (int i = 0; i < sizeof(octet); i++) {
    cnvt[2] = coded[i * 2 + 2];
    cnvt[3] = coded[i * 2 + 3];
    sscanf(cnvt, "%hhx", &octet[i]);
  }

  snprintf(dst, dlen, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);

  return 0;
}

int bsock;

void send_netip_broadcast() {
  struct sockaddr_in sa;
  memset(sa.sin_zero, '\0', sizeof sa.sin_zero);

  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = 0xffffffff;
  sa.sin_port = htons(SERVERPORT);

  int rcvbts;
  if ((rcvbts = sendto(bsock, brpkt, sizeof(brpkt) - 1, 0,
                       (struct sockaddr *)&sa, sizeof(struct sockaddr_in))) ==
      -1) {
    perror("sendto");
    exit(1);
  }
}

void sigalarm(int sig) {
  if (sig == SIGALRM) {
    send_netip_broadcast();
    alarm(TIMEOUT);
  }
}

int scan() {
  time_t start, current;
  start = time(NULL);

  bsock = socket(AF_INET, SOCK_DGRAM, 0);
  if (bsock == -1) {
    perror("socket");
    exit(1);
  }

  struct sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = htons(SERVERPORT);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(bsock, (struct sockaddr *)&name, sizeof(name)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // this call is what allows broadcast packets to be sent:
  int broadcast = 1;
  if (setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, &broadcast,
                 sizeof broadcast) == -1) {
    perror("setsockopt (SO_BROADCAST)");
    exit(1);
  }

  send_netip_broadcast();

  printf("Searching for XM cameras... Abort with CTRL+C.\n\n"
         "IP\t\tMAC-Address\t\tIdentity\n");

  signal(SIGALRM, sigalarm);
  alarm(TIMEOUT);

  size_t seen_len = 0;
  size_t seen_cap = 1;
  uint32_t *seen_vec = malloc(seen_cap * sizeof(*seen_vec));

  while (1) {
	current = time(NULL);

	if ((scansec > 0) && (current-start > scansec)) {
	  exit(EXIT_SUCCESS);
	}

    char buf[1024];
    struct sockaddr_in their_addr;
    socklen_t addr_len = sizeof their_addr;
    int rcvbts;
    if ((rcvbts = recvfrom(bsock, buf, sizeof buf - 1, 0,
                           (struct sockaddr *)&their_addr, &addr_len)) == -1) {
      perror("recvfrom");
      exit(1);
    }
    if (rcvbts <= sizeof brpkt)
      continue;

    buf[rcvbts] = '\0';

    cJSON *json = cJSON_Parse(buf + 20);
    if (!json) {
      const char *error_ptr = cJSON_GetErrorPtr();
      if (error_ptr != NULL) {
        fprintf(stderr, "Error before: %s\n", error_ptr);
      }
      goto skip_loop;
    }
#if 0
    char *str = cJSON_Print(json);
    if (str) {
      puts(str);
    }
    free(str);
#endif

    const cJSON *netcommon =
        cJSON_GetObjectItemCaseSensitive(json, "NetWork.NetCommon");
    const char *hostname = get_json_strval(netcommon, "HostName", "");
    const char *mac = get_json_strval(netcommon, "MAC", "");
    const char *host_ip = get_json_strval(netcommon, "HostIP", "");
    const int netip_port = get_json_intval(netcommon, "TCPPort", 0);
    const int chan_num = get_json_intval(netcommon, "ChannelNum", 0);
    const char *sn = get_json_strval(netcommon, "SN", "");
    const char *version = get_json_strval(netcommon, "Version", "");
    const char *builddt = get_json_strval(netcommon, "BuildDate", "");

    uint32_t numipv4;
    if (sscanf(host_ip, "0x%x", &numipv4) == 1) {
      // find occurence
      for (int i = 0; i < seen_len; i++)
        if (seen_vec[i] == numipv4)
          goto skip_loop;
      if (seen_len == seen_cap) {
        seen_cap *= 2;
        seen_vec = realloc(seen_vec, seen_cap * sizeof(*seen_vec));
      }
      seen_vec[seen_len++] = numipv4;
    }

    char abuf[50] = {0};
    if (strlen(host_ip)) {
      ipaddr_from32bit(abuf, sizeof abuf, host_ip);
      host_ip = abuf;
    }
    enum ConnectStatus netip_conn = netip_connect(host_ip, netip_port);

    char verstr[128] = {0};
    if (strlen(version)) {
      int n_dot = 0, i = 0;
      while (*version) {
        if (*version == '.') {
          n_dot++;
          if (n_dot == 4)
            break;
        } else if (n_dot == 3) {
          verstr[i++] = *version;
        }
        version++;
      }

      if (strlen(builddt) == 19 && builddt[10] == ' ') {
        const char *end = builddt + 10;
        strcat(verstr + strlen(verstr), " (");
        snprintf(verstr + strlen(verstr),
                 MIN(sizeof(verstr) - strlen(verstr), end - builddt + 1), "%s",
                 builddt);
        strcat(verstr + strlen(verstr), ")");
      }
    }

    printf("%s%s\t%s\t%s %s, %s", color(netip_conn), host_ip, mac,
           chan_num > 1 ? "DVR" : "IPC", sn, hostname);
    if (strlen(verstr))
      printf("\t%s", verstr);
    printf("%s\n", *color(netip_conn) ? Reset : "");

  skip_loop:

    cJSON_Delete(json);
  }

  free(seen_vec);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "t:")) != -1) {
        switch (opt) {
		case 't':
			scansec = atoi(optarg);
			break;
		default:
			printf("Usage: %s [-t seconds]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
  	return scan();
}
