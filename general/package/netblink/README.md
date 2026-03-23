## netblink

A minimal Linux command-line utility that monitors network connectivity or
traffic and indicates status via a GPIO-driven LED using the sysfs interface.

Designed for embedded Linux systems such as routers and IP cameras.

### Modes

| Mode      | What it does |
|-----------|--------------|
| `ping`    | Periodically pings a host; blink pattern reflects RTT. |
| `traffic` | Monitors a network interface; blink rate reflects tx+rx throughput. |

---

### Features

- **Ping mode** — Blink patterns based on RTT:
  - Short blink — good connection (RTT < 50 ms)
  - Medium blink — acceptable connection (RTT < 150 ms)
  - Long blink — slow connection (RTT ≥ 150 ms)
  - Double-flash — ping failed or timed out
- **Traffic mode** — Adaptive blink rate based on tx+rx byte rate with
  Exponential Moving Average (EMA) smoothing to prevent jitter on bursts.
- GPIO pin is a mandatory argument common to both modes.
- Logs to syslog (`LOG_DAEMON` facility) and stdout simultaneously.
- Graceful shutdown on `SIGINT`/`SIGTERM` with automatic GPIO unexport.
- POSIX.1-2008 compatible (C99, uClibc / musl / glibc).

---

### Requirements

- Linux kernel with GPIO sysfs support (`/sys/class/gpio`).
- `ping` utility in `PATH` (ping mode only).
- Write access to `/sys/class/gpio` (root or custom udev rules).
- C99-capable compiler (`gcc`) and `make`.

---

### Build

```bash
cd src
make
```

Produces the `netblink` binary. To remove build artifacts:

```bash
make clean
```

---

### Usage

```
netblink <gpio> ping <host> [interval_sec [timeout_sec]]
netblink <gpio> traffic [interface]
```

#### Common argument

| Argument | Range | Description                  |
|----------|-------|------------------------------|
| `gpio`   | 1–150 | GPIO pin number (**required**) |

#### Ping mode options

| Argument       | Default | Range    | Description                                                                 |
|----------------|---------|----------|-----------------------------------------------------------------------------|
| `host`         | —       | required | IP or hostname. Allowed chars: letters, digits, `.` `-` `:`. Max 200 chars. |
| `interval_sec` | `60`    | 1–86400  | Seconds between pings.                                                      |
| `timeout_sec`  | `2`     | 1–30     | Ping timeout in seconds.                                                    |

#### Traffic mode options

| Argument    | Default | Description             |
|-------------|---------|-------------------------|
| `interface` | `eth0`  | Network interface name. |

---

### Examples

```bash
# Ping 8.8.8.8 every 60 s on GPIO 21 (all defaults)
sudo netblink 21 ping 8.8.8.8

# Ping 1.1.1.1 every 30 s, timeout 5 s, on GPIO 17
sudo netblink 17 ping 1.1.1.1 30 5

# Monitor eth0 traffic on GPIO 21 (default interface)
sudo netblink 21 traffic

# Monitor eth1 traffic on GPIO 17
sudo netblink 17 traffic eth1
```

---

### Ping mode — blink patterns

| Condition             | Pattern                    |
|-----------------------|----------------------------|
| RTT < 50 ms           | Short blink (100 ms)       |
| 50 ms ≤ RTT < 150 ms  | Medium blink (300 ms)      |
| RTT ≥ 150 ms          | Long blink (500 ms)        |
| Ping failed / timeout | Double-flash (200 ms × 2)  |

### Traffic mode — blink behavior

The LED blink interval is computed as:

```
delay = 200 ms / (smoothed_KB_per_poll + 1)
```

- Minimum delay: **10 ms** (heavy traffic → fast blinking)
- Maximum delay: **200 ms** (low traffic → slow blinking)
- No blink when smoothed delta < 100 bytes/poll

EMA smoothing factor: **0.3** (prevents jitter on burst traffic).

---

### Security and reliability

- The `host` argument is validated against a strict character whitelist before
  being passed to `popen`, preventing shell injection.
- All numeric arguments are parsed with `strtol` with full error detection.
- `LC_ALL=C` is set for `ping` invocation to ensure consistent output format.
- `ping` output is parsed in C (searching for `time=`) — no `grep`/`awk` needed.
- On `SIGINT`/`SIGTERM` the GPIO pin is cleanly unexported before exit.
- Logs to syslog (`LOG_DAEMON`) and stdout for both interactive and daemon use.
