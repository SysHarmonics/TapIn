CC       := gcc
CFLAGS   := -Wall -O2 -g -std=c11 -Isrc
LDFLAGS  := -lsodium -lpthread
BIN      := tapin

SRC := \
    src/main.c \
    src/socket/socket.c \
    src/crypto/crypto.c \
    src/synack/tapin.c \
	src/invite/invite.c \
    src/common.c
	src/lib/colorize/colorize.c \
	src/invite/invite.c

OBJ := $(SRC:.c=.o)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    BREW_PREFIX := $(shell brew --prefix libsodium)
    CFLAGS  += -I$(BREW_PREFIX)/include
    LDFLAGS += -L$(BREW_PREFIX)/lib
endif

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN) $(TESTS)

test:
	./tests/test_runner.bash
