# Emscripten compiler and linker flags
CXX = em++
CXXFLAGS = -std=c++17 -I/usr/local/include -Wall -O2
LDFLAGS = -s WASM=1 -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s EXPORTED_FUNCTIONS='["_main"]' --bind
LDFLAGS_LIB = -s SIDE_MODULE=1
LDFLAGS_MAIN = -s MAIN_MODULE=1

# Paths to source files
PARKING_SRC = parking/pch.cpp parking/Core.cpp parking/parking.cpp parking/ParkingLayout.cpp
CONSOLE_SRC = console/console.cpp

# Object files (using .wasm.o extension to differentiate from native .o files)
LIB_OBJ = $(PARKING_SRC:.cpp=.wasm.o)
CONSOLE_OBJ = $(CONSOLE_SRC:.cpp=.wasm.o)

# Name of the library (WebAssembly)
PARKING_LIB = bin/parking.wasm

# Name of the executables (WebAssembly + JavaScript)
CONSOLE_HTML = bin/console.html
CONSOLE_JS = bin/console.js
CONSOLE_WASM = bin/console.wasm

.PHONY: all clean parking console

all: parking console

parking: $(PARKING_LIB)

$(PARKING_LIB): $(LIB_OBJ) | bin
	$(CXX) $(LIB_OBJ) -o $@ $(LDFLAGS_LIB)

console: $(CONSOLE_HTML)

$(CONSOLE_HTML): $(CONSOLE_OBJ) $(LIB_OBJ) | bin
	$(CXX) $(CONSOLE_OBJ) $(LIB_OBJ) -o $@ $(LDFLAGS)

%.wasm.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -f $(LIB_OBJ) $(CONSOLE_OBJ) $(PARKING_LIB) $(CONSOLE_HTML) $(CONSOLE_JS) $(CONSOLE_WASM)

# Optional: build just the JavaScript/WASM without HTML wrapper
console-js: bin/console.js

bin/console.js: $(CONSOLE_OBJ) $(LIB_OBJ) | bin
	$(CXX) $(CONSOLE_OBJ) $(LIB_OBJ) -o $@ $(LDFLAGS)