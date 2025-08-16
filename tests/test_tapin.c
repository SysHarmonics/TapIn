#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sodium.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "synack/tapin.h"
#include "invite/invite.h"

void print_key(const char *label, const unsigned char key[32]) {
    printf("%s: ", label);
    for (int i = 0; i < 32; ++i)
        printf("%02x", key[i]);
    printf("\n");
}

void test_key_exchange() {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        perror("socketpair");
        exit(1);
    }

    int key_pipe[2];
    assert(pipe(key_pipe) == 0);


    unsigned char server_rx[32], server_tx[32];
    unsigned char client_tx_copy[32], client_rx_copy[32];

    // Copies to protect against memory overwrite after wait()
    // unsigned char client_tx_copy[32], server_rx_copy[32];
    // unsigned char client_rx_copy[32], server_tx_copy[32];

    char invite[INVITE_LEN];
    const char *password = "hunter2";
    const char *ip = "127.0.0.1";
    const char *port = "4444";

    assert(invite_generate(invite, sizeof invite, password, ip, port) == 0);

    pid_t pid = fork();
    if (pid == 0) {
        // client
        close(fds[0]);
        close(key_pipe[0]);
        
        printf("[Client] Starting tapped_in...\n");

        unsigned char client_rx[32], client_tx[32];
        int result = tapped_in(fds[1], 1, invite, password, client_rx, client_tx);

        printf("[Client] tapped_in returned: %d\n", result);

        if (result != 0) {
            fprintf(stderr, "[Client] tapped_in failed!\n");
            exit(1);
        }

        printf("[Client] writing client_tx to pipe...\n");
        assert(write(key_pipe[1], client_tx, 32) == 32);
        printf("[Client] writing client_rx to pipe...\n");
        assert(write(key_pipe[1], client_rx, 32) == 32);
        printf("[Client] done writing to pipe\n");
        close(key_pipe[1]);  // We're done writing

        // save backup copy of client_tx to compare later
       // memcpy(client_tx_copy, client_tx, 32);
       // memcpy(client_rx_copy, client_rx, 32);

        exit(0);
    } else {
        // server
        close(fds[1]);
        close(key_pipe[1]);
        
        // Save server side keys
        // memcpy(server_tx_copy, server_tx, 32);
        // memcpy(server_rx_copy, server_rx, 32);
        memset(client_tx_copy, 0, 32);
        memset(client_rx_copy, 0, 32);

        assert(tapped_in(fds[0], 0, invite, password, server_rx, server_tx) == 0);

        printf("[Server] reading client_tx from pipe...\n");
        assert(read(key_pipe[0], client_tx_copy, 32) == 32);
        printf("[Server] reading client_rx from pipe...\n");
        assert(read(key_pipe[0], client_rx_copy, 32) == 32);
        printf("[Server] done reading from pipe\n");
        close(key_pipe[0]);


        wait(NULL);


        // Debug output
        print_key("client_tx", client_tx_copy);
        print_key("server_rx", server_rx);
        print_key("server_tx", server_tx);
        print_key("client_rx", client_rx_copy);

        // 🔐 Final assertions
        assert(memcmp(client_tx_copy, server_rx, 32) == 0); // client → server
        assert(memcmp(server_tx, client_rx_copy, 32) == 0); // server → client     
    }
/*
        print_key("client_tx", client_tx_copy);
        print_key("server_rx", server_rx_copy);
        print_key("server_tx", server_tx_copy);
        print_key("client_rx", client_rx_copy);

        assert(memcmp(client_tx_copy, server_rx_copy, 32) == 0);
        assert(memcmp(server_tx_copy, client_rx_copy, 32) == 0);
*/
    printf("[+] tapin key exchange test passed!\n");
}

// Test when client or server misbehaves (e.g., closes socket early)
void test_invalid_key_exchange() {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        perror("socketpair");
        exit(1);
    }

    unsigned char client_rx[32], client_tx[32];

    pid_t pid = fork();
    if (pid == 0) {
        // Child: misbehaving client, closes early
        close(fds[0]);
        close(fds[1]);
        exit(0);
    } else {
        // Parent: tries handshake but fails
        close(fds[1]);
        int result = tapped_in(fds[0], 0, NULL, NULL, client_rx, client_tx);
        assert(result != 0);
        wait(NULL);
    }

    printf("[+] Invalid key exchange test passed!");
}

