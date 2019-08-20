/*
 * Memory - Region
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef MEMORY_REGION_H_
#define MEMORY_REGION_H_

#include <stdint.h>
#include <infiniband/verbs.h>

#include <infinity/core/Context.h>
#include <infinity/memory/RegionType.h>

namespace infinity {
namespace memory {

class RegionToken;

class Region {

public:
	Region();
	virtual ~Region();

	RegionToken createRegionToken();
	RegionToken createRegionToken(uint64_t offset);
	RegionToken createRegionToken(uint64_t offset, uint64_t size);

public:

	RegionType getMemoryRegionType();
	uint64_t getSizeInBytes();
	uint64_t getRemainingSizeInBytes(uint64_t offset);
	uint64_t getAddress();
	uint64_t getAddressWithOffset(uint64_t offset);
	uint32_t getLocalKey();
	uint32_t getRemoteKey();

	Region(const Region&) = delete;
	Region(const Region&&) = delete;
	Region& operator=(const Region&) = delete;
	Region& operator=(Region&&) = delete;

protected:

	infinity::core::Context* context = nullptr;
	RegionType memoryRegionType;
	ibv_mr *ibvMemoryRegion = nullptr;

protected:

	void * data = nullptr;
	uint64_t sizeInBytes = 0;

};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_REGION_H_ */
