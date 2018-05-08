/**
 * Utils - Address
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "Address.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <strings.h>

#include <infinity/utils/Debug.h>

namespace infinity {
namespace utils {

char* Address::getIpAddressOfInterface(const char* interfaceName) {

	struct ifaddrs *ifAddr;
	struct ifaddrs *ifa;
	char *ipAddress = (char*) calloc(16, sizeof(char));

	int returnValue = getifaddrs(&ifAddr);
	INFINITY_ASSERT(returnValue != -1, "[INFINITY][UTILS][ADDRESS] Cannot read interface list.\n");

	for (ifa = ifAddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		if ((ifa->ifa_addr->sa_family == AF_INET) && (strcasecmp(interfaceName, ifa->ifa_name) == 0)) {
			sprintf(ipAddress, "%s", inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr));
			break;
		}
	}
	INFINITY_ASSERT(ifa != NULL, "[INFINITY][UTILS][ADDRESS] Cannot find interface named %s.\n", interfaceName);

	freeifaddrs(ifAddr);

	return ipAddress;

}

uint32_t Address::getIpAddressAsUint32(const char* ipAddress) {

	uint32_t ipAddressNumbers[4];
	sscanf(ipAddress, "%d.%d.%d.%d", &ipAddressNumbers[3], &ipAddressNumbers[2], &ipAddressNumbers[1], &ipAddressNumbers[0]);
	uint32_t ipAddressNumber(ipAddressNumbers[0] | ipAddressNumbers[1] << 8 | ipAddressNumbers[2] << 16 | ipAddressNumbers[3] << 24);
	return ipAddressNumber;
}

} /* namespace utils */
} /* namespace infinity */
