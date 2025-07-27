#ifndef TAPIN_TAPIN_H
#define TAPIN_TAPIN_H

#include <stdint.h>

int tapped_in(
            int fd,
            int initiator,
            const char *invite_code,
            const char *password,   
            unsigned char k_rx[32],
            unsigned char k_tx[32]);
#endif