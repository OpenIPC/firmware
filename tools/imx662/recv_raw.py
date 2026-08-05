#!/usr/bin/env python3
"""
Receiver for IMX662 raw12 stream over TCP.
Usage: python3 recv_raw.py <soc_ip> [port]

Modes: d=demosaic (real color), b=bayer raw, g=grayscale
Controls: q=quit, s=screenshot, +/-=brightness
"""

import sys
import struct
import socket
import numpy as np

try:
    import cv2
except ImportError:
    print("Need opencv-python: pip3 install opencv-python")
    sys.exit(1)

WIDTH = 1920
HEIGHT = 1080

def recv_all(sock, n):
    data = b''
    while len(data) < n:
        chunk = sock.recv(n - len(data))
        if not chunk:
            return None
        data += chunk
    return data


def unpack_raw12(buf):
    """Unpack raw12 packed (3 bytes/2 pixels) → 16-bit array (HEIGHT x WIDTH)."""
    raw = np.frombuffer(buf, dtype=np.uint8)
    n_pixels = (len(raw) // 3) * 2
    triplets = raw[:n_pixels * 3 // 2].reshape(-1, 3)

    hi = triplets[:, 0].astype(np.uint16)
    mid_lo = triplets[:, 1].astype(np.uint16)
    lo = triplets[:, 2].astype(np.uint16)

    p0 = (hi << 4) | (mid_lo >> 4)
    p1 = ((mid_lo & 0x0F) << 8) | lo

    pixels = np.empty(n_pixels, dtype=np.uint16)
    pixels[0::2] = p0
    pixels[1::2] = p1
    return pixels.reshape(HEIGHT, WIDTH)


def to_demosaic(bayer_16):
    """Use OpenCV's built-in demosaicing (BayerRG → BGR)."""
    bayer_8 = (bayer_16 >> 4).astype(np.uint8)
    bgr = cv2.cvtColor(bayer_8, cv2.COLOR_BayerRG2BGR)
    return bgr


def to_bayer_display(bayer_16):
    """Raw bayer visualization: each pixel colored by its channel."""
    bayer_8 = (bayer_16 >> 4).astype(np.uint8)
    rgb = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
    rgb[0::2, 0::2, 2] = bayer_8[0::2, 0::2]  # R
    rgb[0::2, 1::2, 1] = bayer_8[0::2, 1::2]  # G
    rgb[1::2, 0::2, 1] = bayer_8[1::2, 0::2]  # G
    rgb[1::2, 1::2, 0] = bayer_8[1::2, 1::2]  # B
    return rgb


def to_gray(bayer_16):
    """Simple grayscale from raw bayer."""
    return (bayer_16 >> 4).astype(np.uint8)


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <soc_ip> [port]")
        sys.exit(1)

    ip = sys.argv[1]
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 5000

    print(f"Connecting to {ip}:{port}...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    sock.connect((ip, port))
    print("Connected! Receiving frames...")
    print("Modes: d=demosaic(color) b=bayer g=grayscale | q=quit s=screenshot +/-=brightness")

    mode = 'd'
    brightness = 0
    screenshot_count = 0
    frame_count = 0

    while True:
        hdr = recv_all(sock, 12)
        if hdr is None:
            print("Connection closed")
            break

        w, h, stride, _, size = struct.unpack('<HHHHI', hdr)
        if w == 0 or h == 0 or size > 10_000_000:
            print(f"Bad header: w={w} h={h} size={size}")
            break

        data = recv_all(sock, size)
        if data is None:
            print("Connection closed during frame")
            break

        if len(data) != size:
            print(f"Incomplete frame: got {len(data)} expected {size}")
            break

        try:
            bayer_16 = unpack_raw12(data)

            if mode == 'd':
                img = to_demosaic(bayer_16)
            elif mode == 'b':
                img = to_bayer_display(bayer_16)
            else:
                gray = to_gray(bayer_16)
                img = cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)

            if brightness != 0:
                img = np.clip(img.astype(np.int16) + brightness, 0, 255).astype(np.uint8)

            fps_text = f"Frame {frame_count} | Mode: {mode}"
            cv2.putText(img, fps_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
            cv2.imshow('IMX662 Stream', img)
        except Exception as e:
            print(f"Display error: {e}")
            continue

        frame_count += 1
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
        elif key == ord('s'):
            fname = f"screenshot_{screenshot_count:04d}.png"
            cv2.imwrite(fname, img)
            print(f"Saved {fname}")
            screenshot_count += 1
        elif key == ord('d'):
            mode = 'd'
            print("Mode: demosaic (color)")
        elif key == ord('g'):
            mode = 'g'
            print("Mode: grayscale")
        elif key == ord('b'):
            mode = 'b'
            print("Mode: bayer raw")
        elif key == ord('+') or key == ord('='):
            brightness = min(brightness + 10, 200)
            print(f"Brightness: {brightness}")
        elif key == ord('-'):
            brightness = max(brightness - 10, -200)
            print(f"Brightness: {brightness}")

    sock.close()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
