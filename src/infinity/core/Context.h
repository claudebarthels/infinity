/**
 * Core - Context
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef CORE_CONTEXT_H_
#define CORE_CONTEXT_H_

#include <stdlib.h>
#include <stdint.h>
#include <unordered_map>
#include <infiniband/verbs.h>

namespace infinity {
namespace memory {
class Region;
class Buffer;
class Atomic;
class RegisteredMemory;
}
}

namespace infinity {
namespace queues {
class QueuePair;
class QueuePairFactory;
}
}

namespace infinity {
namespace requests {
class RequestToken;
}
}

namespace infinity {
namespace core {

typedef struct {
	infinity::memory::Buffer *buffer = nullptr;
	uint32_t bytesWritten = 0;
	uint32_t immediateValue = 0;
	bool immediateValueValid = false;
	infinity::queues::QueuePair *queuePair = nullptr;
} receive_element_t;

class Context {

	friend class infinity::memory::Region;
	friend class infinity::memory::Buffer;
	friend class infinity::memory::Atomic;
	friend class infinity::memory::RegisteredMemory;
	friend class infinity::queues::QueuePair;
	friend class infinity::queues::QueuePairFactory;
	friend class infinity::requests::RequestToken;

public:

	/**
	 * Constructors
	 */
	Context(uint16_t device = 0, uint16_t devicePort = 1);

	/**
	 * Destructor
	 */
	~Context();

	Context(const Context&) = delete;
	Context(const Context&&) = delete;
	Context& operator=(const Context&) = delete;
	Context& operator=(Context&&) = delete;

public:

	/**
	 * Check if receive operation completed
	 */
	bool receive(receive_element_t *receiveElement);
	bool receive(infinity::memory::Buffer **buffer, uint32_t *bytesWritten, uint32_t *immediateValue, bool *immediateValueValid, infinity::queues::QueuePair **queuePair = nullptr);

	/**
	 * Post a new buffer for receiving messages
	 */
	void postReceiveBuffer(infinity::memory::Buffer *buffer);

public:
        void getDeviceAttr(ibv_device_attr * device_attr);

public:

	infinity::requests::RequestToken * defaultRequestToken = nullptr;
	infinity::memory::Atomic * defaultAtomic = nullptr;

protected:

	/**
	 * Returns ibVerbs context
	 */
	ibv_context * getInfiniBandContext();

	/**
	 * Returns local device id
	 */
	uint16_t getLocalDeviceId();

	/**
	 * Returns device port
	 */
	uint16_t getDevicePort();

	/**
	 * Returns ibVerbs protection domain
	 */
	ibv_pd * getProtectionDomain();

protected:

	/**
	 * Check if send operation completed
	 */
	bool pollSendCompletionQueue();

	/**
	 * Returns ibVerbs completion queue for sending
	 */
	ibv_cq * getSendCompletionQueue();

	/**
	 * Returns ibVerbs completion queue for receiving
	 */
	ibv_cq * getReceiveCompletionQueue();

	/**
	 * Returns ibVerbs shared receive queue
	 */
	ibv_srq * getSharedReceiveQueue();

protected:

	/**
	 * IB context and protection domain
	 */
	ibv_context *ibvContext = nullptr;
	ibv_pd *ibvProtectionDomain = nullptr;

	/**
	 * Local device id and port
	 */
	ibv_device *ibvDevice = nullptr;
	uint16_t ibvLocalDeviceId = 0;
	uint16_t ibvDevicePort = 1;

	/**
	 * IB send and receive completion queues
	 */
	ibv_cq *ibvSendCompletionQueue = nullptr;
	ibv_cq *ibvReceiveCompletionQueue = nullptr;
	ibv_srq *ibvSharedReceiveQueue = nullptr;

protected:

	void registerQueuePair(infinity::queues::QueuePair *queuePair);
	std::unordered_map<uint32_t, infinity::queues::QueuePair *> queuePairMap;

};

} /* namespace core */
} /* namespace infinity */

#endif /* CORE_CONTEXT_H_ */
