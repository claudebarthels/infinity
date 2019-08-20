/**
 * Examples - Read/Write/Send Operations
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cassert>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>

// Usage: ./progam -s for server and ./program for client component
int main(int argc, char **argv) {

	bool isServer = false;
	int port_number = 8011;
	const char *server_ip = "192.0.0.1";


	while (argc > 1) {
		if (argv[1][0] == '-') {
			switch (argv[1][1]) {

				case 's': {
					isServer = true;
					break;
				}
				 case 'h': {
				   server_ip = argv[2];
				   ++argv;
				   --argc;
				   break;
				 }
				  case 'p': {
				   port_number = atoi(argv[2]);
				   ++argv;
				   --argc;
				 }
			}
		}
		++argv;
		--argc;
	}

	infinity::core::Context *context = new infinity::core::Context();
	infinity::queues::QueuePairFactory *qpFactory = new  infinity::queues::QueuePairFactory(context);
	infinity::queues::QueuePair *qp;

	if(isServer) {

		printf("Creating buffers to read from and write to\n");
		infinity::memory::Buffer *bufferToReadWrite = new infinity::memory::Buffer(context, 128);
		infinity::memory::RegionToken bufferToken = bufferToReadWrite->createRegionToken();

		printf("Creating buffers to receive a message\n");
		infinity::memory::Buffer *bufferToReceive = new infinity::memory::Buffer(context, 128);
		context->postReceiveBuffer(bufferToReceive);

		printf("Setting up connection (blocking)\n");
		qpFactory->bindToPort(port_number);
		qp = qpFactory->acceptIncomingConnection(&bufferToken, sizeof(bufferToken));
		printf("Waiting for message (blocking)\n");
		infinity::core::receive_element_t receiveElement;
		while(!context->receive(&receiveElement));

		printf("Message received\n");
		delete bufferToReadWrite;
		delete bufferToReceive;

	} else {

		printf("Connecting to remote node\n");
		qp = qpFactory->connectToRemoteHost(server_ip, port_number);
		infinity::memory::RegionToken *remoteBufferToken = (infinity::memory::RegionToken *) qp->getUserData();


		printf("Creating buffers\n");
		infinity::memory::Buffer *buffer1Sided = new infinity::memory::Buffer(context, 128);
		infinity::memory::Buffer *buffer2Sided = new infinity::memory::Buffer(context, 128);

		printf("Reading content from remote buffer\n");
		infinity::requests::RequestToken requestToken(context);
		qp->read(buffer1Sided, *remoteBufferToken, &requestToken);
		requestToken.waitUntilCompleted();

		printf("Writing content to remote buffer\n");
		qp->write(buffer1Sided, *remoteBufferToken, &requestToken);
		requestToken.waitUntilCompleted();

		printf("Sending message to remote host\n");
		qp->send(buffer2Sided, &requestToken);
		requestToken.waitUntilCompleted();

		delete buffer1Sided;
		delete buffer2Sided;

	}

	delete qp;
	delete qpFactory;
	delete context;

	return 0;

}
