#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "cjson/cJSON.h"
#include "netip.h"
#include "utils.h"

#define PACKED __attribute__((packed))

#define OP_LOGIN 1000
#define OP_SYSINFO 1020

#define RESULT_OK 100
#define RESULT_UNKNOWN_ERROR 101
#define RESULT_INCORRECT_PWD 203

typedef struct netip_preabmle {
  uint8_t head;
  uint8_t version;
  uint16_t unused;
  uint32_t session;
  uint32_t sequence;
  uint8_t total;
  uint8_t cur;
  uint16_t msgid;
  uint32_t len_data;
  char data[];
} PACKED netip_preabmle_t;

#define MAX_UDP_PACKET_SIZE 0xFFFF

typedef union netip_pkt {
  char buf[MAX_UDP_PACKET_SIZE];
  netip_preabmle_t header;
} netip_pkt_t;

#define NETIP_HSIZE sizeof(netip_preabmle_t)
#define NETIP_MAX_JSON sizeof(resp) - NETIP_HSIZE - 1

bool netip_connect(const char *addr, uint16_t port) {
  bool res = true;

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1)
    return false;

  struct sockaddr_in srv;
  srv.sin_addr.s_addr = inet_addr(addr);
  srv.sin_family = AF_INET;
  srv.sin_port = htons(port);

  if (connect(s, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    return false;

  netip_pkt_t msg;
  memset(&msg.header, 0, sizeof(msg.header));
  msg.header.head = 0xff;
  msg.header.msgid = OP_LOGIN;
  const char default_login[] =
      "{\"EncryptType\": \"MD5\", \"LoginType\": \"DVRIP-Web\", \"PassWord\": "
      "\"tlJwpbo6\", \"UserName\": \"admin\"}\n\0";
  strcpy(msg.header.data, default_login);
  msg.header.len_data = sizeof(default_login);

  if (send(s, &msg, sizeof(default_login) + NETIP_HSIZE, 0) < 0) {
    puts("Send failed");
    return false;
  }

  if (recv(s, &msg, sizeof(msg), 0) <= NETIP_HSIZE) {
    puts("recv failed");
  }

  cJSON *json = cJSON_Parse(msg.header.data);
  if (!json) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    res = false;
    goto skip_loop;
  }
  const int retval = get_json_intval(json, "Ret", 0);
  if (retval != RESULT_OK)
    return false;

skip_loop:
  cJSON_Delete(json);

  return res;
}
