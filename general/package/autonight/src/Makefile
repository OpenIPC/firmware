.PHONY: all clean

all: autonight

autonight: autonight.o
	$(CC) -o $@ $^

clean:
	-rm -rf *.o
