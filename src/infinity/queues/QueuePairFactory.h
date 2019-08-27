/**
 * Queues - Queue Pair Factory
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef QUEUES_QUEUEPAIRFACTORY_H_
#define QUEUES_QUEUEPAIRFACTORY_H_

#include <vector>
#include <memory>
#include <stdlib.h>
#include <stdint.h>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePair.h>

namespace infinity {
namespace queues {

class QueuePairFactory {
public:
  QueuePairFactory(const std::shared_ptr<infinity::core::Context> &context);
  ~QueuePairFactory();

  /**
   * Bind to port for listening to incoming connections
   */
  void bindToPort(uint16_t port);
  uint16_t getPort();

  /**
   * Accept incoming connection request (passive side)
   */
  std::shared_ptr<QueuePair>
  acceptIncomingConnection(void *userData = nullptr,
                           uint32_t userDataSizeInBytes = 0);

  /**
   * Connect to remote machine (active side)
   */
  std::shared_ptr<QueuePair>
  connectToRemoteHost(const char *hostAddress, uint16_t port,
                      void *userData = nullptr,
                      uint32_t userDataSizeInBytes = 0);

  /**
   * Create loopback queue pair
   */
  std::shared_ptr<QueuePair> createLoopback(const std::vector<char> &userData);

protected:
  std::shared_ptr<infinity::core::Context> context;

  int32_t serverSocket = -1;

private:
  int32_t readFromSocket(int32_t socket, char *buffer, uint32_t size);
  int32_t sendToSocket(int32_t socket, const char *buffer, uint32_t size);
};

} /* namespace queues */
} /* namespace infinity */

#endif /* QUEUES_QUEUEPAIRFACTORY_H_ */
