/*
 * Memory - Registered Memory
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef INFINITY_MEMORY_REGISTEREDMEMORY_H_
#define INFINITY_MEMORY_REGISTEREDMEMORY_H_

#include <infinity/core/Context.h>

namespace infinity {
namespace memory {

class RegisteredMemory {

public:

	 RegisteredMemory(infinity::core::Context *context, uint64_t sizeInBytes);
	 RegisteredMemory(infinity::core::Context *context, void *data, uint64_t sizeInBytes);
	 ~RegisteredMemory();

	 void * getData();

	 uint64_t getSizeInBytes();

	 ibv_mr * getRegion();


protected:

	 infinity::core::Context* context = nullptr;

	 void *data = 0;
	 uint64_t sizeInBytes = 0;

	 ibv_mr *ibvMemoryRegion = nullptr;

protected:

	 bool memoryAllocated = false;

};

} /* namespace infinity */
} /* namespace memory */

#endif /* INFINITY_MEMORY_REGISTEREDMEMORY_H_ */
