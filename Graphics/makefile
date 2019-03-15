# Declaration of variables
CC = g++ -O3 -fPIC -I../ -std=c++11 -Wno-write-strings

# File names
EXEC = libgraphics.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -shared -Wl,-soname,$(EXEC) -o $(EXEC)

# To obtain object files
%.o: %.cpp
	$(CC) -c $< -o $@

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
