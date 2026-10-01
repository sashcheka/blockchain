# Blockchain Lab

A small C++20 proof-of-work ledger built to make the core blockchain ideas easy to inspect: canonical transaction encoding, SHA-256 block hashes, mining, and chain validation.

> This is an educational project. It is not a cryptocurrency and must not be used to store value. Transactions are not signed, balances are not tracked, and there is no peer-to-peer network.

## What it demonstrates

- Transactions are validated as positive integer units.
- Every transaction in a block contributes to its transaction digest.
- Blocks link to their parent hash and include the mining difficulty in the hashed header.
- Proof of work searches for a SHA-256 hash with the configured number of leading hexadecimal zeroes.
- The chain verifier checks block order, parent links, stored hashes, and proof of work.
- Domain logic is a library, separate from the interactive command-line app.

## Build

Requirements: CMake 3.16+, a C++20 compiler, and OpenSSL 1.1.1+ development files.

On macOS with Homebrew:

```sh
brew install cmake openssl@3
cmake -S . -B build -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)"
cmake --build build
ctest --test-dir build --output-on-failure
```

On Ubuntu/Debian:

```sh
sudo apt-get install cmake g++ libssl-dev
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Run

```text
$ ./build/blockchain
> send alice bob 25 lunch
Transaction added. Pending: 1
> mine
Block mined. Chain length: 1
> verify
Chain is valid.
> chain
Block #0 | timestamp: ... | nonce: ... | transactions: 1
  previous: 000000...
  hash:     00...
  alice -> bob : 25 (lunch)
```

Available commands: `send`, `mine`, `chain`, `verify`, `help`, and `exit`. Difficulty defaults to two leading hexadecimal zeroes and is bounded to keep local mining practical.

## Architecture

```text
app/main.cpp                 CLI parsing and presentation
include/blockchain/          Public domain interfaces
src/                         Hashing, transaction, block, and chain logic
tests/                       Lightweight tests registered with CTest
```

The `Blockchain` owns blocks and pending transactions. It chooses the next block index and parent hash, while `Block` owns canonical header hashing and proof-of-work verification. Transactions and blocks are value types; the domain layer does not print to the console.

## Design notes and next steps

Integers are used for amounts to avoid floating-point rounding. Serialized fields are length-prefixed and integers use a fixed big-endian representation, so different field boundaries cannot produce the same byte sequence accidentally. The current block transaction digest is a hash over the ordered transaction list; a Merkle tree is a natural next improvement.

Useful next milestones are signed transactions, an account-state transition model with replay protection, persistence with load-time validation, and a richer CLI. Each should come with tests before adding networking or a graphical interface.
