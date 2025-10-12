# Default compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Cross-compilation settings
MINGW_CXX = x86_64-w64-mingw32-g++
MINGW_CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++

# Targets
TARGET_LINUX = build/banewfn
TARGET_WINDOWS = build/banewfn.exe
SOURCES = src/banewfn.cpp src/config.cpp src/input.cpp src/ui.cpp src/utils.cpp
OBJECTS_LINUX = build/banewfn.o build/config.o build/input.o build/ui.o build/utils.o
OBJECTS_WINDOWS = build/banewfn_win.o build/config_win.o build/input_win.o build/ui_win.o build/utils_win.o

# Default target (both platforms)
all: both

# Linux build
linux: $(TARGET_LINUX)

$(TARGET_LINUX): $(OBJECTS_LINUX) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET_LINUX) $(OBJECTS_LINUX)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows cross-compilation
windows: $(TARGET_WINDOWS)

$(TARGET_WINDOWS): $(OBJECTS_WINDOWS) | build
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -o $(TARGET_WINDOWS) $(OBJECTS_WINDOWS)

build/%_win.o: src/%.cpp | build
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -c $< -o $@

# Build both platforms
both: linux windows
	cp conf/banewfn.rc build/

build:
	mkdir -p build

clean:
	rm -rf build/*

.PHONY: all linux windows both clean
