# Declaration of variables
CC = g++ -O3 -g -Wall -Wextra -Wpedantic -fPIC -I/home/j0ker/github/Rana_Core_Utils -I/home/j0ker/github -lutil -std=c++11 -Wno-write-strings
CC_FLAGS = -w

# File names
EXEC = libnetwork.so
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
