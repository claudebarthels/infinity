/**
 * Core - Configuration
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef CORE_CONFIGURATION_H_
#define CORE_CONFIGURATION_H_

#include <memory>
#include <stdint.h>

namespace infinity {
namespace core {

class Context;
class Configuration {

public:

	/**
	 * Queue length settings
	 */

	static const uint32_t sendCompletionQueueLength(Context * context); 		// Must be less than MAX_CQE

	static const uint32_t recvCompletionQueueLength(Context * context); 		// Must be less than MAX_CQE

        static const uint32_t sendCompletionQueueLength(std::shared_ptr<Context>& context); 		// Must be less than MAX_CQE

        static const uint32_t recvCompletionQueueLength(std::shared_ptr<Context>& context); 		// Must be less than MAX_CQE

        static const uint32_t sharedRecvQueueLength(Context * context); 		// Must be less than MAX_SRQ_WR

	static const uint32_t maxNumberOfOutstandingRequests(Context * context);      	// Must be less than (MAX_QP_WR * MAX_QP)
									// Since we use one single shared receive queue,
									// this number should be less than MAX_SRQ_WR

        static const uint32_t maxNumberOfSGEElements(const std::shared_ptr<Context>& context);		// Must be less than MAX_SGE

public:

	/**
	 * System settings
	 */

	static const uint32_t PAGE_SIZE = 4096; 			// Memory regions will be page aligned by the Infinity library

	static const uint32_t MAX_CONNECTION_USER_DATA_SIZE = 1024;	// Size of the user data which can be transmitted when establishing a connection

	static constexpr const char* DEFAULT_IB_DEVICE = "ib0";		// Default name of IB device

};

} /* namespace core */
} /* namespace infinity */

#endif /* CORE_CONFIGURATION_H_ */
