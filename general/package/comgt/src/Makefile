#
#   Makefile - build and install the comgt package
#
#   Copyright (C) 2005  Martin Gregorie,  martin@gregorie.org
#   Copyright (C) 2006  Paul Hardwick, paul@peck.org.uk
#   Copyright (C) 2022  Igor Zalatov, flyrouter@gmail.com
#

all: comgt

install:
	chmod a+x $(BIN)

clean:
	-rm *.o comgt

comgt: comgt.o
	cc comgt.o $(LDFLAGS) -o comgt

comgt.o: comgt.c comgt.h
	cc comgt.c $(CFLAGS)

comgt:  comgt.c comgt.h
	$(CC) $(CFLAGS) -o comgt $< $(LDFLAGS)

