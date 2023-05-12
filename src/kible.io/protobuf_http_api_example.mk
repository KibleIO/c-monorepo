# Declaration of variables

CC = g++ -O3

CC_BUILD_FLAGS = -I./gen

CC_COMPILE_FLAGS = 

CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/ pkg-config --libs grpc grpc++ protobuf)

# File names
EXEC = protobuf_http_api_example/test
SOURCES = $(wildcard protobuf_http_api_example/*.cpp) $(wildcard protobuf_http_api_example/*/*.cpp) $(wildcard mongoose/*.cpp) $(wildcard gen/*/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntu make -f protobuf_http_api_example.mk docker -j8

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
	docker run -p 8000:8000/tcp --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntu $(EXEC)