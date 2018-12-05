# Declaration of variables
CC = g++ -O3 -g -fPIC -Wall -Wextra -Wpedantic -Wl,--as-needed -I/home/j0ker/github/Rana_Core_Utils -I/home/j0ker/github -std=c++11 -Wno-write-strings -lX11 -lXtst -lXext -lgraphics -lUtil -lnetwork -linput -levdev -lavcodec -lavutil -lzstd -lcurl -lx264 -ldrm
CC_FLAGS = -w

# File names
EXEC = libgui.so
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
