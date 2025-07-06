CC=gcc
CFLAGS=-Wall -g
CLIENT_DIR = src/SocketClient
SERV_DIR = src/SocketServer

CLIENT_LIB=src/Library/Client
SERV_LIB=src/Library/Server

#Default target
all: client server


# Create client objects
$(CLIENT_DIR)/%.o: $(CLIENT_DIR)/%.c
		$(CC) -c -o $@ $< $(CFLAGS)

$(CLIENT_LIB)/%.o: $(CLIENT_LIB)/%.c $(CLIENT_LIB)/client.h
		$(CC) -c -o $@ $< $(CFLAGS)


# Create server objects
$(SERV_DIR)/%.o: $(SERV_DIR)/%.c
		$(CC) -c -o $@ $< $(CFLAGS)

$(SERV_LIB)/%.o: $(SERV_LIB)/%.c $(SERV_LIB)/server.h 
		$(CC) -c -o $@ $< $(CFLAGS)


# Object files for the client and server
CLIENT_OBJS=$(CLIENT_DIR)/main.o $(CLIENT_LIB)/client.o
SERVER_OBJS=$(SERV_DIR)/main.o $(SERV_LIB)/server.o


# Client executable
client: $(CLIENT_OBJS)
	$(CC) -o client $(CLIENT_OBJS) $(CFLAGS)


# Server executable
server: $(SERVER_OBJS)
	$(CC) -o server $(SERVER_OBJS) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(CLIENT_LIB)/*.o client $(SERVER_LIB)/*.o server
