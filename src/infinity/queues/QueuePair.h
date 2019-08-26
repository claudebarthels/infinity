/**
 * Queues - Queue Pair
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef QUEUES_QUEUEPAIR_H_
#define QUEUES_QUEUEPAIR_H_

#include <memory>
#include <vector>
#include <infiniband/verbs.h>

#include <infinity/core/Context.h>
#include <infinity/memory/Atomic.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>

namespace infinity {
namespace queues {
class QueuePairFactory;
}
}

namespace infinity {
namespace queues {

class OperationFlags {

public:
  bool fenced;
  bool signaled;
  bool inlined;

  OperationFlags() : fenced(false), signaled(false), inlined(false) {};

  /**
   * Turn the bools into a bit field.
   */
  int ibvFlags();
};

class QueuePair {

  friend class infinity::queues::QueuePairFactory;

public:
  /**
   * Constructor
   */
  QueuePair(std::shared_ptr<infinity::core::Context> context);

  /**
   * Destructor
   */
  ~QueuePair();

  QueuePair(const QueuePair &) = delete;
  QueuePair(const QueuePair &&) = delete;
  QueuePair &operator=(const QueuePair &) = delete;
  QueuePair &operator=(QueuePair &&) = delete;

protected:
  /**
   * Activation methods
   */

  void activate(uint16_t remoteDeviceId, uint32_t remoteQueuePairNumber,
                uint32_t remoteSequenceNumber);
  void setRemoteUserData(const std::vector<char> &userData);

public:
  /**
   * User data received during connection setup
   */

  bool hasUserData();
  uint32_t getUserDataSize();
  void *getUserData();

public:
  /**
   * Queue pair information
   */

  uint16_t getLocalDeviceId();
  uint32_t getQueuePairNumber();
  uint32_t getSequenceNumber();

public:
  /**
   * Buffer operations
   */

  void send(std::shared_ptr<infinity::memory::Buffer> buffer,
            infinity::requests::RequestToken *requestToken = nullptr);
  void send(std::shared_ptr<infinity::memory::Buffer> buffer,
            uint32_t sizeInBytes,
            infinity::requests::RequestToken *requestToken = nullptr);
  void send(std::shared_ptr<infinity::memory::Buffer> buffer,
            uint64_t localOffset, uint32_t sizeInBytes, OperationFlags flags,
            infinity::requests::RequestToken *requestToken = nullptr);

  void write(std::shared_ptr<infinity::memory::Buffer> buffer,
             const infinity::memory::RegionToken &destination,
             infinity::requests::RequestToken *requestToken = nullptr);
  void write(std::shared_ptr<infinity::memory::Buffer> buffer,
             const infinity::memory::RegionToken &destination,
             uint32_t sizeInBytes,
             infinity::requests::RequestToken *requestToken = nullptr);
  void write(std::shared_ptr<infinity::memory::Buffer> buffer,
             uint64_t localOffset,
             const infinity::memory::RegionToken &destination,
             uint64_t remoteOffset, uint32_t sizeInBytes, OperationFlags flags,
             infinity::requests::RequestToken *requestToken = nullptr);

  void read(std::shared_ptr<infinity::memory::Buffer> buffer,
            const infinity::memory::RegionToken &source,
            infinity::requests::RequestToken *requestToken = nullptr);
  void read(std::shared_ptr<infinity::memory::Buffer> buffer,
            const infinity::memory::RegionToken &source, uint32_t sizeInBytes,
            infinity::requests::RequestToken *requestToken = nullptr);
  void read(std::shared_ptr<infinity::memory::Buffer> buffer,
            uint64_t localOffset, const infinity::memory::RegionToken &source,
            uint64_t remoteOffset, uint32_t sizeInBytes, OperationFlags flags,
            infinity::requests::RequestToken *requestToken = nullptr);

public:
  /**
   * Complex buffer operations
   */

  void multiWrite(
      const std::vector<std::shared_ptr<infinity::memory::Buffer> > &buffers,
      uint32_t *sizesInBytes, uint64_t *localOffsets,
      const infinity::memory::RegionToken &destination, uint64_t remoteOffset,
      OperationFlags flags,
      infinity::requests::RequestToken *requestToken = nullptr);

  void sendWithImmediate(std::shared_ptr<infinity::memory::Buffer> buffer,
                         uint64_t localOffset, uint32_t sizeInBytes,
                         uint32_t immediateValue, OperationFlags flags,
                         infinity::requests::RequestToken *requestToken =
                             nullptr);

  void writeWithImmediate(std::shared_ptr<infinity::memory::Buffer> buffer,
                          uint64_t localOffset,
                          const infinity::memory::RegionToken &destination,
                          uint64_t remoteOffset, uint32_t sizeInBytes,
                          uint32_t immediateValue, OperationFlags flags,
                          infinity::requests::RequestToken *requestToken =
                              nullptr);

  void multiWriteWithImmediate(
      const std::vector<std::shared_ptr<infinity::memory::Buffer> > &buffers,
      uint32_t *sizesInBytes, uint64_t *localOffsets,
      const infinity::memory::RegionToken &destination, uint64_t remoteOffset,
      uint32_t immediateValue, OperationFlags flags,
      infinity::requests::RequestToken *requestToken = nullptr);

public:
  /**
   * Atomic value operations
   */

  void compareAndSwap(const infinity::memory::RegionToken &destination,
                      uint64_t compare, uint64_t swap,
                      infinity::requests::RequestToken *requestToken = nullptr);
  void compareAndSwap(const infinity::memory::RegionToken &destination,
                      std::shared_ptr<infinity::memory::Atomic> previousValue,
                      uint64_t compare, uint64_t swap, OperationFlags flags,
                      infinity::requests::RequestToken *requestToken = nullptr);
  void fetchAndAdd(const infinity::memory::RegionToken &destination,
                   uint64_t add,
                   infinity::requests::RequestToken *requestToken = nullptr);
  void fetchAndAdd(const infinity::memory::RegionToken &destination,
                   std::shared_ptr<infinity::memory::Atomic> previousValue,
                   uint64_t add, OperationFlags flags,
                   infinity::requests::RequestToken *requestToken = nullptr);

protected:
  std::shared_ptr<infinity::core::Context> context;

  ibv_qp *ibvQueuePair = nullptr;
  uint32_t sequenceNumber = 0;
  std::shared_ptr<infinity::memory::Atomic> defaultAtomic;
  std::vector<char> userData;
  uint32_t maxNumberOfSGEElements = 0;
};

} /* namespace queues */
} /* namespace infinity */

#endif /* QUEUES_QUEUEPAIR_H_ */
