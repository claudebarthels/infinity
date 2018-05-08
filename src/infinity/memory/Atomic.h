/*
 * Memory - Atomic
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef MEMORY_ATOMIC_H_
#define MEMORY_ATOMIC_H_

#include <stdint.h>

#include <infinity/memory/Region.h>
#include <infinity/core/Context.h>


namespace infinity {
namespace memory {

class Atomic : public Region {

public:

	Atomic(infinity::core::Context *context);
	virtual ~Atomic();

public:

	uint64_t getValue();

	void setValueNonAtomic(uint64_t value);

protected:

	uint64_t value;


};

} /* namespace memory */
} /* namespace infinity */

#endif /* MEMORY_ATOMIC_H_ */
