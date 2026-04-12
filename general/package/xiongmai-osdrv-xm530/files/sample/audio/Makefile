# xm510 sample Makefile
CROSS	= arm-xm-linux-
CC	= @echo " GCC	$@"; $(CROSS)gcc
CPP	= @echo " G++	$@"; $(CROSS)g++
LD	= @echo " LD	$@"; $(CROSS)ld
AR	= @echo " AR	$@"; $(CROSS)ar
RM	= @echo " RM	$@"; rm -f

REL_LIB = ../../lib

CFLAGS += -I../../include
CFLAGS += -I.
CFLAGS += -Wall -g

LDFLAGS += -lpthread

# target source
SRC  := $(wildcard *.c) 
OBJ  := $(SRC:%.c=%.o)

TARGET := sample_audio
.PHONY : clean all

all: $(TARGET)

MPI_LIBS := $(REL_LIB)/*.a

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^  -Xlinker "-(" $(MPI_LIBS) -Xlinker "-)"

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
