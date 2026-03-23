# Authentication System: any httpd server + haserl

## Authentication Method

This project implements **Cookie-based Session Authentication** (stateful
server-side sessions) — a classic web authentication pattern well-suited for
resource-constrained embedded devices.

**How it works:**

1. The user submits credentials via an HTML login form (POST request).
2. The server verifies the password by comparing its SHA-256 hash against
   `/etc/webface.passwd`.
3. On success the server creates a session file in `/tmp/sessions/<random_token>`
   containing the username, creation time, and client IP address.
4. The token is sent to the browser as an HTTP cookie
   (`Set-Cookie: SESSIONID=<token>; HttpOnly; SameSite=Strict`).
5. On every subsequent request the browser automatically includes the cookie;
   the server validates the token against the session file (checking TTL and
   IP binding) and grants or denies access.

**Comparison with other approaches:**
Unlike **Basic Auth**, which transmits base64-encoded credentials with every
request and relies on the browser's built-in dialog, cookie-based sessions
authenticate only once and then use an opaque token — giving full control over
the login UI, session lifetime, and logout.  Unlike **JWT** (JSON Web Tokens),
where all session data is encoded inside the token itself (stateless), this
approach stores session state on the server (stateful), which makes it trivial
to invalidate individual sessions by simply deleting a file — an important
advantage on embedded systems where simplicity matters more than horizontal
scalability.

---

