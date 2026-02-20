# Emscripten compiler and linker flags
CXX = em++
CXXFLAGS = -std=c++17 -I parking/Include -I/usr/local/include -Wall -O2

LDFLAGS_WEB = --bind \
	-s MODULARIZE=1 \
	-s EXPORT_NAME='ParkingModule' \
	-s ENVIRONMENT=web \
	-s ALLOW_MEMORY_GROWTH=1

# Source files
PARKING_SRC = parking/pch.cpp \
	parking/Core.cpp \
	parking/ParkingLayout.cpp \
	parking/ParkingImage.cpp \
	parking/WKTParser.cpp \
	parking/bindings.cpp

# Object files
WEB_OBJ = $(PARKING_SRC:.cpp=.wasm.o)

# Output
PARKING_JS = bin/parking.js

.PHONY: all web clean

all: web

web: $(PARKING_JS)

$(PARKING_JS): $(WEB_OBJ) | bin
	$(CXX) $(WEB_OBJ) -o $@ $(LDFLAGS_WEB)

%.wasm.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean:
	rm -f $(WEB_OBJ) $(PARKING_JS) bin/parking.wasm