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
...
[*] Running unit tests...
Running tests/test_invite...
[+] All invite tests passed!
Running tests/test_crypto...
[+] Encrypt/decrypt roundtrip passed.
[+] Decryption fails with wrong key.
[+] Tampered ciphertext correctly rejected.
[+] All crypto tests passed.
Running tests/test_tapin...
[Client] Starting tapped_in...
[Client] Local pubkey: a1457903e6a4351cdc9b7291dd09687a71ff83cca20d68fc381212a808909565
[Client] Local pubkey: 9db2be369d1ea503ef178edbfbc5f0dd65082b2dfea763c93c5427cf05b7993c
...
server_tx: 255cfa14a2d8898f9c7eb89d2b9af911c7cb7363126e0b8da8aa88efd1305c2e
client_rx: 255cfa14a2d8898f9c7eb89d2b9af911c7cb7363126e0b8da8aa88efd1305c2e
[+] tapin key exchange test passed!
invite code and password are required 
```

