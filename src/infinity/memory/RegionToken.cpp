/*
 * Memory - Region Token
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include <infinity/memory/RegionToken.h>

namespace infinity {
namespace memory {

RegionToken::RegionToken() :
	memoryRegion (NULL),
	memoryRegionType (UNKNOWN),
	sizeInBytes(0),
	address(0),
	localKey(0),
	remoteKey(0) {

	// Nothing to do here

}

RegionToken::RegionToken(Region *memoryRegion, RegionType memoryRegionType, uint64_t sizeInBytes, uint64_t address, uint32_t localKey, uint32_t remoteKey) :
	memoryRegion (memoryRegion),
	memoryRegionType (memoryRegionType),
	sizeInBytes(sizeInBytes),
	address(address),
	localKey(localKey),
	remoteKey(remoteKey) {

	// Nothing to do here

}

Region* RegionToken::getMemoryRegion() {
	return memoryRegion;
}

RegionType RegionToken::getMemoryRegionType() {
	return this->memoryRegionType;
}

uint64_t RegionToken::getSizeInBytes() {
	return this->sizeInBytes;
}

uint64_t RegionToken::getRemainingSizeInBytes(uint64_t offset) {
	return this->sizeInBytes-offset;
}

uint64_t RegionToken::getAddress() {
	return address;
}

uint64_t RegionToken::getAddressWithOffset(uint64_t offset) {
	return address + offset;
}

uint32_t RegionToken::getLocalKey() {
	return this->localKey;
}

uint32_t RegionToken::getRemoteKey() {
	return this->remoteKey;
}


} /* namespace memory */
} /* namespace infinity */
