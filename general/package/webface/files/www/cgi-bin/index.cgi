#!/usr/bin/haserl
<%
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# index.cgi — dashboard page (protected)
#

source /var/www/cgi-bin/session_guard.sh

HOSTNAME=$(cat /etc/hostname 2>/dev/null || echo "camera")
UPTIME=$(uptime 2>/dev/null | sed 's/.*up /up /' | sed 's/,  load.*//')
KERNEL=$(uname -r 2>/dev/null)
MEMFREE=$(grep MemAvailable /proc/meminfo 2>/dev/null | awk '{printf "%.1f MB", $2/1024}')

echo "Content-Type: text/html"
echo "Cache-Control: no-store"
csp_header
echo ""
%>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title><%= $HOSTNAME %> — Dashboard</title>
<link rel="stylesheet" href="/assets/css/webface.css">
</head>
<body class="page-dashboard">
<div class="container">

  <!-- Top bar: user session info and logout -->
  <div class="topbar">
    <span class="topbar-user"><%= $SESSION_USER %>@<%= $HOSTNAME %></span>
    <a href="/cgi-bin/logout.cgi" class="logout-link">logout</a>
  </div>

  <!-- Device status grid -->
  <div class="grid">
    <div class="stat">
      <div class="stat-label">Hostname</div>
      <div class="stat-value"><%= $HOSTNAME %></div>
    </div>
    <div class="stat">
      <div class="stat-label">Uptime</div>
      <div class="stat-value"><%= $UPTIME %></div>
    </div>
    <div class="stat">
      <div class="stat-label">Kernel</div>
      <div class="stat-value"><%= $KERNEL %></div>
    </div>
    <div class="stat">
      <div class="stat-label">Free memory</div>
      <div class="stat-value"><%= $MEMFREE %></div>
    </div>
  </div>

  <!-- About OpenIPC -->
  <div class="card">
    <h2>About OpenIPC</h2>
    <p>
      <a href="https://openipc.org" target="_blank" rel="noopener">OpenIPC</a>
      is an open-source operating system targeting IP cameras and related devices.
      It replaces the vendor stock firmware with a modern, transparent, and
      community-maintained Linux-based alternative.
    </p>

    <h3>Key Features</h3>
    <ul>
      <li>Support for a wide range of SoCs: HiSilicon, Ingenic, SigmaStar, Goke, XM, and more</li>
      <li>Modular design — enable only the components you need</li>
      <li>Streamer options: <code>majestic</code> (native high-performance streamer)</li>
      <li>RTSP, ONVIF, and web-based live preview</li>
      <li>Built-in support for wireless FPV video links</li>
      <li>Telemetry, OSD overlay, and MAVLink integration for drones</li>
      <li>Lightweight web UI for camera configuration</li>
    </ul>

    <h3>Architecture</h3>
    <p>
      OpenIPC is built on top of <a href="https://buildroot.org" target="_blank" rel="noopener">Buildroot</a>
      and produces minimal root filesystem images (8–16 MB) suitable for devices
      with limited flash and RAM. The firmware is designed to boot fast and run
      reliably on embedded hardware.
    </p>

    <h3>Web Interface Authentication</h3>
    <p>
      This login system is a sample implementation for OpenIPC devices.
      It uses <code>any httpd server</code> + <code>haserl</code> for CGI processing,
      SHA-256 password hashing, and cookie-based sessions — no Basic-auth required.
      Session files are stored on <code>tmpfs</code> and automatically expire.
    </p>

    <h3>Useful Links</h3>
    <ul>
      <li><a href="https://openipc.org" target="_blank" rel="noopener">openipc.org</a> — Official website</li>
      <li><a href="https://github.com/OpenIPC" target="_blank" rel="noopener">github.com/OpenIPC</a> — Source code repositories</li>
      <li><a href="https://openipc.org/wiki" target="_blank" rel="noopener">Wiki</a> — Documentation and guides</li>
      <li><a href="https://openipc.org/supported-hardware" target="_blank" rel="noopener">Supported hardware</a> — List of compatible devices</li>
    </ul>
  </div>

  <div class="footer">webface v1.0.0 &middot; OpenIPC</div>

</div>
</body>
</html>
