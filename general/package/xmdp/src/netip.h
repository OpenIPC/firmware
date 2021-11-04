#ifndef NETIP_H
#define NETIP_H

enum ConnectStatus {
  CONNECT_OK,
  CONNECT_ERR,
  CONNECT_PWDREQ,
};

enum ConnectStatus netip_connect(const char *addr, uint16_t port);

#endif /* NETIP_H */
