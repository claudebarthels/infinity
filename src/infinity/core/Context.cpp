/**
 * Core - Context
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include "Context.h"

#include <string.h>
#include <limits>
#include <arpa/inet.h>

#include <infinity/core/Configuration.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Atomic.h>
#include <infinity/memory/Buffer.h>
#include <infinity/requests/RequestToken.h>
#include <infinity/utils/Debug.h>

#include <signal.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

namespace infinity {
namespace core {

/*******************************
 * Context
 ******************************/

Context::Context(uint16_t device, uint16_t devicePort) {

	// Get IB device list
	int32_t numberOfInstalledDevices = 0;
	ibv_device **ibvDeviceList = ibv_get_device_list(&numberOfInstalledDevices);
	INFINITY_ASSERT(numberOfInstalledDevices > 0, "[INFINITY][CORE][CONTEXT] No InfiniBand devices found.\n");
	INFINITY_ASSERT(device < numberOfInstalledDevices, "[INFINITY][CORE][CONTEXT] Requested device %d not found. There are %d devices available.\n",
			device, numberOfInstalledDevices);
	INFINITY_ASSERT(ibvDeviceList != NULL, "[INFINITY][CORE][CONTEXT] Device list was NULL.\n");

	// Get IB device
	this->ibvDevice = ibvDeviceList[device];
	INFINITY_ASSERT(this->ibvDevice != NULL, "[INFINITY][CORE][CONTEXT] Requested device %d was NULL.\n", device);

	// Open IB device and allocate protection domain
	this->ibvContext = ibv_open_device(this->ibvDevice);
	INFINITY_ASSERT(this->ibvContext != NULL, "[INFINITY][CORE][CONTEXT] Could not open device %d.\n", device);
	this->ibvProtectionDomain = ibv_alloc_pd(this->ibvContext);
	INFINITY_ASSERT(this->ibvProtectionDomain != NULL, "[INFINITY][CORE][CONTEXT] Could not allocate protection domain.\n");

	// Get the LID
	ibv_port_attr portAttributes;
	ibv_query_port(this->ibvContext, devicePort, &portAttributes);
	this->ibvLocalDeviceId = portAttributes.lid;
	this->ibvDevicePort = devicePort;

	// Allocate completion queues
	this->ibvSendCompletionQueue = ibv_create_cq(this->ibvContext, MAX(Configuration::SEND_COMPLETION_QUEUE_LENGTH, 1), NULL, NULL, 0);
	this->ibvReceiveCompletionQueue = ibv_create_cq(this->ibvContext, MAX(Configuration::RECV_COMPLETION_QUEUE_LENGTH, 1), NULL, NULL, 0);

	// Allocate shared receive queue
	ibv_srq_init_attr sia;
	memset(&sia, 0, sizeof(ibv_srq_init_attr));
	sia.srq_context = this->ibvContext;
	sia.attr.max_wr = MAX(Configuration::SHARED_RECV_QUEUE_LENGTH, 1);
	sia.attr.max_sge = 1;
	this->ibvSharedReceiveQueue = ibv_create_srq(this->ibvProtectionDomain, &sia);
	INFINITY_ASSERT(this->ibvSharedReceiveQueue != NULL, "[INFINITY][CORE][CONTEXT] Could not allocate shared receive queue.\n");

	// Create a default request token
	defaultRequestToken = new infinity::requests::RequestToken(this);
	defaultAtomic = new infinity::memory::Atomic(this);

#ifdef _RDMA_USE_ODP
	// Check if On-Demand-Paging (ODP) is supported by the device
	ibv_device_attr_ex da;
	INFINITY_ASSERT(ibv_query_device_ex(this->ibvContext, NULL, &da) == 0, "[INFINITY][CORE][CONTEXT] Failed to query device for extended device properties.\n");
	INFINITY_ASSERT(da.odp_caps.general_caps & IBV_ODP_SUPPORT, "[INFINITY][CORE][CONTEXT] ODP is not supported.\n");
	INFINITY_ASSERT(da.odp_caps.general_caps & IBV_ODP_SUPPORT_IMPLICIT, "[INFINITY][CORE][CONTEXT] ODP implicit is not supported.\n");
	INFINITY_ASSERT(da.odp_caps.per_transport_caps.rc_odp_caps & IBV_ODP_SUPPORT_READ, "[INFINITY][CORE][CONTEXT] ODP for reads is not supported.\n");
	INFINITY_ASSERT(da.odp_caps.per_transport_caps.rc_odp_caps & IBV_ODP_SUPPORT_RECV, "[INFINITY][CORE][CONTEXT] ODP for receives is not supported.\n");
	INFINITY_ASSERT(da.odp_caps.per_transport_caps.rc_odp_caps & IBV_ODP_SUPPORT_WRITE, "[INFINITY][CORE][CONTEXT] ODP for writes is not supported.\n");
	INFINITY_ASSERT(da.odp_caps.per_transport_caps.rc_odp_caps & IBV_ODP_SUPPORT_SEND, "[INFINITY][CORE][CONTEXT] ODP for sends is not supported.\n");
#endif // _RDMA_USE_ODP

}

