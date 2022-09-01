ARCH ?= $(shell uname -i)
VERSION ?= $(shell git rev-parse HEAD)

LDFLAGS := -lrt -lpcap -lsodium
CFLAGS := -Wall -O2 -DWFB_VERSION='"$(VERSION)"'

all: wfb_rx wfb_tx wfb_keygen

src/%.o: src/%.c src/*.h
	$(CC) $(CFLAGS) -std=gnu99 -c -o $@ $<

src/%.o: src/%.cpp src/*.hpp src/*.h
	$(CXX) $(CFLAGS) -std=gnu++11 -c -o $@ $<

wfb_rx: src/rx.o src/radiotap.o src/fec.o src/wifibroadcast.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wfb_tx: src/tx.o src/fec.o src/wifibroadcast.o
	$(CXX) -o $@ $^ $(LDFLAGS)

wfb_keygen: src/keygen.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf wfb_rx wfb_tx wfb_keygen *~ src/*.o
