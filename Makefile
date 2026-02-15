# Default compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Cross-compilation settings
MINGW_CXX = x86_64-w64-mingw32-g++
MINGW_CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++
MINGW_WINDRES = x86_64-w64-mingw32-windres

# Install settings
INSTALL_BINDIR ?= /usr/local/bin
INSTALL_CONFDIR ?= $(HOME)/.bane/wfn
# Detect Multiwfn executable (fallback to literal "Multiwfn") and default cores
INSTALL_CORES := $(if $(CORES),$(CORES),32)
MULTIWFN_EXEC := $(shell which Multiwfn 2>/dev/null || echo Multiwfn)

# Targets
TARGET_LINUX_BANE   = build/banewfn
TARGET_LINUX_PACK   = build/bwpack
TARGET_WINDOWS_BANE = build/banewfn.exe
TARGET_WINDOWS_PACK = build/bwpack.exe

# Sources (shared + per-binary)
COMMON_SOURCES = src/config.cpp src/input.cpp src/utils.cpp src/inline_conf.cpp
BANE_SOURCES   = src/banewfn.cpp src/ui.cpp
PACK_SOURCES   = src/bwpack.cpp

# Objects (Linux)
OBJECTS_LINUX_COMMON = $(patsubst src/%.cpp,build/%.o,$(COMMON_SOURCES))
OBJECTS_LINUX_BANE   = $(patsubst src/%.cpp,build/%.o,$(BANE_SOURCES)) $(OBJECTS_LINUX_COMMON)
OBJECTS_LINUX_PACK   = $(patsubst src/%.cpp,build/%.o,$(PACK_SOURCES)) $(OBJECTS_LINUX_COMMON)

# Objects (Windows)
OBJECTS_WINDOWS_COMMON = $(patsubst src/%.cpp,build/%_win.o,$(COMMON_SOURCES))
OBJECTS_WINDOWS_BANE   = $(patsubst src/%.cpp,build/%_win.o,$(BANE_SOURCES)) $(OBJECTS_WINDOWS_COMMON) build/banewfn_win_res.o
OBJECTS_WINDOWS_PACK   = $(patsubst src/%.cpp,build/%_win.o,$(PACK_SOURCES)) $(OBJECTS_WINDOWS_COMMON)

# Default target (Linux)
all: linux

# Linux build
linux: $(TARGET_LINUX_BANE) $(TARGET_LINUX_PACK)

$(TARGET_LINUX_BANE): $(OBJECTS_LINUX_BANE) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET_LINUX_BANE) $(OBJECTS_LINUX_BANE)

$(TARGET_LINUX_PACK): $(OBJECTS_LINUX_PACK) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET_LINUX_PACK) $(OBJECTS_LINUX_PACK)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows cross-compilation
windows: $(TARGET_WINDOWS_BANE) $(TARGET_WINDOWS_PACK)

$(TARGET_WINDOWS_BANE): $(OBJECTS_WINDOWS_BANE) | build
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -o $(TARGET_WINDOWS_BANE) $(OBJECTS_WINDOWS_BANE)

$(TARGET_WINDOWS_PACK): $(OBJECTS_WINDOWS_PACK) | build
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -o $(TARGET_WINDOWS_PACK) $(OBJECTS_WINDOWS_PACK)

build/%_win.o: src/%.cpp | build
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -c $< -o $@

# Windows资源文件编译 (banewfn only)
build/banewfn_win_res.o: src/banewfn.rc | build
	$(MINGW_WINDRES) -O coff -i $< -o $@ -I src

# Build both platforms
both: linux windows
	cp conf/banewfn.rc build/

# Install (Linux executables + config files)
install: linux
	mkdir -p $(INSTALL_BINDIR)
	cp $(TARGET_LINUX_BANE) $(INSTALL_BINDIR)/
	cp $(TARGET_LINUX_PACK) $(INSTALL_BINDIR)/
	chmod +x $(INSTALL_BINDIR)/banewfn
	chmod +x $(INSTALL_BINDIR)/bwpack
	mkdir -p $(INSTALL_CONFDIR)
	cp conf/* $(INSTALL_CONFDIR)/
	@printf 'Multiwfn_exec=%s\nconfpath=%s\ncores=%s\n' "$(MULTIWFN_EXEC)" "$(INSTALL_CONFDIR)" "$(INSTALL_CORES)" > "$(INSTALL_CONFDIR)/banewfn.rc"

build:
	mkdir -p build

clean:
	rm -rf build/*

.PHONY: all linux windows both clean install
