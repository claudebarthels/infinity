/*
 * Memory - Buffer
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef MEMORY_BUFFER_H_
#define MEMORY_BUFFER_H_

#include <infinity/core/Context.h>
#include <infinity/memory/Region.h>
#include <infinity/memory/RegisteredMemory.h>

namespace infinity {
namespace memory {

class Buffer : public Region {

public:

	Buffer(infinity::core::Context *context, uint64_t sizeInBytes);
	Buffer(infinity::core::Context *context, infinity::memory::RegisteredMemory *memory, uint64_t offset, uint64_t sizeInBytes);
	Buffer(infinity::core::Context *context, void *memory, uint64_t sizeInBytes);
	~Buffer();

public:

	void * getData();
	void resize(uint64_t newSize, void *newData = NULL);

protected:

	bool memoryRegistered;
	bool memoryAllocated;


};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_BUFFER_H_ */
