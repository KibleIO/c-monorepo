# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

CC_BUILD_FLAGS = -fPIC -Wl,--as-needed -I./ -std=c++11 -Wno-write-strings \
	-Wno-implicit-fallthrough -Wno-pedantic

ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS +=-D_FILE_OFFSET_BITS=64 -I/opt/vc/include\
	-I/opt/vc/include/interface/vmcs_host/linux\
	-I/opt/vc/include/interface/vcos/pthreads
endif

# File names
EXEC = librana.so
SOURCES = $(wildcard */*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default:
	docker run --pull=always --rm -it -v `pwd`:/root kible/coreutils make docker

docker: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

local: $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

install: $(EXEC)
	sudo cp ./librana.so /usr/local/lib
	sudo cp -R ./Networking/* /usr/local/include/rana/Networking
	sudo cp -R ./Utilities/* /usr/local/include/rana/Utilities

dockerimage-publish: $(EXEC)
	docker build -t kible/coreutils .
	docker push kible/coreutils

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
