#!/bin/sh

wait_for_pid_exit() {
    pid="$1"
    timeout="${2:-15}"
    i=0
    while [ "$i" -lt "$timeout" ]; do
        [ -z "$pid" ] && return 0
        kill -0 "$pid" 2>/dev/null || return 0
        sleep 1
        i=$((i + 1))
    done
    return 1
}

wait_for_name_exit() {
    name="$1"
    timeout="${2:-15}"
    i=0
    while [ "$i" -lt "$timeout" ]; do
        pidof "$name" >/dev/null 2>&1 || return 0
        sleep 1
        i=$((i + 1))
    done
    return 1
}

read_pidfile() {
    pidfile="$1"
    [ -f "$pidfile" ] || return 1
    pid="$(cat "$pidfile" 2>/dev/null)"
    [ -n "$pid" ] || return 1
    echo "$pid"
}

is_running_pidfile() {
    name="$1"
    pidfile="$2"
    pid="$(read_pidfile "$pidfile" 2>/dev/null)" || return 1
    kill -0 "$pid" 2>/dev/null || return 1
    cmdline="$(tr '\0' ' ' < /proc/$pid/cmdline 2>/dev/null)"
    echo "$cmdline" | grep -q "$name" || return 1
    return 0
}

ensure_pidfile() {
    name="$1"
    pidfile="$2"
    if is_running_pidfile "$name" "$pidfile"; then
        return 0
    fi
    rm -f "$pidfile"
    for pid in $(pidof "$name" 2>/dev/null); do
        cmdline="$(tr '\0' ' ' < /proc/$pid/cmdline 2>/dev/null)"
        echo "$cmdline" | grep -q "$name" || continue
        echo "$pid" > "$pidfile"
        return 0
    done
    return 1
}

start_service() {
    name="$1"
    pidfile="$2"
    shift 2

    pids="$(pidof "$name" 2>/dev/null)"
    if [ -n "$pids" ]; then
        count="$(echo "$pids" | wc -w)"
        if [ "$count" -gt 1 ]; then
            echo "$name has $count instances; forcing clean restart"
            stop_service "$name" "$pidfile" 12 || return 1
        elif ensure_pidfile "$name" "$pidfile"; then
            echo "$name is already running"
            return 0
        fi
    fi

    rm -f "$pidfile"
    start-stop-daemon -b -m -S -q -p "$pidfile" -x "$name" -- "$@"
    if [ $? -ne 0 ]; then
        echo "Failed to launch $name"
        return 1
    fi

    i=0
    while [ "$i" -lt 8 ]; do
        if ensure_pidfile "$name" "$pidfile"; then
            return 0
        fi
        sleep 1
        i=$((i + 1))
    done
    echo "$name did not stay running"
    return 1
}

stop_service() {
    name="$1"
    pidfile="$2"
    grace="${3:-12}"

    pid="$(read_pidfile "$pidfile" 2>/dev/null)"

    if [ -n "$pid" ]; then
        kill -TERM "$pid" 2>/dev/null || true
    fi
    killall -q -TERM "$name" 2>/dev/null || true

    if [ -n "$pid" ]; then
        wait_for_pid_exit "$pid" "$grace" || true
    fi
    wait_for_name_exit "$name" "$grace" || true

    if pidof "$name" >/dev/null 2>&1; then
        killall -q -KILL "$name" 2>/dev/null || true
        wait_for_name_exit "$name" 5 || {
            echo "Failed to stop $name"
            return 1
        }
    fi

    rm -f "$pidfile"
    return 0
}

wait_for_http_ready() {
    url="$1"
    timeout="${2:-20}"
    i=0
    while [ "$i" -lt "$timeout" ]; do
        curl -sf --connect-timeout 1 --max-time 2 "$url" >/dev/null 2>&1 && return 0
        sleep 1
        i=$((i + 1))
    done
    return 1
}
