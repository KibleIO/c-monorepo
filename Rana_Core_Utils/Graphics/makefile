# Declaration of variables
CC = g++ -O3 -Wall -Wextra -Wpedantic

UNAME_M = $(shell uname -m)
USER=`whoami`

CC_BUILD_FLAGS =-fPIC -Wl,--as-needed -I../ -Wno-write-strings

ifeq ($(UNAME_M),armv7l)
	CC_BUILD_FLAGS += -D_FILE_OFFSET_BITS=64
endif

# File names
EXEC = libgraphics.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $(CC_BUILD_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
