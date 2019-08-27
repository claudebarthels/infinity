/*
 * Memory - Buffer
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "Buffer.h"

#include <stdlib.h>
#include <string.h>

#include <infinity/core/Configuration.h>
#include <infinity/utils/Debug.h>

namespace infinity {
namespace memory {

std::shared_ptr<Buffer>
Buffer::createBuffer(std::shared_ptr<infinity::core::Context> context,
                     uint64_t sizeInBytes) {
  return std::make_shared<Buffer>(context, sizeInBytes, Token());
}

std::shared_ptr<Buffer>
Buffer::createBuffer(std::shared_ptr<infinity::core::Context> context,
                     infinity::memory::RegisteredMemory *memory,
                     uint64_t offset, uint64_t sizeInBytes) {
  return std::make_shared<Buffer>(context, memory, offset, sizeInBytes,
                                  Token());
}

std::shared_ptr<Buffer>
Buffer::createBuffer(std::shared_ptr<infinity::core::Context> context,
                     void *memory, uint64_t sizeInBytes) {
  return std::make_shared<Buffer>(context, memory, sizeInBytes, Token());
}

Buffer::Buffer(std::shared_ptr<infinity::core::Context> context,
               uint64_t sizeInBytes, Token) {

  this->context = context;
  this->sizeInBytes = sizeInBytes;
  this->memoryRegionType = RegionType::BUFFER;

  int res = posix_memalign(
      &(this->data), infinity::core::Configuration::PAGE_SIZE, sizeInBytes);
  INFINITY_ASSERT(
      res == 0,
      "[INFINITY][MEMORY][BUFFER] Cannot allocate and align buffer.\n");

  memset(this->data, 0, sizeInBytes);

  this->ibvMemoryRegion = ibv_reg_mr(
      this->context->getProtectionDomain(), this->data, this->sizeInBytes,
      IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE |
          IBV_ACCESS_REMOTE_READ);
  INFINITY_ASSERT(this->ibvMemoryRegion != nullptr,
                  "[INFINITY][MEMORY][BUFFER] Registration failed.\n");

  this->memoryAllocated = true;
  this->memoryRegistered = true;
}

Buffer::Buffer(std::shared_ptr<infinity::core::Context> context,
               infinity::memory::RegisteredMemory *memory, uint64_t offset,
               uint64_t sizeInBytes, Token) {

  this->context = context;
  this->sizeInBytes = sizeInBytes;
  this->memoryRegionType = RegionType::BUFFER;

  this->data = reinterpret_cast<char *>(memory->getData()) + offset;
  this->ibvMemoryRegion = memory->getRegion();

  this->memoryAllocated = false;
  this->memoryRegistered = false;
}

Buffer::Buffer(std::shared_ptr<infinity::core::Context> context, void *memory,
               uint64_t sizeInBytes, Token) {

  this->context = context;
  this->sizeInBytes = sizeInBytes;
  this->memoryRegionType = RegionType::BUFFER;

  this->data = memory;
  this->ibvMemoryRegion = ibv_reg_mr(
      this->context->getProtectionDomain(), this->data, this->sizeInBytes,
      IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE |
          IBV_ACCESS_REMOTE_READ);
  INFINITY_ASSERT(this->ibvMemoryRegion != nullptr,
                  "[INFINITY][MEMORY][BUFFER] Registration failed.\n");

  this->memoryAllocated = false;
  this->memoryRegistered = true;
}

Buffer::~Buffer() {

  if (this->memoryRegistered) {
    ibv_dereg_mr(this->ibvMemoryRegion);
  }
  if (this->memoryAllocated) {
    free(this->data);
  }
}

void *Buffer::getData() { return reinterpret_cast<void *>(this->getAddress()); }

void Buffer::resize(uint64_t newSize, void *newData) {

  void *oldData = this->data;
  uint64_t oldSize = this->sizeInBytes;

  if (newData == nullptr) {
    newData = this->data;
  }

  if (oldData != newData) {
    uint64_t copySize = std::min(newSize, oldSize);
    memcpy(newData, oldData, copySize);
  }

  if (memoryRegistered) {
    ibv_dereg_mr(this->ibvMemoryRegion);
    this->ibvMemoryRegion =
        ibv_reg_mr(this->context->getProtectionDomain(), newData, newSize,
                   IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE |
                       IBV_ACCESS_REMOTE_READ);
    this->data = newData;
    this->sizeInBytes = newSize;
  } else {
    INFINITY_ASSERT(false, "[INFINITY][MEMORY][BUFFER] You can only resize "
                           "memory which has registered by this buffer.\n");
  }
}

Buffer &Buffer::operator=(Buffer &&other) {
  if (&other != this) {
    data = other.data;
    sizeInBytes = other.sizeInBytes;
    memoryRegistered = other.memoryRegistered;
    memoryAllocated = other.memoryAllocated;
    other.data = nullptr;
    other.sizeInBytes = 0;
    other.memoryRegistered = false;
    other.memoryAllocated = false;
  }
  return *this;
}

} /* namespace memory */
} /* namespace infinity */
