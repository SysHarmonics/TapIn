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


# --- Tests ---
TEST_SRC := \
    tests/test_invite.c \
    tests/test_crypto.c \
    tests/test_tapin.c

TESTS := $(TEST_SRC:.c=)

tests/test_invite: tests/test_invite.c src/socket/socket.c src/crypto/crypto.c src/invite/invite.c src/common.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_crypto: tests/test_crypto.c src/socket/socket.c src/crypto/crypto.c src/invite/invite.c src/common.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# test_tapin requires tapin.c
tests/test_tapin: tests/test_tapin.c src/socket/socket.c src/crypto/crypto.c src/invite/invite.c src/synack/tapin.c src/common.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TESTS)
	@echo "[*] Running unit tests..."
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		./$$test || echo "$$test failed"; done
