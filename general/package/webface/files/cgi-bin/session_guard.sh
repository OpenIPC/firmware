#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# session_guard.sh — include guard for protected CGI pages
#

source /var/www/cgi-bin/auth_common.sh

TOKEN=$(get_cookie "$SESSION_COOKIE")
AUTHENTICATED=0
SESSION_USER=""

if [ -n "$TOKEN" ] && validate_session "$TOKEN"; then
  AUTHENTICATED=1
fi

if [ "$AUTHENTICATED" != "1" ]; then
  echo "Status: 302"
  echo "Location: ${LOGIN_PAGE}"
  echo "Cache-Control: no-store"
  csp_header
  echo ""
  exit 0
fi
