#!/usr/bin/haserl
<%
# logout.cgi — Destroy session and redirect to login
# Copyright (c) OpenIPC  https://openipc.org  MIT License

source /var/www/cgi-bin/auth_common.sh

TOKEN=$(get_cookie "$SESSION_COOKIE")

if [ -n "$TOKEN" ]; then
  SAFE_TOKEN=$(echo "$TOKEN" | tr -cd '0-9a-f' | head -c 64)
  [ -n "$SAFE_TOKEN" ] && rm -f "${SESSION_DIR}/${SAFE_TOKEN}"
fi

echo "HTTP/1.1 302"
echo "Location: ${LOGIN_PAGE}"
echo "Set-Cookie: ${SESSION_COOKIE}=deleted; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT; HttpOnly; SameSite=Strict"
echo "Cache-Control: no-store"
csp_header
echo ""
%>
