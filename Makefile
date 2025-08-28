CC       := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wcast-align -fanalyzer -O2 -std=c11
INCLUDES = -I src -I src/lib/colorize
LDFLAGS  := -lsodium -lpthread
BIN      := tapin

SRC := \
    src/main.c \
    src/socket/socket.c \
    src/crypto/crypto.c \
    src/synack/tapin.c \
	src/invite/invite.c \
    src/common.c \
	src/lib/colorize/colorize.c

OBJ := $(SRC:.c=.o)

SRC_FILES := $(shell find src -name '*.c')

LINKER_FILES := $(shell find src -name "*.c" ! -name "main.c")

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


show-warns:
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_FILES)

clean:
	rm -f $(OBJ) $(BIN) $(TESTS)

test:
	./tests/test_runner.bash
