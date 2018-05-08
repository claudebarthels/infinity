/**
 * Requests - Request Token
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "RequestToken.h"

namespace infinity {
namespace requests {

RequestToken::RequestToken(infinity::core::Context *context) :
		context(context) {
	this->success.store(false);
	this->completed.store(false);
	this->region = NULL;
	this->userData = NULL;
	this->userDataValid = false;
	this->userDataSize = 0;
	this->immediateValue = 0;
	this->immediateValueValid = false;
}

void RequestToken::setCompleted(bool success) {
	this->success.store(success);
	this->completed.store(true);
}

bool RequestToken::checkIfCompleted() {
	if (this->completed.load()) {
		return true;
	} else {
		this->context->pollSendCompletionQueue();
		return this->completed.load();
	}
}

void RequestToken::waitUntilCompleted() {
	while (!this->completed.load()) {
		this->context->pollSendCompletionQueue();
	}
}

bool RequestToken::wasSuccessful() {
	return this->success.load();
}

void RequestToken::reset() {
	this->success.store(false);
	this->completed.store(false);
	this->region = NULL;
	this->userData = NULL;
	this->userDataValid = false;
	this->userDataSize = 0;
	this->immediateValue = 0;
	this->immediateValueValid = false;
}

void RequestToken::setRegion(infinity::memory::Region* region) {
	this->region = region;
}

infinity::memory::Region* RequestToken::getRegion() {
	return this->region;
}

void RequestToken::setUserData(void* userData, uint32_t userDataSize) {
	this->userData = userData;
	this->userDataSize = userDataSize;
	this->userDataValid = true;
}

void* RequestToken::getUserData() {
	return this->userData;
}

bool RequestToken::hasUserData() {
	return this->userDataValid;
}

uint32_t RequestToken::getUserDataSize() {
	return this->userDataSize;
}

void RequestToken::setImmediateValue(uint32_t immediateValue) {
	this->immediateValue = immediateValue;
	this->immediateValueValid = true;
}

uint32_t RequestToken::getImmediateValue() {
	return this->immediateValue;
}

bool RequestToken::hasImmediateValue() {
	return this->immediateValueValid;
}

} /* namespace requests */
} /* namespace infinity */
