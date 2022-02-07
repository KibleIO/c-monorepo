# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

CC_BUILD_FLAGS = -fPIC -Wl,--as-needed -I./ -I$(HOME)/.local/include -std=c++11 \
	-Wno-write-strings -Wno-implicit-fallthrough -Wno-pedantic

CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/ pkg-config --libs grpc grpc++ protobuf)

ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS +=-D_FILE_OFFSET_BITS=64 -I/opt/vc/include\
	-I/opt/vc/include/interface/vmcs_host/linux\
	-I/opt/vc/include/interface/vcos/pthreads
endif

# File names
EXEC = librana.so
SOURCES = $(wildcard */*.cpp) $(wildcard idl/cpp/gen/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils make docker

zorin:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:zorin make docker

docker: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC) $(CXXFLAGS)

local: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC) $(CXXFLAGS)

build_proto:
	mkdir -p idl/cpp/gen
	PATH=$(PATH):$(HOME)/.local/bin protoc -I idl/idl --cpp_out=./idl/cpp/gen --grpc_out=./idl/cpp/gen --plugin=protoc-gen-grpc=$(HOME)/.local/bin/grpc_cpp_plugin idl/idl/*.proto
	rename 's/\.cc$$/.cpp/' ./idl/cpp/gen/*.cc

install: $(EXEC)
	sudo cp ./librana.so /usr/local/lib
	sudo mkdir -p /usr/local/include/rana/Networking/
	sudo mkdir -p /usr/local/include/rana/Utilities/
	sudo mkdir -p /usr/local/include/rana/Hermes/
	sudo mkdir -p /usr/local/include/rana/idl/cpp/gen/
	sudo rm -rf /usr/local/include/rana/Networking/*
	sudo rm -rf /usr/local/include/rana/Utilities/*
	sudo rm -rf /usr/local/include/rana/Hermes/*
	sudo rm -rf /usr/local/include/rana/idl/cpp/gen/*
	sudo cp -R ./Networking/*.h /usr/local/include/rana/Networking
	sudo cp -R ./Utilities/*.h /usr/local/include/rana/Utilities
	sudo cp -R ./Hermes/*.h /usr/local/include/rana/Hermes
	sudo cp -R ./idl/cpp/gen/*.h /usr/local/include/rana/idl/cpp/gen

dockerimage-publish: $(EXEC)
	docker build --file Dockerfile -t kible/coreutils .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils

dockerimage-zorin-publish: $(EXEC)
	docker build --file Dockerfile.Zorin -t kible/coreutils:zorin .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:zorin

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -rf idl/cpp/gen
	rm -f $(EXEC) $(OBJECTS)
