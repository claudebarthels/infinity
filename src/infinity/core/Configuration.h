/**
 * Core - Configuration
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef CORE_CONFIGURATION_H_
#define CORE_CONFIGURATION_H_

#include <stdint.h>

namespace infinity {
namespace core {

class Configuration {

public:

	/**
	 * Queue length settings
	 */

	static const uint32_t SEND_COMPLETION_QUEUE_LENGTH = 16351; 		// Must be less than MAX_CQE

	static const uint32_t RECV_COMPLETION_QUEUE_LENGTH = 16351; 		// Must be less than MAX_CQE

	static const uint32_t SHARED_RECV_QUEUE_LENGTH = 16351; 			// Must be less than MAX_SRQ_WR

	static const uint32_t MAX_NUMBER_OF_OUTSTANDING_REQUESTS = 16351;	// Must be less than (MAX_QP_WR * MAX_QP)
																		// Since we use one single shared receive queue,
																		// this number should be less than MAX_SRQ_WR

	static const uint32_t MAX_NUMBER_OF_SGE_ELEMENTS = 1;				// Must be less than MAX_SGE

public:

	/**
	 * System settings
	 */

	static const uint32_t PAGE_SIZE = 4096; 							// Memory regions will be page aligned by the Infinity library

	static const uint32_t MAX_CONNECTION_USER_DATA_SIZE = 1024;			// Size of the user data which can be transmitted when establishing a connection

	static constexpr const char* DEFAULT_IB_DEVICE = "ib0";				// Default name of IB device

};

} /* namespace core */
} /* namespace infinity */

#endif /* CORE_CONFIGURATION_H_ */
