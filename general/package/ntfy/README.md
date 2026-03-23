# ntfy — lightweight ntfy.sh client for embedded Linux

Minimal [ntfy](https://ntfy.sh) notification client written in C99 for
the **OpenIPC** ecosystem.  Designed for IP cameras, routers, NVR, and
other resource-constrained embedded Linux targets.

## Features

| Feature | Details |
|---------|---------|
| **Publish** | Send notifications with title, priority, and tags |
| **Subscribe** | Stream messages in real time (long-lived HTTP) |
| **Poll** | Fetch cached messages and exit |
| **Auth** | Bearer token (`-T`) and HTTP basic auth (`-u`) |
| **Stdin** | Pipe message body from other commands |
| **Syslog** | All activity logged via `syslog(3)` |

Single static-friendly binary.  Only external dependency is **libcurl**.

## Build

### Native

```sh
cd src
make
./ntfy pub mytopic "Hello from $(hostname)"
```

### Cross-compile

```sh
make CC=arm-linux-gnueabihf-gcc \
     CFLAGS="-Os -march=armv7-a" \
     LIBS="-lcurl"
```

### Buildroot (OpenIPC external tree)

Enable `BR2_PACKAGE_NTFY` in `menuconfig`.  The package automatically
pulls in `libcurl`.

## Usage

```
ntfy 1.0.0 — lightweight ntfy client for embedded Linux

Usage:
  ntfy pub  [opts] <topic> <message...>  Publish a message
  ntfy sub  [opts] <topic>               Subscribe (streaming)
  ntfy poll [opts] <topic>               Poll cached messages

Options:
  -s <url>        Server URL (default: https://ntfy.sh)
  -t <title>      Message title (pub)
  -p <priority>   Priority: min,low,default,high,urgent (pub)
  -g <tags>       Comma-separated tags (pub)
  -T <token>      Bearer auth token
  -u <user:pass>  Basic auth credentials
  -S <since>      Fetch since: duration/timestamp/id/all (sub/poll)
  -q              Quiet — message body only
  -h              Show help
```

## Examples

```sh
# Simple publish
ntfy pub alerts "Disk full on camera-7"

# Publish with title, priority, and tags
ntfy pub -t "Warning" -p high -g "warning,disk" alerts "Disk 95%"

# Publish from a script (stdin)
echo "Backup completed" | ntfy pub backups

# Subscribe (streaming) — blocks until Ctrl+C
ntfy sub alerts

# Poll recent messages (last 30 minutes)
ntfy poll -S 30m alerts

# Use a self-hosted server with auth
ntfy pub -s https://ntfy.example.com -T tk_mytoken123 cam01 "Motion detected"

# Quiet mode for scripting (message body only)
ntfy poll -q -S 1h alerts | while read -r msg; do
  echo "Got: $msg"
done
```

## Output format

### Default (verbose)

```
[2024-01-15 08:15:23] alerts [HIGH] Warning: CPU at 98%
  tags: warning,cpu
[2024-01-15 08:16:01] alerts Backup completed
```

### Quiet (`-q`)

```
CPU at 98%
Backup completed
```

## ntfy API coverage

| Endpoint | Method | Supported |
|----------|--------|-----------|
| `POST /<topic>` | Publish | ✅ |
| `GET /<topic>/json` | Subscribe (stream) | ✅ |
| `GET /<topic>/json?poll=1` | Poll | ✅ |
| `GET /<topic>/json?since=` | Since filter | ✅ |
| Title, Priority, Tags headers | Publish options | ✅ |
| Bearer / Basic auth | Authentication | ✅ |
| Attachments, Actions, Click | Extended features | ❌ (planned) |

## License

MIT — see [LICENSE](../../LICENSE) or the SPDX headers in source files.

## Links

- [ntfy.sh documentation](https://docs.ntfy.sh)
- [OpenIPC project](https://openipc.org)
