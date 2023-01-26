LIB_NAME := libhisicompat

SRCS := sTaT.c

OBJS := sTaT.o

all: $(LIB_NAME).so

$(LIB_NAME).so: $(OBJS) 
	$(CC) -shared -o $@ $(OBJS)

clean:
	@rm -f $(OBJS) $(LIB_NAME).so