void test_key_exchange_with_invite() {
    int fds[2];
    int pipefd[2];

    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == 0);
    assert(pipe(pipefd) == 0);

    char invite[INVITE_LEN];
    const char *password = "hunter2";
    const char *ip = "192.168.0.42";
    const char *port = "31337";

    assert(invite_generate(invite, sizeof invite, password, ip, port) == 0);

    pid_t pid = fork();
    if (pid == 0) {
        // Child - client
        close(fds[0]);
        close(pipefd[0]);

        unsigned char client_rx[32], client_tx[32];
        assert(tapped_in(fds[1], 1, invite, password, client_rx, client_tx) == 0);

        write(pipefd[1], client_tx, 32);
        write(pipefd[1], client_rx, 32);
        close(pipefd[1]);
        exit(0);
    } else {
        // Parent - server
        close(fds[1]);
        close(pipefd[1]);

        unsigned char server_rx[32], server_tx[32];
        unsigned char client_tx_copy[32], client_rx_copy[32];

        assert(read(pipefd[0], client_tx_copy, 32) == 32);
        assert(read(pipefd[0], client_rx_copy, 32) == 32);
        close(pipefd[0]);

        assert(tapped_in(fds[0], 0, invite, password, server_rx, server_tx) == 0);
        wait(NULL);

        print_key("client_tx", client_tx_copy);
        print_key("server_rx", server_rx);
        print_key("server_tx", server_tx);
        print_key("client_rx", client_rx_copy);

        assert(memcmp(client_tx_copy, server_rx, 32) == 0);
        assert(memcmp(server_tx, client_rx_copy, 32) == 0);

        printf("[+] tapin key exchange with invite test passed!\n");
    }
}

void test_key_exchange_rekey() {
    int fds[2];
    assert(pipe(fds) == 0);

    pid_t pid = fork();
    assert(pid >= 0);

    const char *invite = "sample_invite";
    const char *pw = "secret_password";

    if (pid == 0) {
        // Client
        close(fds[0]);

        unsigned char k_tx1[32], k_rx1[32];
        unsigned char k_tx2[32], k_rx2[32];

        int rc1 = tapped_in(fds[1], 0, NULL, NULL, k_tx1, k_rx1);
        assert(rc1 == 0);

        int rc2 = tapped_in(fds[1], 0, NULL, NULL, k_tx2, k_rx2);
        assert(rc2 == 0);

        // Ensure rekey changed both directions
        assert(memcmp(k_tx1, k_tx2, 32) != 0);
        assert(memcmp(k_rx1, k_rx2, 32) != 0);

        write(fds[1], k_tx2, 32);
        write(fds[1], k_rx2, 32);
        close(fds[1]);
        exit(0);
    } else {
        // Server
        close(fds[1]);

        unsigned char server_tx[32], server_rx[32];
        unsigned char client_tx[32], client_rx[32];

        int rc = tapped_in(fds[0], 1, invite, pw, server_tx, server_rx);
        assert(rc == 0);
        // EXPECT(rc == -1, "should fail with bad invite/password");

        read(fds[0], client_tx, 32);
        read(fds[0], client_rx, 32);
        close(fds[0]);

        assert(memcmp(client_tx, server_rx, 32) == 0);
        assert(memcmp(server_tx, client_rx, 32) == 0);

        printf("[+] tapin rekey test passed!\n");
    }
}

void test_rekey_terminal_behavior() {
    int fds[2];
    if (pipe(fds) != 0) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child: Initiator
        close(fds[0]);
        unsigned char k_tx1[32], k_rx1[32];

        const char *invite = "invite123";
        const char *pw = "pass123";

        int rc1 = tapped_in(fds[1], 0, invite, pw, k_tx1, k_rx1);
        if (rc1 != 0) {
            fprintf(stderr, "[Client] Initial tapped_in failed\n");
            exit(1);
        }

        // Simulate manual --rekey call
        unsigned char k_tx2[32], k_rx2[32];
        int rc2 = tapped_in(fds[1], 0, invite, pw, k_tx2, k_rx2);
        if (rc2 != 0) {
            fprintf(stderr, "[Client] Rekey tapped_in failed\n");
            exit(1);
        }

        // Keys must differ after rekey
        if (memcmp(k_tx1, k_tx2, 32) == 0 || memcmp(k_rx1, k_rx2, 32) == 0) {
            fprintf(stderr, "[Client] Rekey failed to produce new keys\n");
            exit(1);
        }

        printf("[+] Manual rekey with invite succeeded\n");
        exit(0);

    } else {
        // Parent: Passive
        close(fds[1]);
        unsigned char server_tx1[32], server_rx1[32];

        const char *invite = "invite123";
        const char *pw = "pass123";

        int rc = tapped_in(fds[0], 1, invite, pw, server_tx1, server_rx1);
        if (rc != 0) {
            fprintf(stderr, "[Server] tapped_in failed\n");
            exit(1);
        }

        // Wait for client to finish
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("[+] test_manual_rekey_invite passed!\n");
        } else {
            fprintf(stderr, "[-] test_manual_rekey_invite failed!\n");
        }
    }
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium init failed\n");
        return 1;
    }

    test_key_exchange();
    test_invalid_key_exchange();
    test_key_exchange_with_invite();
    test_key_exchange_rekey();
    test_rekey_terminal_behavior();

    printf("[+] All tapin tests passed!\n");
    return 0;
}
