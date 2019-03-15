# Declaration of variables
CC = g++ -O3

CC_BUILD_FLAGS =	-fPIC -Wl,--as-needed -I../ -I/usr/include/libevdev-1.0 \
					-std=c++11 -Wno-write-strings

# File names
EXEC = libUtil.so
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
