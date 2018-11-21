# Declaration of variables
CC = g++ $(DEFINES) -g -Wall -Wextra -Wpedantic -O3 -fPIC -Wl,--as-needed -I/home/j0ker/github/Rana_Core_Utils -I/home/j0ker/github -std=c++11 -Wno-write-strings -lssh -lgraphics -lnetwork -linput -lgui -levdev -lavcodec -lavutil -lzstd -lcurl -lx264 -ldrm -lswscale
CC_FLAGS = -w

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
