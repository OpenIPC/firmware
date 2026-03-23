#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# auth_common.sh — shared configuration and utility functions
#

# ── Configuration ────────────────────────────────────────────

SESSION_DIR="/tmp/sessions"
SESSION_COOKIE="SESSIONID"
SESSION_TTL=3600                  # seconds (1 hour)
LOGIN_PAGE="/login.html"
PASSWD_FILE="/etc/webface.passwd"   # user:sha256hash
REDIRECT_OK="/cgi-bin/index.cgi"

AUTH_FAIL_DIR="/tmp/auth_fails"
AUTH_MAX_ATTEMPTS=5
AUTH_LOCKOUT_SEC=300              # seconds (5 min)

CSP_POLICY="default-src 'self'; style-src 'self'; script-src 'self'; img-src 'self'; frame-ancestors 'none'; form-action 'self'"

mkdir -p "$SESSION_DIR" "$AUTH_FAIL_DIR"

# ── Helpers ──────────────────────────────────────────────────

# Extract a named cookie value from HTTP_COOKIE header
get_cookie() {
  local name="$1"
  echo "$HTTP_COOKIE" | tr ';' '\n' | sed 's/^ //' \
    | grep "^${name}=" | head -1 | cut -d= -f2-
}

# Generate 32-char hex token (128 bits) from /dev/urandom
gen_token() {
  dd if=/dev/urandom bs=16 count=1 2>/dev/null | hexdump -e '"%02x"'
}

# SHA-256 hash → 64-char lowercase hex digest
sha256() {
  printf '%s' "$1" | sha256sum | awk '{print $1}'
}

# ── HTTP response helpers ────────────────────────────────────

csp_header() {
  echo "Content-Security-Policy: ${CSP_POLICY}"
}

# Send JSON response with security headers and exit
json_response() {
  local code="$1" body="$2"
  echo "Status: ${code}"
  echo "Content-Type: application/json"
  echo "Cache-Control: no-store"
  csp_header
  echo ""
  echo "$body"
  exit 0
}

# Set HttpOnly + SameSite session cookie
set_session_cookie() {
  local token="$1"
  echo "Set-Cookie: ${SESSION_COOKIE}=${token}; Path=/; HttpOnly; SameSite=Strict"
}

# ── Session validation ───────────────────────────────────────
# Sanitize token → check file → check TTL → check IP.
# Sets SESSION_USER on success (return 0), returns 1 on failure.

validate_session() {
  local token="$1"
  SESSION_USER=""

  # Hex-only filter prevents path traversal (e.g. "../../etc/passwd")
  token=$(printf '%s' "$token" | tr -cd '0-9a-f' | head -c 64)
  [ -z "$token" ] && return 1

  local sfile="${SESSION_DIR}/${token}"
  [ -f "$sfile" ] || return 1

  local created now age
  created=$(grep "^created=" "$sfile" 2>/dev/null | cut -d= -f2)
  [ -z "$created" ] && { rm -f "$sfile"; return 1; }
  now=$(date +%s)
  age=$(( now - created ))

  if [ "$age" -gt "$SESSION_TTL" ]; then
    rm -f "$sfile"
    return 1
  fi

  # IP binding
  local sess_ip
  sess_ip=$(grep "^ip=" "$sfile" 2>/dev/null | cut -d= -f2)
  if [ -n "$sess_ip" ] && [ "$sess_ip" != "$REMOTE_ADDR" ]; then
    rm -f "$sfile"
    return 1
  fi

  # Sliding expiration — refresh timestamp
  SESSION_USER=$(grep "^user=" "$sfile" 2>/dev/null | cut -d= -f2)
  sed -i "s/^created=.*/created=${now}/" "$sfile"
  return 0
}

# ── Session cleanup ──────────────────────────────────────────
# Remove expired session files (called on each login attempt)

cleanup_sessions() {
  local now sf created
  now=$(date +%s)
  for sf in "${SESSION_DIR}"/*; do
    [ -f "$sf" ] || continue
    created=$(grep "^created=" "$sf" 2>/dev/null | cut -d= -f2)
    [ -z "$created" ] && { rm -f "$sf"; continue; }
    if [ "$(( now - created ))" -gt "$SESSION_TTL" ]; then
      rm -f "$sf"
    fi
  done
}

# ── Rate limiting ────────────────────────────────────────────
# Per-IP failure tracking: file contains count (line 1) and timestamp (line 2)

_safe_ip() {
  printf '%s' "$1" | tr -cd '0-9a-fA-F.:' | head -c 45
}

is_ip_locked() {
  local safe_ip count ts now
  safe_ip=$(_safe_ip "$1")
  local fail_file="${AUTH_FAIL_DIR}/${safe_ip}"
  [ -f "$fail_file" ] || return 1

  count=$(head -1 "$fail_file" 2>/dev/null)
  ts=$(sed -n '2p' "$fail_file" 2>/dev/null)
  now=$(date +%s)

  if [ -n "$ts" ] && [ "$(( now - ts ))" -gt "$AUTH_LOCKOUT_SEC" ]; then
    rm -f "$fail_file"
    return 1
  fi

  [ "${count:-0}" -ge "$AUTH_MAX_ATTEMPTS" ] && return 0
  return 1
}

record_fail() {
  local safe_ip count now
  safe_ip=$(_safe_ip "$1")
  local fail_file="${AUTH_FAIL_DIR}/${safe_ip}"
  now=$(date +%s)
  count=0

  if [ -f "$fail_file" ]; then
    count=$(head -1 "$fail_file" 2>/dev/null)
    count=${count:-0}
  fi

  count=$(( count + 1 ))
  printf '%s\n%s\n' "$count" "$now" > "$fail_file"
}

clear_fails() {
  local safe_ip
  safe_ip=$(_safe_ip "$1")
  rm -f "${AUTH_FAIL_DIR}/${safe_ip}"
}
