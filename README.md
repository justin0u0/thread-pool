# Introduction

This is a repo containing implementation of thread(worker) pool with unit testing.

- [x] Pthread library
- [x] Golang goroutines
- [ ] std::thread

# Testing

Run `make clean && make` first.

Then:

- `./pthread/thread_pool_test` to test pthread thread pool.
- `go test -race ./go/.` to test golang worker pool.