Context::~Context() {

	// Delete default token
	delete defaultRequestToken;
	delete defaultAtomic;

	// Destroy shared receive queue
	int returnValue = ibv_destroy_srq(this->ibvSharedReceiveQueue);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Could not delete shared receive queue\n");

	// Destroy completion queues
	returnValue = ibv_destroy_cq(this->ibvSendCompletionQueue);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Could not delete send completion queue\n");
	returnValue = ibv_destroy_cq(this->ibvReceiveCompletionQueue);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Could not delete receive completion queue\n");

	// Destroy protection domain
	returnValue = ibv_dealloc_pd(this->ibvProtectionDomain);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Could not delete protection domain\n");

	// Close device
	returnValue = ibv_close_device(this->ibvContext);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Could not close device\n");

}

void Context::postReceiveBuffer(infinity::memory::Buffer* buffer) {

	INFINITY_ASSERT(buffer->getSizeInBytes() <= std::numeric_limits<uint32_t>::max(),
			"[INFINITY][CORE][CONTEXT] Cannot post receive buffer which is larger than max(uint32_t).\n");

	// Create scatter-getter
	ibv_sge isge;
	memset(&isge, 0, sizeof(ibv_sge));
	isge.addr = buffer->getAddress();
	isge.length = static_cast<uint32_t>(buffer->getSizeInBytes());
	isge.lkey = buffer->getLocalKey();

	// Create work request
	ibv_recv_wr wr;
	memset(&wr, 0, sizeof(ibv_recv_wr));
	wr.wr_id = reinterpret_cast<uint64_t>(buffer);
	wr.next = NULL;
	wr.sg_list = &isge;
	wr.num_sge = 1;

	// Post buffer to shared receive queue
	ibv_recv_wr *badwr;
	uint32_t returnValue = ibv_post_srq_recv(this->ibvSharedReceiveQueue, &wr, &badwr);
	INFINITY_ASSERT(returnValue == 0, "[INFINITY][CORE][CONTEXT] Cannot post buffer to receive queue.\n");

}

bool Context::receive(receive_element_t* receiveElement) {

	return receive(&(receiveElement->buffer), &(receiveElement->bytesWritten), &(receiveElement->immediateValue), &(receiveElement->immediateValueValid), &(receiveElement->queuePair));

}

bool Context::receive(infinity::memory::Buffer** buffer, uint32_t *bytesWritten, uint32_t *immediateValue, bool *immediateValueValid, infinity::queues::QueuePair **queuePair) {

	ibv_wc wc;
	if (ibv_poll_cq(this->ibvReceiveCompletionQueue, 1, &wc) > 0) {

		if(wc.opcode == IBV_WC_RECV) {
			*(buffer) = reinterpret_cast<infinity::memory::Buffer*>(wc.wr_id);
			*(bytesWritten) = wc.byte_len;
		} else if (wc.opcode == IBV_WC_RECV_RDMA_WITH_IMM) {
			*(buffer) = NULL;
			*(bytesWritten) = wc.byte_len;
			infinity::memory::Buffer* receiveBuffer = reinterpret_cast<infinity::memory::Buffer*>(wc.wr_id);
			this->postReceiveBuffer(receiveBuffer);
		}

		if(wc.wc_flags & IBV_WC_WITH_IMM) {
			*(immediateValue) = ntohl(wc.imm_data);
			*(immediateValueValid) = true;
		} else {
			*(immediateValue) = 0;
			*(immediateValueValid) = false;
		}

		if(queuePair != NULL) {
			*(queuePair) = queuePairMap.at(wc.qp_num);
		}

		return true;
	}

	return false;

}

bool Context::pollSendCompletionQueue() {

	ibv_wc wc;
	if (ibv_poll_cq(this->ibvSendCompletionQueue, 1, &wc) > 0) {

		infinity::requests::RequestToken * request = reinterpret_cast<infinity::requests::RequestToken*>(wc.wr_id);
		if (request != NULL) {
			request->setCompleted(wc.status == IBV_WC_SUCCESS);
		}

		if (wc.status == IBV_WC_SUCCESS) {
			INFINITY_DEBUG("[INFINITY][CORE][CONTEXT] Request completed (id %lu).\n", wc.wr_id);
		} else {
			INFINITY_DEBUG("[INFINITY][CORE][CONTEXT] Request failed (id %lu).\n", wc.wr_id);
		}
		return true;
	}

	return false;

}

void Context::registerQueuePair(infinity::queues::QueuePair* queuePair) {
	this->queuePairMap.insert({queuePair->getQueuePairNumber(), queuePair});
}

ibv_context* Context::getInfiniBandContext() {
	return this->ibvContext;
}

uint16_t Context::getLocalDeviceId() {
	return this->ibvLocalDeviceId;
}

uint16_t Context::getDevicePort() {
	return this->ibvDevicePort;
}

ibv_pd* Context::getProtectionDomain() {
	return this->ibvProtectionDomain;
}

ibv_cq* Context::getSendCompletionQueue() {
	return this->ibvSendCompletionQueue;
}

ibv_cq* Context::getReceiveCompletionQueue() {
	return this->ibvReceiveCompletionQueue;
}

ibv_srq* Context::getSharedReceiveQueue() {
	return this->ibvSharedReceiveQueue;
}

} /* namespace core */
} /* namespace infinity */
