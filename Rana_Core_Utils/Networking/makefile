# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic -Werror
C = gcc -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

CC_BUILD_FLAGS =	-fPIC -Wl,--as-needed -I../ -lUtil \
					-std=c++11 -Wno-write-strings
C_BUILD_FLAGS =	-fPIC -Wl,--as-needed

ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS += -D_FILE_OFFSET_BITS=64
endif

# File names
EXEC = libnetwork.so
SOURCES = $(wildcard *.cpp)
SOURCES_C = $(wildcard *.c)
OBJECTS = $(SOURCES:.cpp=.o) $(SOURCES_C:.c=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

%.o: %.c
	$(C) -c $(C_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
