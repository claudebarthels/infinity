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

        Region *memoryRegion = nullptr;
        const RegionType memoryRegionType = UNKNOWN;
        const uint64_t sizeInBytes = 0;
        const uint64_t address = 0;
        const uint32_t localKey = 0;
        const uint32_t remoteKey = 0;

};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_REGIONTOKEN_H_ */
