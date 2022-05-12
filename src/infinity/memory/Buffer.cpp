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

#define MIN(a,b) (((a)<(b)) ? (a) : (b))

namespace infinity {
namespace memory {

Buffer::Buffer(infinity::core::Context* context, uint64_t sizeInBytes) {

	this->context = context;
	this->sizeInBytes = sizeInBytes;
	this->memoryRegionType = RegionType::BUFFER;

	int res = posix_memalign(&(this->data), infinity::core::Configuration::PAGE_SIZE, sizeInBytes);
	INFINITY_ASSERT(res == 0, "[INFINITY][MEMORY][BUFFER] Cannot allocate and align buffer.\n");

	memset(this->data, 0, sizeInBytes);

#ifdef _RDMA_USE_ODP
	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), NULL, SIZE_MAX,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_ON_DEMAND);
#else
	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), this->data, this->sizeInBytes,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
#endif // _RDMA_USE_ODP

	INFINITY_ASSERT(this->ibvMemoryRegion != NULL, "[INFINITY][MEMORY][BUFFER] Registration failed.\n");

	this->memoryAllocated = true;
	this->memoryRegistered = true;

}

Buffer::Buffer(infinity::core::Context* context, infinity::memory::RegisteredMemory* memory, uint64_t offset, uint64_t sizeInBytes) {

	this->context = context;
	this->sizeInBytes = sizeInBytes;
	this->memoryRegionType = RegionType::BUFFER;

	this->data = reinterpret_cast<char *>(memory->getData()) + offset;
	this->ibvMemoryRegion = memory->getRegion();

	this->memoryAllocated = false;
	this->memoryRegistered = false;

}

Buffer::Buffer(infinity::core::Context *context, void *memory, uint64_t sizeInBytes) {

	this->context = context;
	this->sizeInBytes = sizeInBytes;
	this->memoryRegionType = RegionType::BUFFER;

	this->data = memory;

#ifdef _RDMA_USE_ODP
	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), NULL, SIZE_MAX,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_ON_DEMAND);
#else
	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), this->data, this->sizeInBytes,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
#endif // _RDMA_USE_ODP

	INFINITY_ASSERT(this->ibvMemoryRegion != NULL, "[INFINITY][MEMORY][BUFFER] Registration failed.\n");

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

void* Buffer::getData() {
	return reinterpret_cast<void *>(this->getAddress());
}

void Buffer::setData(void *data, uint64_t sizeInBytes) {
	if (this->memoryAllocated) {
		free(this->data);
	}

	this->data = data;
	this->sizeInBytes = sizeInBytes;
	this->memoryAllocated = false;
}

void Buffer::resize(uint64_t newSize, void* newData) {

	void *oldData = this->data;
	uint32_t oldSize = this->sizeInBytes;

	if (newData == NULL) {
		newData = this->data;
	}

	if (oldData != newData) {
		uint64_t copySize = MIN(newSize, oldSize);
		memcpy(newData, oldData, copySize);
	}

	if (memoryRegistered) {
		ibv_dereg_mr(this->ibvMemoryRegion);

	// When using ODP, nothing needs to be done since we don't register specific addresses anyway.
	// So re-assigning a new buffer and new size is a no-op in the ODP case in terms of registering.
#ifndef _RDMA_USE_ODP
		this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), newData, newSize,
				IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
#endif // _RDMA_USE_ODP

		this->data = newData;
		this->sizeInBytes = newSize;
	} else {
		INFINITY_ASSERT(false, "[INFINITY][MEMORY][BUFFER] You can only resize memory which has registered by this buffer.\n");
	}
}

} /* namespace memory */
} /* namespace infinity */
