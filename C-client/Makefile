CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
SRCS = main.c udp_client.c record_store.c protocol.c
OBJS = $(SRCS:.c=.o)
TARGET = uut_tester

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
