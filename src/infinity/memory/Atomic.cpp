/*
 * Memory - Atomic
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "Atomic.h"

#include <infiniband/verbs.h>

namespace infinity {
namespace memory {

Atomic::Atomic(infinity::core::Context* context) {

	this->context = context;
	this->sizeInBytes = sizeof(uint64_t);
	this->memoryRegionType = RegionType::ATOMIC;

	this->value = 0;
	this->data = &value;

	this->ibvMemoryRegion = ibv_reg_mr(this->context->getProtectionDomain(), &(this->value), this->sizeInBytes,
			IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_ATOMIC | IBV_ACCESS_REMOTE_READ | IBV_ACCESS_REMOTE_WRITE);


}

uint64_t infinity::memory::Atomic::getValue() {

	return this->value;

}

void infinity::memory::Atomic::setValueNonAtomic(uint64_t value) {

	this->value = value;

}


Atomic::~Atomic() {

	ibv_dereg_mr(this->ibvMemoryRegion);

}

} /* namespace memory */
} /* namespace infinity */
