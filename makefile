CC = gcc
BINARY = chat
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lcrypt -lssl -lcrypto

SRCS := $(shell find . -name  "*.c")
HEADERS := $(shell find -name "*.h")
OBJS := $(SRCS:.c=.o)

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o

fmt:
	clang-format -i $(SRCS) $(HEADERS)
