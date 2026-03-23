# OpenIPC Matter IP Camera Server

Part of the [OpenIPC](https://openipc.org) project — open-source firmware and tooling for IP cameras and embedded Linux devices.

## Overview

A minimal, self-contained Matter-compatible server for IP cameras, written in C (POSIX / C99). It implements a subset of the Matter 1.2 specification sufficient to make an IP camera discoverable and controllable by Matter controllers (Apple Home, Google Home, Amazon Alexa, etc.) without depending on the official Matter SDK.

## Features

- **Matter Interaction Model** over TCP (port 5540)
- **Clusters**: OnOff, LevelControl, BasicInformation, OccupancySensing, Identify
- **mDNS announcement** — broadcasts `_matter._tcp.local` every 30 seconds (port 5353)
- **RTSP URL attribute** — exposes the camera stream URL as a Matter attribute
- **Motion detection simulation** — periodically toggles OccupancySensing::Occupancy
- **Simplified PASE commissioning stub** — responds to PBKDFParamRequest with StatusReport OK
- **Statistics thread** — logs device status every 60 seconds
- Multi-client TCP server with per-client threads
- No external dependencies — pure C99 / POSIX

## Build

```sh
gcc -Wall -Wextra -O2 -pthread matter-server.c -o matter-server
```

For embedded targets (e.g. OpenIPC buildroot):

```sh
$(CROSS_COMPILE)gcc -Wall -Wextra -O2 -pthread matter-server.c -o matter-server
```

## Usage

```sh
./matter-server [rtsp_url] [port]
```

**Examples:**

```sh
# Default RTSP URL and port 5540
./matter-server

# Custom RTSP stream
./matter-server rtsp://192.168.1.100:554/stream

# Custom RTSP stream and port
./matter-server rtsp://192.168.1.100:554/stream 5540
```

## Matter Device Parameters

| Parameter      | Value      |
|----------------|------------|
| Vendor ID      | `0xFFF1`   |
| Product ID     | `0x8001`   |
| Device Type    | `0x0043` (Video Camera) |
| Discriminator  | `3840`     |
| Passcode       | `20202021` |
| TCP Port       | `5540`     |

## Supported Clusters

| Cluster ID | Name              | Supported Attributes / Commands             |
|------------|-------------------|---------------------------------------------|
| `0x0028`   | BasicInformation  | VendorName, ProductName, NodeLabel, HWVersion, SWVersion |
| `0x0003`   | Identify          | Identify command                            |
| `0x0006`   | OnOff             | OnOff attribute; On / Off / Toggle commands |
| `0x0008`   | LevelControl      | CurrentLevel attribute; MoveToLevel command |
| `0x0406`   | OccupancySensing  | Occupancy attribute (simulated)             |

## Notes

- This is a **prototype implementation** — PASE/CASE cryptography (SPAKE2+) is stubbed out. The server accepts commissioning requests but does not perform real cryptographic handshakes.
- The TLV parser uses a simplified byte-scan heuristic, not a full recursive TLV decoder.
- Intended for development, testing, and integration on OpenIPC-based hardware.

## License

This software is part of the OpenIPC project and is distributed under the MIT License.

Copyright (c) OpenIPC Contributors  https://openipc.org  tech@openipc.eu