Designed as a sample authentication module for [OpenIPC](https://openipc.org)
firmware.  Consists of 9 files:

```
/var/www/
├── index.html                  ← redirect to login page (root entry point)
├── login.html                  ← login page (HTML structure)
├── assets/
│   ├── css/
│   │   └── webface.css         ← unified styles (login + dashboard)
│   └── js/
│       └── login.js            ← login form handler (JS)
└── cgi-bin/
    ├── auth_common.sh          ← shared configuration and utilities
    ├── auth.cgi                ← POST handler for login (haserl)
    ├── index.cgi               ← protected dashboard page (example)
    ├── logout.cgi              ← session termination
    └── session_guard.sh        ← include guard for protected pages
```

---

## 1. File Installation

```sh
# Create target directories
mkdir -p /var/www/cgi-bin /var/www/assets/css /var/www/assets/js

# Place files
cp index.html /var/www/index.html
cp login.html /var/www/login.html
cp assets/css/webface.css /var/www/assets/css/webface.css
cp assets/js/login.js /var/www/assets/js/login.js
cp cgi-bin/auth_common.sh /var/www/cgi-bin/auth_common.sh
cp cgi-bin/auth.cgi /var/www/cgi-bin/auth.cgi
cp cgi-bin/index.cgi /var/www/cgi-bin/index.cgi
cp cgi-bin/logout.cgi /var/www/cgi-bin/logout.cgi
cp cgi-bin/session_guard.sh /var/www/cgi-bin/session_guard.sh

# Set permissions
chmod 755 /var/www/cgi-bin/auth.cgi /var/www/cgi-bin/index.cgi /var/www/cgi-bin/logout.cgi
chmod 644 /var/www/cgi-bin/auth_common.sh /var/www/cgi-bin/session_guard.sh
chmod 644 /var/www/index.html /var/www/login.html /var/www/assets/css/webface.css /var/www/assets/js/login.js
```

---

## 2. Create Password File

Each line: `username:sha256(password)`

```sh
# Generate password hash
echo -n "mypassword" | sha256sum | awk '{print $1}'
# → e3b0c44298fc1c149afb...

# Create file (outside webroot!)
echo "admin:e3b0c44298fc1c149afb..." > /etc/webface.passwd
chmod 600 /etc/webface.passwd

# Add additional users
echo "user2:$(echo -n 'pass2' | sha256sum | awk '{print $1}')" >> /etc/webface.passwd
```

---

## 3. Use together with Majestic

Enable specifying the directory for use **Cookie-based Session Authentication** and
disable **Basic Auth**.

```
cli -s .system.staticDir /www
```

---

## 4. Protecting Pages (Example)

Source `session_guard.sh` at the top of any CGI script that requires authentication.
See `index.cgi` for a complete working example.

```sh
#!/usr/bin/haserl
<%
  # Redirects to login.html if session is invalid; sets $SESSION_USER on success
  source /var/www/cgi-bin/session_guard.sh
%>
Content-Type: text/html

<!DOCTYPE html>
<html>
<body>
  <h1>Welcome, <%= $SESSION_USER %>!</h1>
  <a href="/cgi-bin/logout.cgi">Log out</a>
</body>
</html>
```

---

## 5. How Authentication Works

```
Browser                     any httpd server / haserl
   │                                   │
   │  GET /                            │
   │──────────────────────────────────►│  Serves index.html
   │◄──────────────────────────────────│  (meta refresh → /login.html)
   │                                   │
   │  GET /login.html                  │
   │──────────────────────────────────►│  Serves static HTML
   │◄──────────────────────────────────│
   │                                   │
   │  POST /cgi-bin/auth.cgi           │
   │  Body: username=&password=        │
   │──────────────────────────────────►│  Computes sha256(password)
   │                                   │  Looks up /etc/webface.passwd
   │                                   │  Creates /tmp/sessions/<token>
   │  200 + Set-Cookie: SESSIONID      │
   │◄──────────────────────────────────│
   │                                   │
   │  GET /cgi-bin/index.cgi           │
   │  Cookie: SESSIONID=<token>        │
   │──────────────────────────────────►│  session_guard.sh validates
   │                                   │  /tmp/sessions/<token>
   │  200 Protected page               │
   │◄──────────────────────────────────│
```

---

## 6. Configuration

All settings are centralized in `auth_common.sh`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SESSION_DIR` | `/tmp/sessions` | Directory for session files |
| `SESSION_TTL` | `3600` | Session lifetime in seconds |
| `PASSWD_FILE` | `/etc/webface.passwd` | Path to credentials file |
| `AUTH_MAX_ATTEMPTS` | `5` | Failed logins before IP lockout |
| `AUTH_LOCKOUT_SEC` | `300` | Lockout duration in seconds |
| `REDIRECT_OK` | `/cgi-bin/index.cgi` | Redirect target after login |

---

## 7. Security

| Measure | Implementation |
|---------|---------------|
| Password not sent in cleartext (with HTTPS) | fetch POST → body |
| Passwords stored as SHA-256 hashes | /etc/webface.passwd |
| Cookie HttpOnly + SameSite=Strict | Set-Cookie header |
| Constant delay on every login attempt | sleep 1 (prevents timing oracle) |
| Token from /dev/urandom (128 bits) | gen_token() |
| Sessions expire after TTL | SESSION_TTL=3600 |
| Session IP binding | REMOTE_ADDR validated on each request |
| Token sanitization (hex only) | tr -cd '0-9a-f' (prevents path traversal) |
| Username sanitization | tr -cd 'a-zA-Z0-9_-' |
| Rate limiting per IP | 5 attempts, then 5 min lockout |
| Expired session cleanup | On every login attempt |
| Content-Security-Policy | CSP header on all responses + meta tag |

> ⚠️  For production use, HTTPS is strongly recommended.

---

## 8. Dependencies

- `httpd` — any httpd server with CGI support (included in busybox)
- `haserl` — CGI processor (available in OpenWrt and OpenIPC/Buildroot)
- `sha256sum` (included in busybox)
- `hexdump` (included in busybox)

---

## 9. About OpenIPC

[OpenIPC](https://openipc.org) is an open-source Linux firmware for IP cameras.
This authentication module is designed as a sample web UI component for OpenIPC devices
running any httpd server.  See `index.cgi` for a working dashboard example.

---

## License

Copyright (c) [OpenIPC](https://openipc.org).
Released under the [MIT License](https://opensource.org/licenses/MIT).
