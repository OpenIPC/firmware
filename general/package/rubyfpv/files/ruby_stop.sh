echo -n "Stopping Ruby..."
pidof ruby_start | xargs kill -9 2>/dev/null
pidof ruby_rx_commands | xargs kill -9 2>/dev/null
pidof ruby_tx_telemetry | xargs kill -9 2>/dev/null
pidof ruby_rt_vehicle | xargs kill -9 2>/dev/null
pidof ruby_start | xargs kill -9 2>/dev/null
pidof ruby_logger | xargs kill -9 2>/dev/null

pgrep ruby_start | xargs kill -9 2>/dev/null
pgrep ruby_rx_commands | xargs kill -9 2>/dev/null
pgrep ruby_tx_telemetry | xargs kill -9 2>/dev/null
pgrep ruby_rt_vehicle | xargs kill -9 2>/dev/null
pgrep ruby_start | xargs kill -9 2>/dev/null
pgrep ruby_logger | xargs kill -9 2>/dev/null
echo "OK"
