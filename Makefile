##################################################
#
# (c) 2018 Claude Barthels, ETH Zurich
#
# Call 'make library' to build the library
# Call 'make examples' to build the examples
# Call 'make all' to build everything
#
##################################################

PROJECT_NAME = libinfinity

##################################################

CC 					= g++
CC_FLAGS 		= -O3 -std=c++0x
LD_FLAGS		= -linfinity -libverbs

##################################################

SOURCE_FOLDER		= src
BUILD_FOLDER		= build
RELEASE_FOLDER	= release
INCLUDE_FOLDER	= include
EXAMPLES_FOLDER	= examples

##################################################

SOURCE_FILES =	$(SOURCE_FOLDER)/infinity/core/Context.cpp \
						$(SOURCE_FOLDER)/infinity/memory/Atomic.cpp \
						$(SOURCE_FOLDER)/infinity/memory/Buffer.cpp \
						$(SOURCE_FOLDER)/infinity/memory/Region.cpp \
						$(SOURCE_FOLDER)/infinity/memory/RegionToken.cpp \
						$(SOURCE_FOLDER)/infinity/memory/RegisteredMemory.cpp \
						$(SOURCE_FOLDER)/infinity/queues/QueuePair.cpp \
						$(SOURCE_FOLDER)/infinity/queues/QueuePairFactory.cpp \
						$(SOURCE_FOLDER)/infinity/requests/RequestToken.cpp \
						$(SOURCE_FOLDER)/infinity/utils/Address.cpp

HEADER_FILES	=	$(SOURCE_FOLDER)/infinity/infinity.h \
						$(SOURCE_FOLDER)/infinity/core/Context.h \
						$(SOURCE_FOLDER)/infinity/core/Configuration.h \
						$(SOURCE_FOLDER)/infinity/memory/Atomic.h \
						$(SOURCE_FOLDER)/infinity/memory/Buffer.h \
						$(SOURCE_FOLDER)/infinity/memory/Region.h \
						$(SOURCE_FOLDER)/infinity/memory/RegionToken.h \
						$(SOURCE_FOLDER)/infinity/memory/RegionType.h \
						$(SOURCE_FOLDER)/infinity/memory/RegisteredMemory.h \
						$(SOURCE_FOLDER)/infinity/queues/QueuePair.h \
						$(SOURCE_FOLDER)/infinity/queues/QueuePairFactory.h \
						$(SOURCE_FOLDER)/infinity/requests/RequestToken.h \
						$(SOURCE_FOLDER)/infinity/utils/Debug.h \
						$(SOURCE_FOLDER)/infinity/utils/Address.h

##################################################

OBJECT_FILES		= $(patsubst $(SOURCE_FOLDER)/%.cpp,$(BUILD_FOLDER)/%.o,$(SOURCE_FILES))
SOURCE_DIRECTORIES	= $(dir $(HEADER_FILES))
BUILD_DIRECTORIES	= $(patsubst $(SOURCE_FOLDER)/%,$(BUILD_FOLDER)/%,$(SOURCE_DIRECTORIES))

##################################################

all: library examples

##################################################

$(BUILD_FOLDER)/%.o: $(SOURCE_FILES) $(HEADER_FILES)
	mkdir -p $(BUILD_FOLDER)
	mkdir -p $(BUILD_DIRECTORIES)
	$(CC) $(CC_FLAGS) -c $(SOURCE_FOLDER)/$*.cpp -I $(SOURCE_FOLDER) -o $(BUILD_FOLDER)/$*.o

##################################################

library: $(OBJECT_FILES)
	mkdir -p $(RELEASE_FOLDER)
	ar rvs $(RELEASE_FOLDER)/$(PROJECT_NAME).a $(OBJECT_FILES)
	rm -rf $(RELEASE_FOLDER)/$(INCLUDE_FOLDER)
	cp --parents $(HEADER_FILES) $(RELEASE_FOLDER)
	mv $(RELEASE_FOLDER)/$(SOURCE_FOLDER)/ $(RELEASE_FOLDER)/$(INCLUDE_FOLDER)

##################################################

clean:
	rm -rf $(BUILD_FOLDER)
	rm -rf $(RELEASE_FOLDER)

##################################################

examples:
	mkdir -p $(RELEASE_FOLDER)/$(EXAMPLES_FOLDER)
	$(CC) src/examples/read-write-send.cpp $(CC_FLAGS) $(LD_FLAGS) -I $(RELEASE_FOLDER)/$(INCLUDE_FOLDER) -L $(RELEASE_FOLDER) -o $(RELEASE_FOLDER)/$(EXAMPLES_FOLDER)/read-write-send
	$(CC) src/examples/send-performance.cpp $(CC_FLAGS) $(LD_FLAGS) -I $(RELEASE_FOLDER)/$(INCLUDE_FOLDER) -L $(RELEASE_FOLDER) -o $(RELEASE_FOLDER)/$(EXAMPLES_FOLDER)/send-performance

##################################################
