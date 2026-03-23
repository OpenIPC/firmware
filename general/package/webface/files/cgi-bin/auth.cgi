#!/usr/bin/haserl
<%
# auth.cgi — Session-based authentication (POST=login, GET=status)
# Copyright (c) OpenIPC  https://openipc.org  MIT License

source /var/www/cgi-bin/auth_common.sh

check_session() {
  local token
  token=$(get_cookie "$SESSION_COOKIE")
  [ -z "$token" ] && return 1
  validate_session "$token"
}

# ── POST: login ──────────────────────────────────────────────
if [ "$REQUEST_METHOD" = "POST" ]; then

  cleanup_sessions

  # haserl exposes form fields as POST_<name> env vars
  USERNAME="$POST_username"
  PASSWORD="$POST_password"
  USERNAME=$(echo "$USERNAME" | tr -cd 'a-zA-Z0-9_-' | head -c 64)

  if [ -z "$USERNAME" ] || [ -z "$PASSWORD" ]; then
    sleep 1
    json_response 400 '{"ok":false,"error":"All fields are required"}'
  fi

  if is_ip_locked "$REMOTE_ADDR"; then
    sleep 1
    json_response 429 '{"ok":false,"error":"Too many attempts. Try again later"}'
  fi

  PASS_HASH=$(sha256 "$PASSWORD")
  MATCH=$(grep "^${USERNAME}:${PASS_HASH}$" "$PASSWD_FILE" 2>/dev/null)

  if [ -z "$MATCH" ]; then
    record_fail "$REMOTE_ADDR"
    sleep 1
    json_response 401 '{"ok":false,"error":"Invalid username or password"}'
  fi

  # Constant delay on success too (prevents timing oracle)
  sleep 1
  clear_fails "$REMOTE_ADDR"

  # Create session file
  TOKEN=$(gen_token)
  NOW=$(date +%s)
  SESSION_FILE="${SESSION_DIR}/${TOKEN}"

  cat > "$SESSION_FILE" <<EOF
user=${USERNAME}
created=${NOW}
ip=${REMOTE_ADDR}
EOF
  chmod 600 "$SESSION_FILE"

  echo "Status: 200"
  echo "Content-Type: application/json"
  echo "Cache-Control: no-store"
  csp_header
  set_session_cookie "$TOKEN"
  echo ""
  printf '{"ok":true,"redirect":"%s"}\n' "$REDIRECT_OK"
  exit 0
fi

# ── GET: session status ──────────────────────────────────────
if check_session; then
  json_response 200 '{"ok":true,"authenticated":true}'
else
  json_response 200 '{"ok":false,"authenticated":false}'
fi
%>
