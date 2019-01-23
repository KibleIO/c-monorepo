# Declaration of variables
CC = g++ -g -Wall -Wextra -Wpedantic -O3 -fPIC -Wl,--as-needed -I../ -I/usr/include/libevdev-1.0 -std=c++11 -Wno-write-strings -lssh -lgraphics -lnetwork -linput -levdev -lavcodec /usr/local/lib/libx264.a -lavutil -lcurl -lswscale
CC_FLAGS = -w

UNAME_M = $(shell uname -m)

ifeq ($(UNAME_M),x86_64)
	CC += -lopenh264
endif

# File names
EXEC = libUtil.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
