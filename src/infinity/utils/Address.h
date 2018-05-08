/**
 * Utils - Address
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef UTILS_ADDRESS_H_
#define UTILS_ADDRESS_H_

#include <stdint.h>

namespace infinity {
namespace utils {

class Address {

public:

	static char * getIpAddressOfInterface(const char *interfaceName);
	static uint32_t getIpAddressAsUint32(const char *ipAddress);

};

} /* namespace utils */
} /* namespace infinity */

#endif /* UTILS_ADDRESS_H_ */
