TARGET := ruby_start ruby_logger ruby_rt_vehicle ruby_rx_commands ruby_tx_telemetry

all: $(TARGET)

$(TARGET):
	@mkdir -p output
	$(CC) ruby_start.c -o output/$@ -s
