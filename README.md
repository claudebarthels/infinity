# Infinity - A lightweight C++ RDMA library for InfiniBand

Infinity is a simple, powerful, object-oriented abstraction of ibVerbs. The library enables users to build sophisticated applications that use Remote Direct Memory Access (RDMA) without sacrificing performance. It significantly lowers the barrier to get started with RDMA programming. Infinity provides support for two-sided (send/receive) as well as one-sided (read/write/atomic) operations. The library is written in C++ and will be ported to Rust in the near future.

## Installation

Installing ''ibVerbs'' is a prerequisite before building Infinity. The output is located in ''release/libinfinity.a''.

```sh
$ make library # Build the library
$ make examples # Build the examples
```
## Using Infinity

Using Infinity is straight-forward and requires only a few lines of C++ code.

```C
// Create new context
infinity::core::Context *context = new infinity::core::Context();

// Create a queue pair
infinity::queues::QueuePairFactory *qpFactory = new  infinity::queues::QueuePairFactory(context);
infinity::queues::QueuePair *qp = qpFactory->connectToRemoteHost(SERVER_IP, PORT_NUMBER);

// Create and register a buffer with the network
infinity::memory::Buffer *localBuffer = new infinity::memory::Buffer(context, BUFFER_SIZE);

// Get information from a remote buffer
infinity::memory::RegionToken *remoteBufferToken = new infinity::memory::RegionToken(REMOTE_BUFFER_INFO);

// Read (one-sided) from a remote buffer and wait for completion
infinity::requests::RequestToken requestToken(context);
qp->read(localBuffer, remoteBufferToken, &requestToken);
requestToken.waitUntilCompleted();

// Write (one-sided) content of a local buffer to a remote buffer and wait for completion
qp->write(localBuffer, remoteBufferToken, &requestToken);
requestToken.waitUntilCompleted();

// Send (two-sided) content of a local buffer over the queue pair and wait for completion
qp->send(localBuffer, &requestToken);
requestToken.waitUntilCompleted();

// Close connection
delete remoteBufferToken;
delete localBuffer;
delete qp;
delete qpFactory;
delete context;
```
