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

CC_BUILD_FLAGS = -I/usr/include/opus -I./rana_core_utils -I./gen -I./clip

CC_COMPILE_FLAGS = -L./rana_core_utils -lpthread -lrana -lcares -lcurl -lavutil\
		 -lasound -lopus -lsodium -lswscale\
		-lXtst -lX11 -lXext -lopusfile -luuid -lx264 -ljson-c -lpulse-simple -lpulse -lwebsockets

CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/ pkg-config --libs grpc grpc++ protobuf) -lcurl

# File names
EXEC = themis/Themis
SOURCES = $(wildcard themis/*.cpp) $(wildcard themis/*/*.cpp) $(wildcard themis/*/*/*.cpp) $(wildcard gen/*/*.cpp) $(wildcard mongoose/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:arm make -f themis.mk docker -j8

x86:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:x86 make -f themis.mk docker -j8

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

run:
	mkdir -p ./themis/rana_core_utils
	cp ./rana_core_utils/librana.so ./themis/rana_core_utils
	docker run -p 4460:4460 -p 4461:4461 --platform linux/arm64 --pull=always --rm -it -v `pwd`/themis/:/home/kasm-user kible/firefox:arm bash