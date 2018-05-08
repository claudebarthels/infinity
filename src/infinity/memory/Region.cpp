/*
 * Memory - Region
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "Buffer.h"

#include <infinity/utils/Debug.h>
#include <infinity/memory/RegionToken.h>

namespace infinity {
namespace memory {

Region::~Region() {
	// To be overwritten in sub class
}

RegionToken* Region::createRegionToken() {
	return new RegionToken(this, getMemoryRegionType(), getSizeInBytes(), getAddress(), getLocalKey(), getRemoteKey());
}

RegionToken * Region::createRegionToken(uint64_t offset) {
	return new RegionToken(this, getMemoryRegionType(), getRemainingSizeInBytes(offset), getAddressWithOffset(offset), getLocalKey(), getRemoteKey());
}

RegionToken * Region::createRegionToken(uint64_t offset, uint64_t size) {
	return new RegionToken(this, getMemoryRegionType(), size, getAddressWithOffset(offset), getLocalKey(), getRemoteKey());
}

RegionType Region::getMemoryRegionType() {
	return this->memoryRegionType;
}

uint64_t Region::getSizeInBytes() {
	return this->sizeInBytes;
}

uint64_t Region::getRemainingSizeInBytes(uint64_t offset) {
	return this->sizeInBytes - offset;
}

uint64_t Region::getAddress() {
	return reinterpret_cast<uint64_t>(this->data);
}

uint64_t Region::getAddressWithOffset(uint64_t offset) {
	return reinterpret_cast<uint64_t>(this->data) + offset;
}

uint32_t Region::getLocalKey() {
	return this->ibvMemoryRegion->lkey;
}

uint32_t Region::getRemoteKey() {
	return this->ibvMemoryRegion->rkey;
}

} /* namespace memory */
} /* namespace infinity */
