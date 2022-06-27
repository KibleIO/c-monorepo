# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

#-mmacosx-version-min=10.13

ifeq ($(OS),Windows_NT)
	CC_BUILD_FLAGS = -fPIC -Wl,--as-needed -I$(HOME)/github/Rana_Core_Utils/Rana_Core_Utils -I$(HOME)/.local/include -I/mingw64/include -I/usr/include -std=c++11 \
	-Wno-write-strings -Wno-implicit-fallthrough -Wno-pedantic -Wno-unused-variable

	CXXFLAGS = -L/mingw64/lib -lgrpc++ -lgrpc -laddress_sorting -lre2 -lupb -lcares -lz -lgpr -lssl -lcrypto -labsl_raw_hash_set -labsl_hashtablez_sampler -labsl_hash -labsl_city -labsl_low_level_hash -labsl_random_distributions -labsl_random_seed_sequences -labsl_random_internal_pool_urbg -labsl_random_internal_randen -labsl_random_internal_randen_hwaes -labsl_random_internal_randen_hwaes_impl -labsl_random_internal_randen_slow -labsl_random_internal_platform -lbcrypt -labsl_random_internal_seed_material -labsl_random_seed_gen_exception -labsl_statusor -labsl_status -labsl_cord -labsl_cordz_info -labsl_cord_internal -labsl_cordz_functions -labsl_exponential_biased -labsl_cordz_handle -labsl_bad_optional_access -labsl_str_format_internal -labsl_synchronization -labsl_graphcycles_internal -labsl_stacktrace -ldbghelp -labsl_symbolize -labsl_debugging_internal -labsl_demangle_internal -labsl_malloc_internal -labsl_time -labsl_civil_time -labsl_strings -labsl_strings_internal -ladvapi32 -labsl_base -labsl_spinlock_wait -labsl_int128 -labsl_throw_delegate -labsl_time_zone -labsl_bad_variant_access -labsl_raw_logging_internal -labsl_log_severity -lprotobuf -lpthread

else
	CC_BUILD_FLAGS = -fPIC -Wl,--as-needed -I./ -I$(HOME)/.local/include -std=c++11 \
	-Wno-write-strings -Wno-implicit-fallthrough -Wno-pedantic -Wno-unused-variable

	CXXFLAGS = $(shell PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:$$HOME/.local/lib/pkgconfig/:/usr/local/opt/openssl@3/lib/pkgconfig pkg-config --libs grpc grpc++ protobuf)

endif


ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS +=-D_FILE_OFFSET_BITS=64 -I/opt/vc/include\
	-I/opt/vc/include/interface/vmcs_host/linux\
	-I/opt/vc/include/interface/vcos/pthreads
endif

# File names
EXEC = librana.so
EXEC_MAC = librana.dylib
EXEC_WIN = rana.dll
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

win: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC_WIN) -o $(EXEC_WIN) $(CXXFLAGS) -ljson-c -lsodium -lavcodec -lswscale -lcurl -lws2_32 -lrpcrt4 -lcrypt32

docker-build-proto:
	docker run --platform linux/amd64 --pull=always --rm -it -v `pwd`:/root/code kible/coreutils make build_proto

build_proto:
	mkdir -p idl/cpp/gen
	PATH=$(PATH):$(HOME)/.local/bin protoc -I idl/idl --cpp_out=./idl/cpp/gen --grpc_out=./idl/cpp/gen --plugin=protoc-gen-grpc=$(HOME)/.local/bin/grpc_cpp_plugin idl/idl/*.proto
	rename 's/\.cc$$/.cpp/' ./idl/cpp/gen/*.cc

build_proto_win:
	mkdir -p idl/cpp/gen
	PATH=$(PATH):/mingw64/bin protoc -I idl/idl --cpp_out=./idl/cpp/gen --grpc_out=./idl/cpp/gen --plugin=protoc-gen-grpc=/mingw64/bin/grpc_cpp_plugin idl/idl/*.proto
	rename .cc .cpp ./idl/cpp/gen/*.cc

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

install_win: $(EXEC_WIN)
	cp ./$(EXEC_WIN) /usr/local/lib
	mkdir -p /usr/local/include/rana/Networking/
	mkdir -p /usr/local/include/rana/Utilities/
	mkdir -p /usr/local/include/rana/Hermes/
	mkdir -p /usr/local/include/rana/idl/cpp/gen/
	rm -rf /usr/local/include/rana/Networking/*
	rm -rf /usr/local/include/rana/Utilities/*
	rm -rf /usr/local/include/rana/Hermes/*
	rm -rf /usr/local/include/rana/idl/cpp/gen/*
	cp -R ./Networking/*.h /usr/local/include/rana/Networking
	cp -R ./Utilities/*.h /usr/local/include/rana/Utilities
	cp -R ./Hermes/*.h /usr/local/include/rana/Hermes
	cp -R ./idl/cpp/gen/*.h /usr/local/include/rana/idl/cpp/gen

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
