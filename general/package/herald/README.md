# Herald

**Herald** is a compact mDNS stack built for the [OpenIPC](https://openipc.org) project.
Designed specifically for OpenIPC IP cameras, routers, and other embedded Linux targets
that may join the OpenIPC ecosystem — wherever mDNS-based service discovery is needed
on resource-constrained hardware.

Implements RFC 6762 (mDNS) and RFC 6763 (DNS-SD) and ships as two independent utilities:

| Utility | File | Purpose |
|---|---|---|
| `herald-server` | `herald-server.c` | Announces a service on the local network and responds to mDNS queries |
| `herald-client` | `herald-client.c` | Scans the local network and prints discovered services |

---

## OpenIPC

Herald is part of the [OpenIPC](https://openipc.org) open-source firmware ecosystem
for IP surveillance cameras. It enables cameras, NVRs, routers, and other devices
running OpenIPC firmware to advertise themselves and discover peers on the local
network with zero configuration — no DNS server required.

Typical OpenIPC use cases:

| Device | Service type | Port |
|---|---|---|
| IP camera (RTSP stream) | `_rtsp._tcp.local` | `554` |
| IP camera (web UI) | `_http._tcp.local` | `80` |
| OpenIPC NVR | `_nvr._tcp.local` | `8080` |
| OpenIPC router | `_http._tcp.local` | `80` |

---

## Build

### Native

```sh
make
```

Or without `make`:

```sh
gcc -Os -o herald-server herald-server.c
gcc -Os -o herald-client herald-client.c
```

**Requirements:** C99+ compiler, POSIX environment with `getifaddrs`, `poll`, `IP_ADD_MEMBERSHIP`.
Works with uClibc, musl, and glibc. No external libraries required.

### Cross-compilation

```sh
make CC=arm-linux-gnueabihf-gcc CFLAGS="-Os -march=armv7-a"
make install DESTDIR=./rootfs PREFIX=/usr
```

### Buildroot integration (BR2_EXTERNAL)

Herald ships a ready-to-use Buildroot external tree under `buildroot/`.

```sh
# From your Buildroot source root:
make BR2_EXTERNAL=/path/to/herald/buildroot menuconfig
# Enable: Target packages → Herald → [*] herald
make
```

Binaries are installed to `/usr/bin/herald-client` and `/usr/bin/herald-server`
on the target rootfs.

To add Herald directly to an existing Buildroot `package/` tree, copy
`buildroot/package/herald/` into `package/herald/` and add
`source "package/herald/Config.in"` to `package/Config.in`.

---

## herald-server -- service announcement

Runs an mDNS responder: announces a host and service on the local network, responds to PTR / SRV / TXT / A queries.

### Usage

```
./herald-server <hostname> <service_type> <port> [txt]
```

| Argument | Description | Example |
|---|---|---|
| `hostname` | Host name without `.local` | `mycam` |
| `service_type` | DNS-SD service type (fully qualified) | `_rtsp._tcp.local` |
| `port` | TCP/UDP service port (1-65535) | `554` |
| `txt` | Optional TXT record in `key=value` format | `path=/stream` |

### Examples

```sh
# OpenIPC camera with RTSP stream
./herald-server mycam _rtsp._tcp.local 554 "path=/stream"

# OpenIPC router with web interface
./herald-server myrouter _http._tcp.local 80 "admin=true"

# Network printer
./herald-server printer1 _ipp._tcp.local 631 "txtvers=1"
```

### Lifecycle

```
[probe]     Conflict detection (3 x 250 ms, RFC 6762 s8.1)
[announce]  3 announcements with TTL=75 min (RFC 6762 s8.3)
[ready]     Listening for queries, responding with 20-120 ms jitter (RFC 6762 s6)
^C
[goodbye]   2 packets with TTL=0 (RFC 6762 s11.3)
```

---

## herald-client -- network scan

Sends an mDNS PTR query, collects responses for a given duration, and prints a table of discovered devices.

### Usage

```
./herald-client [service_type] [timeout_sec]
```

| Argument | Default | Description |
|---|---|---|
| `service_type` | `_services._dns-sd._udp.local` | Service type to search for |
| `timeout_sec` | `5` | Time to wait for responses (seconds) |

### Examples

```sh
# Find all OpenIPC cameras by RTSP
./herald-client _rtsp._tcp.local 5

# Find all HTTP devices on the network
./herald-client _http._tcp.local 3

# Enumerate all announced service types
./herald-client _services._dns-sd._udp.local 5
```

### Sample output

```
searching for: _rtsp._tcp.local (timeout: 3s)

NAME                                     IP                     PORT
---------------------------------------- ---------------------- -----
mycam._rtsp._tcp.local                   192.168.1.42           554

Total: 1 device(s)
```

---

## Protocol notes

Herald operates on top of the standard mDNS stack:

- **Multicast address:** `224.0.0.251:5353` (UDP)
- **Records:** PTR -> SRV -> TXT -> A (full DNS-SD set)
- **DNS compression:** supported in the decoder (RFC 1035 s4.1.4)
- **TTL:** 4500 s (75 minutes) for live announcements; 0 for goodbye packets
- **Probing:** 3 attempts x 250 ms before the first announcement
- **Response jitter:** random 20-120 ms delay to prevent answer storms

---

## Code layout

```
herald-server.c
+-- encode_name / decode_name   DNS wire-format codec
+-- append_ptr/srv/txt/a        record builders
+-- build_response              full response packet
+-- classify_query              incoming query parser
+-- probe_hostname              RFC 6762 s8.1 probing
+-- announce                    RFC 6762 s8.3 announcement
+-- main                        event loop (poll)

herald-client.c
+-- encode_name / decode_name   DNS wire-format codec
+-- build_query                 PTR query builder
+-- parse_rr / parse_response   response parser
+-- get_device                  discovered device registry
+-- main                        send x2 -> poll loop -> print

Makefile                        cross-compilation ready (CC / CFLAGS / DESTDIR)

buildroot/
+-- Config.in                   BR2_EXTERNAL entry point
+-- external.mk                 BR2_EXTERNAL metadata
+-- package/herald/
    +-- Config.in               Kconfig menu entry
    +-- herald.mk               Buildroot package rules
```

---

## Compatibility

Tested on:
- OpenIPC firmware (MIPS, ARM, RV1126)
- OpenWrt (MIPS, ARM)
- Embedded Linux (uClibc, musl, glibc)
- Ubuntu / Debian (development and testing)

---

## License

MIT
