/*
 * Memory - Registered Memory
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "RegisteredMemory.h"

#include <stdlib.h>
#include <string.h>

#include <infinity/core/Configuration.h>
#include <infinity/utils/Debug.h>

namespace infinity {
namespace memory {

RegisteredMemory::RegisteredMemory(infinity::core::Context* context, uint64_t sizeInBytes) {

	this->context = context;
	this->sizeInBytes = sizeInBytes;
	this->memoryAllocated = true;

	int res = posix_memalign(&(this->data), infinity::core::Configuration::PAGE_SIZE, sizeInBytes);
	INFINITY_ASSERT(res == 0, "[INFINITY][MEMORY][REGISTERED] Cannot allocate and align buffer.\n");

	memset(this->data, 0, sizeInBytes);

	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), this->data, this->sizeInBytes,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
	INFINITY_ASSERT(this->ibvMemoryRegion != NULL, "[INFINITY][MEMORY][REGISTERED] Registration failed.\n");
}

RegisteredMemory::RegisteredMemory(infinity::core::Context* context, void *data, uint64_t sizeInBytes) {

	this->context = context;
	this->sizeInBytes = sizeInBytes;
	this->memoryAllocated = false;

	this->data = data;

	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), this->data, this->sizeInBytes,
			IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_READ);
	INFINITY_ASSERT(this->ibvMemoryRegion != NULL, "[INFINITY][MEMORY][REGISTERED] Registration failed.\n");
}


RegisteredMemory::~RegisteredMemory() {

	ibv_dereg_mr(this->ibvMemoryRegion);

	if(this->memoryAllocated) {
		free(this->data);
	}

}

void* RegisteredMemory::getData() {

	return this->data;

}

uint64_t RegisteredMemory::getSizeInBytes() {

	return this->sizeInBytes;

}

ibv_mr* RegisteredMemory::getRegion() {

	return this->ibvMemoryRegion;

}

} /* namespace pool */
} /* namespace ivory */
