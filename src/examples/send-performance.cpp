/**
 * Examples - Send Performance
 *
 * (c) 2018 Claude Barthels, ETH Zurich
 * Contact: claudeb@inf.ethz.ch
 *
 */

#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <infinity/core/Context.h>
#include <infinity/queues/QueuePairFactory.h>
#include <infinity/queues/QueuePair.h>
#include <infinity/memory/Buffer.h>
#include <infinity/memory/RegionToken.h>
#include <infinity/requests/RequestToken.h>

#define BUFFER_COUNT 128
#define MAX_BUFFER_SIZE 4096*8*4*4*4
#define OPERATIONS_COUNT 1024

uint64_t timeDiff(struct timeval stop, struct timeval start);

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

        auto context = std::make_shared<infinity::core::Context>();
        auto qpFactory = std::make_shared<infinity::queues::QueuePairFactory>(context);
	std::shared_ptr<infinity::queues::QueuePair> qp;

        if (isServer) {

                std::cout << "Creating buffers to receive a messages\n";
		std::vector<std::shared_ptr<infinity::memory::Buffer>> receiveBuffers;
                for (uint32_t i = 0; i < BUFFER_COUNT; ++i) {
		        receiveBuffers.emplace_back(std::make_shared<infinity::memory::Buffer>(context, MAX_BUFFER_SIZE));
                        context->postReceiveBuffer(receiveBuffers[i]);
                }

                std::cout << "Waiting for incoming connection\n";
                qpFactory->bindToPort(port_number);
                qp = qpFactory->acceptIncomingConnection();

                std::cout << "Waiting for first message (first message has additional setup costs)\n";
                infinity::core::receive_element_t receiveElement;
                while (!context->receive(receiveElement));
                context->postReceiveBuffer(receiveElement.buffer);

                std::cout << "Performing measurement\n";

                uint32_t messageSize = 1;
                uint32_t rounds = (uint32_t) log2(MAX_BUFFER_SIZE);

                for(uint32_t sizeIndex = 0; sizeIndex <= rounds; ++sizeIndex) {

                        std::cout << "Receiving messages of size " << messageSize << " bytes" << std::endl;

                        uint32_t numberOfReceivedMessages = 0;
                        while (numberOfReceivedMessages < OPERATIONS_COUNT) {
                                while (!context->receive(receiveElement));
                                ++numberOfReceivedMessages;
                                context->postReceiveBuffer(receiveElement.buffer);
                        }

                        messageSize *= 2;
                }

                std::cout << "All messages received\n";

                std::cout << "Sending notification to client\n";
                auto sendBuffer = std::make_shared<infinity::memory::Buffer>(context, 1);
                infinity::requests::RequestToken requestToken(context);
                qp->send(sendBuffer, &requestToken);
                requestToken.waitUntilCompleted();

                std::cout << "Clean up\n";
        } else {

                std::cout << "Connecting to remote node " << server_ip << ":" << port_number << "\n";
                qp = qpFactory->connectToRemoteHost(server_ip, port_number);

                std::cout << "Creating buffers\n";
                auto sendBuffer = std::make_shared<infinity::memory::Buffer>(context, MAX_BUFFER_SIZE);
                auto receiveBuffer = std::make_shared<infinity::memory::Buffer>(context, 1);
                context->postReceiveBuffer(receiveBuffer);

                std::cout << "Sending first message\n";
                infinity::requests::RequestToken defaultRequestToken(context);
                qp->send(sendBuffer, sendBuffer->getSizeInBytes(), &defaultRequestToken);
                defaultRequestToken.waitUntilCompleted();

                std::cout << "Performing measurement\n";
                uint32_t rounds = (uint32_t) log2(MAX_BUFFER_SIZE);
                uint32_t messageSize = 1;

                for(uint32_t sizeIndex = 0; sizeIndex <= rounds; ++sizeIndex) {

                        std::cout << "Sending messages of size " << messageSize << " bytes\t";

                        struct timeval start;
                        gettimeofday(&start, nullptr);

                        for(uint32_t i=0; i<OPERATIONS_COUNT; ++i) {
                                if(i %BUFFER_COUNT == 0 || i == OPERATIONS_COUNT) {

                                        infinity::requests::RequestToken requestToken(context);
                                        qp->send(sendBuffer, messageSize, &requestToken);
                                        requestToken.waitUntilCompleted();

                                } else {

                                        qp->send(sendBuffer, messageSize, nullptr);

                                }
                        }

                        struct timeval stop;
                        gettimeofday(&stop, nullptr);

                        uint64_t time = timeDiff(stop, start);
                        double msgRate = ((double)(OPERATIONS_COUNT * 1000000L)) / time;
                        double bandwidth = ((double) (OPERATIONS_COUNT * messageSize)) / (1024*1024) / (((double) time) / 1000000L);
                        std::cout << std::setprecision(3) << std::fixed  << msgRate << " msg/sec\t" << bandwidth << " MB/sec" << std::endl;

                        messageSize *= 2;

                }

                std::cout << "Waiting for notification from server\n";
                infinity::core::receive_element_t receiveElement;
                while (!context->receive(receiveElement));
        }

        return 0;

}

uint64_t timeDiff(struct timeval stop, struct timeval start) {
        return (stop.tv_sec * 1000000L + stop.tv_usec) - (start.tv_sec * 1000000L + start.tv_usec);
}
