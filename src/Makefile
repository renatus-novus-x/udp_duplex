TARGET = udp_duplex.x

CROSS = m68k-xelf-
CC = $(CROSS)gcc
LD = $(CROSS)gcc
CFLAGS = -Wall -O2 -g
LDFLAGS =
LIBS = -lsocket

SRCS = udp_duplex.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	
%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f $(TARGET) $(OBJS) *.elf

.PHONY: all clean
