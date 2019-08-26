/*
 * Memory - Buffer
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef MEMORY_BUFFER_H_
#define MEMORY_BUFFER_H_

#include <memory>
#include <infinity/core/Context.h>
#include <infinity/memory/Region.h>
#include <infinity/memory/RegisteredMemory.h>

namespace infinity {
namespace memory {

/* 
   Buffers should only be created as shared_ptrs because of the code in
   Context that calls getptr(), which will produce undefined behaviour
   if the Buffer is not owned by a shared_ptr.
 */
class Buffer : public Region, public std::enable_shared_from_this<infinity::memory::Buffer>{

public:

	Buffer(std::shared_ptr<infinity::core::Context> context, uint64_t sizeInBytes);
	Buffer(std::shared_ptr<infinity::core::Context> context, infinity::memory::RegisteredMemory *memory, uint64_t offset, uint64_t sizeInBytes);
	Buffer(std::shared_ptr<infinity::core::Context> context, void *memory, uint64_t sizeInBytes);
	~Buffer();
	Buffer(const Buffer&) = delete;
	Buffer(const Buffer&&) = delete;
	Buffer& operator=(const Buffer&) = delete;
	Buffer& operator=(Buffer&&other);

public:

	void * getData();
	void resize(uint64_t newSize, void *newData = nullptr);

public:
        std::shared_ptr<Buffer> getptr() { return shared_from_this(); }  

protected:

	bool memoryRegistered = false;
	bool memoryAllocated = false;


};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_BUFFER_H_ */
