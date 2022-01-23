# Introduction

This is a repo containing implementations of thread(worker) pool with gracefully shutdown in different languages or different libraries.

- [x] Pthread library
- [x] Golang goroutines
- [ ] std::thread

# Usage

See testing files.

# Testing

Run `make clean && make` first.

Then:

- `./pthread/thread_pool_test` to test pthread thread pool.
- `go test -race ./go/.` to test golang worker pool.
