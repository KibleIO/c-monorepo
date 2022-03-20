# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

#-mmacosx-version-min=10.13

CC_BUILD_FLAGS = -fPIC -Wl,--as-needed -I./ -I$(HOME)/.local/include -std=c++11 \
	-Wno-write-strings -Wno-implicit-fallthrough -Wno-pedantic -Wno-unused-variable

CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/:/usr/local/opt/openssl@3/lib/pkgconfig pkg-config --libs grpc grpc++ protobuf)

ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS +=-D_FILE_OFFSET_BITS=64 -I/opt/vc/include\
	-I/opt/vc/include/interface/vmcs_host/linux\
	-I/opt/vc/include/interface/vcos/pthreads
endif

# File names
EXEC = librana.so
EXEC_MAC = librana.dylib
SOURCES = $(wildcard */*.cpp) $(wildcard idl/cpp/gen/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils make docker

zorin:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:zorin make docker

xubuntu:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntu make docker

xubuntux86:
	docker run --pull=always --rm -it -v `pwd`:/root/code kible/coreutils:xubuntux86 make docker

docker: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC) $(CXXFLAGS)

mac: $(OBJECTS)
	$(CC) -dynamiclib -undefined suppress -flat_namespace $(OBJECTS) -o $(EXEC_MAC) $(CXXFLAGS) -framework Cocoa -L/usr/local/Cellar/libsodium/1.0.18_1/lib -L/usr/local/Cellar/ffmpeg/5.0/lib -lsodium -lswscale -lcurl -ljson-c

local: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC) $(CXXFLAGS)

docker-build-proto:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils make build_proto

build_proto:
	mkdir -p idl/cpp/gen
	PATH=$(PATH):$(HOME)/.local/bin protoc -I idl/idl --cpp_out=./idl/cpp/gen --grpc_out=./idl/cpp/gen --plugin=protoc-gen-grpc=$(HOME)/.local/bin/grpc_cpp_plugin idl/idl/*.proto
	rename 's/\.cc$$/.cpp/' ./idl/cpp/gen/*.cc

install_mac: $(EXEC_MAC)
	sudo cp ./$(EXEC_MAC) /usr/local/lib
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

install: $(EXEC)
	sudo cp ./$(EXEC) /usr/local/lib
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
	docker build --platform linux/amd64 --file Dockerfile -t kible/coreutils .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils

dockerimage-zorin-publish: $(EXEC)
	docker build --file Dockerfile.Zorin -t kible/coreutils:zorin .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:zorin

dockerimage-xubuntu-publish: $(EXEC)
	docker build --file Dockerfile.Xubuntu -t kible/coreutils:xubuntu .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:xubuntu

dockerimage-xubuntu-x86-publish: $(EXEC)
	docker build --platform linux/amd64 --file Dockerfile.Xubuntu -t kible/coreutils:xubuntux86 .
	docker login -u "kible" -p "eehKgVR4QmoED8" docker.io
	docker push kible/coreutils:xubuntux86

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# Main target
$(EXEC_MAC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -rf idl/cpp/gen
	rm -f $(EXEC) $(OBJECTS) $(EXEC_MAC)
