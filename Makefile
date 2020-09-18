CFLAGS=-Iinclude -D_GNU_SOURCE --std=gnu11 -g
SRCS=$(wildcard */*.c)

OBJS=$(SRCS:.c=.o)

all: $(OBJS)
	$(LD) -r */*.o -o libPlexMessage.o

clean:
	rm -fv */*.o libPlexMessage.o