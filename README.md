# TapIn
## Description
Peer to peer encrypted terminal based chat application.

## How to install
You may need to install libsodium directly. Simplest way is using the `apt` install tools.

```shell
sudo apt update
sudo apt install libsodium-dev build-essential
```

Then you can run `make` to build the application.

## How to run tests

```shell
make test
``` 

Since `make test` uses the `test/test_runner.bash` script you can also run new `test_*.c` files
you create directly by passing an argument to the script. The script only takes a single argument.
If no argument is given the full test suite is run.

```shell
./tests/test_runner.bash test_crypto.c
............START..............
[+] Running tests/test_crypto.c
[+] Encrypt/decrypt roundtrip passed.
[+] Decryption fails with wrong key.
[+] Tampered ciphertext correctly rejected.
[+] All crypto tests passed.
............END................
```
