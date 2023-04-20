LIBDIR=lib
LIBS=$(LIBDIR)/libimp.a $(LIBDIR)/libalog.a

all: playaudio

playaudio: playaudio.o
	$(CC) -o $@ $^ $(LIBS)

clean:
	-rm playaudio *.o
