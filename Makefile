# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -c

# Source files
SOURCES = visca27.cpp ViscaAPI.cpp main.cpp

# Object files
OBJECTS = visca27.o ViscaAPI.o

# Static library
LIBRARY = libvisca27.a

# Executable
EXECUTABLE = testvisca

# Default target
all: $(LIBRARY) $(EXECUTABLE)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Create static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# Link the executable
$(EXECUTABLE): main.o $(LIBRARY)
	$(CXX) -o $@ main.o -L. -lvisca27

# Clean up build files
clean:
	rm -f $(OBJECTS) main.o $(LIBRARY) $(EXECUTABLE)
