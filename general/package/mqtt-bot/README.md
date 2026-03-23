# mqtt-bot

A lightweight MQTT command executor written in C for routers and IP cameras.
Subscribes to an MQTT topic, executes only explicitly whitelisted commands, and
publishes their output back to a configurable response topic.

## Supported (fully tested) devices

- **Goke**
  - gk7205v200
  - gk7205v210
  - gk7205v300
- **HiSilicon**
  - hi3516cv300
- **Ingenic**
  - t23
- **SigmaStar**
  - ssc338q
  - ssc378de

## Features

- **Whitelist-only execution** — unknown commands are rejected without execution.
- **Command output over MQTT** — stdout + stderr of every command is captured and
  published to the response topic so the controlling host knows what happened.
- **Built-in `snap` command** — fetches `http://localhost/image.jpg` and publishes
  the raw JPEG bytes to the response topic (no external dependencies).
- **Command arguments** — `-w cmd:/path/bin:arg1:arg2` (up to 8 argv entries).
- **No shell injection** — commands launched via `fork()` + `execv()`.
- **Async execution** — watcher child processes run independently; the MQTT event
  loop is never blocked.
- **Configurable QoS** — subscription QoS level via `-q 0|1|2`.
- **Auto-reconnect** — `mosquitto_loop_forever()` with configurable back-off delay.
- **Optional TLS** — via `-s` flag; uses **mbedTLS** as the TLS backend (through
  libmosquitto compiled with mbedTLS support).
- **Dual logging** — all messages go to both stdout/stderr and syslog
  (`LOG_DAEMON` facility, tagged `mqtt-bot[PID]`).
- **Graceful shutdown** — `SIGINT` / `SIGTERM` cleanly stop the event loop.
- **Buildroot package** — `buildroot/` contains a br2-external tree ready to use
  with Buildroot for cross-compilation.

## Requirements

- C99-compatible compiler (gcc / clang)
- [libmosquitto](https://mosquitto.org/) compiled **with mbedTLS** support

On Debian/Ubuntu (native build):
```sh
apt install libmosquitto-dev
```

For cross-compilation use the provided Buildroot package (see below).

## Building (native)

```sh
make
```

Produces the `mqtt-bot` binary. To clean:
```sh
make clean
```

## Usage

```
mqtt-bot -h <host> [-p <port>] -u <user> -P <pass> [-t <topic>]
         [-r <resp_topic>] [-q <qos>] [-s [-C <cafile>]]
         -w <cmd>:<exec>[:<arg1>[:<arg2>...]] [-w ...]
```

### Options

| Flag | Description |
|------|-------------|
| `-h <host>` | MQTT broker hostname or IP address |
| `-p <port>` | Broker port (default: `1883`, or `8883` with `-s`) |
| `-u <user>` | MQTT username |
| `-P <pass>` | MQTT password |
| `-t <topic>` | Subscribe topic (default: `cmds`) |
| `-r <resp_topic>` | Topic for command output and snap images (default: same as `-t <topic>`) |
| `-q <qos>` | Subscription QoS level: `0`, `1`, or `2` (default: `0`) |
| `-s` | Enable TLS (requires mosquitto built with mbedTLS) |
| `-C <cafile>` | CA certificate file (optional; uses system store if omitted) |
| `-w <cmd>:<exec>[:<arg>...]` | Whitelist entry. May be repeated up to 64 times. Up to 8 argv entries per command. |

### Built-in commands

| Command | Description |
|---------|-------------|
| `snap` | Fetch `http://localhost/image.jpg` and publish raw JPEG bytes to `<resp_topic>` |

Built-in commands are checked **before** the whitelist.

### Examples

**Basic usage with command output:**
```sh
mqtt-bot -h mqtt.local -u admin -P secret \
         -t cam/cmds -r cam/events -q 1   \
         -w reboot:/sbin/reboot           \
         -w status:/usr/local/bin/report_status
```

**Command with arguments:**
```sh
mqtt-bot -h mqtt.local -u admin -P secret -t cam/cmds \
         -w snap_hq:/usr/bin/ffmpeg:-i:rtsp://cam/s:-frames:1:/tmp/snap.jpg
```

**With TLS:**
```sh
mqtt-bot -h mqtt.example.com -s -C /etc/certs/ca.crt \
         -u admin -P secret -t cam/cmds -r cam/events -q 1 \
         -w reboot:/sbin/reboot
```

## How It Works

1. On startup the whitelist and response topic are printed / sent to syslog.
2. A persistent connection to the MQTT broker is established (optionally TLS).
3. The bot subscribes to `<topic>` at the configured QoS.
4. For each incoming message:
   - **`snap`** — fork a watcher that fetches `http://localhost/image.jpg` via a
     raw TCP socket and publishes the binary JPEG to `<resp_topic>`.
   - **Whitelisted command** — fork a watcher that runs the command with a
     captured pipe, reads stdout + stderr, then opens a **separate** MQTT
     connection and publishes the output (+ exit code) to `<resp_topic>`.
   - **Unknown command** — logged and discarded.
5. Watcher processes are auto-reaped (`SA_NOCLDWAIT`); the event loop is never
   blocked. Each watcher self-kills after `WATCHER_TIMEOUT` (3 s) via `alarm()`.
6. Unexpected disconnects trigger automatic reconnect (5 – 10 s back-off).
7. `SIGINT` / `SIGTERM` trigger a clean disconnect and exit.

## Buildroot Integration

The `buildroot/` directory is a **br2-external** tree. It requires mosquitto to be
configured with mbedTLS as its TLS backend.

### Setup

```sh
# In your Buildroot directory:
make BR2_EXTERNAL=/path/to/mqtt-bot/buildroot menuconfig
```

Then enable:
```
External options → mqtt-bot
```

Mosquitto must be configured with:
```
Libraries → Networking → mosquitto → TLS library → mbedtls
```

### Directory layout

```
buildroot/
├── external.desc                  — br2-external metadata
├── Config.in                      — includes package Kconfig entries
├── external.mk                    — includes package makefiles
└── package/
    └── mqtt-bot/
        ├── Config.in              — Kconfig (depends on mosquitto + mbedTLS)
        └── mqtt-bot.mk            — build and install rules
```

## Limits

| Parameter | Value |
|-----------|-------|
| Max whitelist entries | 64 |
| Max argv entries per command | 8 |
| Max string length | 255 characters |
| Max captured command output | 8 KB |
| Max JPEG size (snap) | 512 KB |
| Watcher timeout | 3 seconds |
| Reconnect delay | 5 – 10 seconds |

## License

MIT — see [LICENSE](LICENSE) | OpenIPC 2026
