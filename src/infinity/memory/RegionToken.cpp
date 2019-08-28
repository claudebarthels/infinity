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

RegionToken::RegionToken() {}

RegionToken::RegionToken(Region *memoryRegion, RegionType memoryRegionType,
                         uint64_t sizeInBytes, uint64_t address,
                         uint32_t localKey, uint32_t remoteKey)
    : memoryRegion(memoryRegion), memoryRegionType(memoryRegionType),
      sizeInBytes(sizeInBytes), address(address), localKey(localKey),
      remoteKey(remoteKey) {

  // Nothing to do here
}

Region *RegionToken::getMemoryRegion() const { return memoryRegion; }

RegionType RegionToken::getMemoryRegionType() const {
  return this->memoryRegionType;
}

uint64_t RegionToken::getSizeInBytes() const { return this->sizeInBytes; }

uint64_t RegionToken::getRemainingSizeInBytes(uint64_t offset) const {
  return this->sizeInBytes - offset;
}

uint64_t RegionToken::getAddress() const { return address; }

uint64_t RegionToken::getAddressWithOffset(uint64_t offset) const {
  return address + offset;
}

uint32_t RegionToken::getLocalKey() const { return this->localKey; }

uint32_t RegionToken::getRemoteKey() const { return this->remoteKey; }

std::ostream &operator<<(std::ostream &os, const RegionToken &regionToken) {
  os << "size: " << regionToken.getSizeInBytes() << " address "
     << regionToken.getAddress() << " localKey " << regionToken.getLocalKey()
     << " remoteKey " << regionToken.getRemoteKey();
  return os;
}

} /* namespace memory */
} /* namespace infinity */
