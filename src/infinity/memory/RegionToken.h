/*
 * Memory - Region Token
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef MEMORY_REGIONTOKEN_H_
#define MEMORY_REGIONTOKEN_H_

#include <stdint.h>
#include <infinity/memory/RegionType.h>
#include <infinity/memory/Region.h>

namespace infinity {
namespace memory {

class RegionToken {

public:

	RegionToken();
	RegionToken(Region *memoryRegion, RegionType memoryRegionType, uint64_t sizeInBytes, uint64_t address, uint32_t localKey, uint32_t remoteKey);

public:

	Region * getMemoryRegion();
	RegionType getMemoryRegionType();
	uint64_t getSizeInBytes();
	uint64_t getRemainingSizeInBytes(uint64_t offset);
	uint64_t getAddress();
	uint64_t getAddressWithOffset(uint64_t offset);
	uint32_t getLocalKey();
	uint32_t getRemoteKey();

protected:

	Region *memoryRegion;
	const RegionType memoryRegionType;
	const uint64_t sizeInBytes;
	const uint64_t address;
	const uint32_t localKey;
	const uint32_t remoteKey;

};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_REGIONTOKEN_H_ */
