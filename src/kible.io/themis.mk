# Declaration of variables

# sanitize memory (also does leak checking)
#SAN +=-fsanitize=address
# sanitize leaks
# SAN +=-fsanitize=leak
# sanitize threads
#SAN =-fsanitize=thread
# sanitize threads
#SAN =-fsanitize=undefined
# sanitize off
SAN =

CC = g++ -Wall -Wextra -Wno-pedantic -Wno-write-strings -Wno-missing-field-initializers -O3 $(SAN)

CC_BUILD_FLAGS =	-I/usr/local/include/rana -I/usr/include/opus -I$(HOME)/.local/include

CC_COMPILE_FLAGS = -lpthread -lrana -lcares -lcurl -lavutil\
		 -lasound -lopus -lsodium -lswscale\
		-lXtst -lX11 -lXext -lopusfile -luuid -lx264 -ljson-c -lpulse-simple -lpulse -lwebsockets

CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/ pkg-config --libs grpc grpc++ protobuf)

# File names
EXEC = Themis
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntu make docker

docker: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(CC_COMPILE_FLAGS) $(CXXFLAGS)

local: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(CC_COMPILE_FLAGS) $(CXXFLAGS)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(CC_COMPILE_FLAGS)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)

run-arm:
	docker run -p 4460:4460 --platform linux/arm64 --pull=always --rm -it kible/firefox:arm bash