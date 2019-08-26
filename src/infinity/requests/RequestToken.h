/**
 * Requests - Request Token
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#ifndef REQUESTS_REQUESTTOKEN_H_
#define REQUESTS_REQUESTTOKEN_H_

#include <atomic>
#include <stdint.h>

#include <infinity/core/Context.h>
#include <infinity/memory/Region.h>

namespace infinity {
namespace requests {

class RequestToken {

public:

        RequestToken(std::shared_ptr<infinity::core::Context> context);

	void reset();

        void setRegion(std::shared_ptr<infinity::memory::Region> region);
        std::shared_ptr<infinity::memory::Region> getRegion();

	void setCompleted(bool success);
	bool wasSuccessful();

	bool checkIfCompleted();
	void waitUntilCompleted();

	void setImmediateValue(uint32_t immediateValue);
	bool hasImmediateValue();
	uint32_t getImmediateValue();

	void setUserData(void* userData, uint32_t userDataSize);
	bool hasUserData();
	void* getUserData();
	uint32_t getUserDataSize();

	RequestToken(const RequestToken&) = delete;
	RequestToken(const RequestToken&&) = delete;
	RequestToken& operator=(const RequestToken&) = delete;
	RequestToken& operator=(RequestToken&&) = delete;

protected:

        std::shared_ptr<infinity::core::Context> const context;
        std::shared_ptr<infinity::memory::Region> region;

	std::atomic<bool> completed;
	std::atomic<bool> success;

	void *userData = nullptr;
	uint32_t userDataSize = 0;
	bool userDataValid = false;
 
	uint32_t immediateValue = 0;
	bool immediateValueValid = false;

};

} /* namespace requests */
} /* namespace infinity */

#endif /* REQUESTS_REQUESTTOKEN_H_ */
