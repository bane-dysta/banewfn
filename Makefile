# Default compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS ?=
LDLIBS ?=

# GCC 8 keeps std::filesystem in a separate support library.  Detect the
# compiler major version so the direct Makefile build matches the CMake build.
CXX_VERSION := $(shell $(CXX) -dumpfullversion -dumpversion 2>/dev/null)
CXX_MAJOR := $(word 1,$(subst ., ,$(CXX_VERSION)))
FILESYSTEM_LIB := $(if $(filter 8,$(CXX_MAJOR)),-lstdc++fs)

# Cross-compilation settings
MINGW_CXX = x86_64-w64-mingw32-g++
MINGW_CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -static-libgcc -static-libstdc++
MINGW_WINDRES = x86_64-w64-mingw32-windres

# Build directories
BUILD_DIR = build
BUILD_WINDOWS_DIR = build_windows

# Install settings
INSTALL_BINDIR ?= /usr/local/bin
INSTALL_CONFDIR ?= $(HOME)/.bane/wfn
# Detect Multiwfn executable (fallback to literal "Multiwfn") and default cores
INSTALL_CORES := $(if $(CORES),$(CORES),32)
MULTIWFN_EXEC := $(shell which Multiwfn 2>/dev/null || echo Multiwfn)

# Targets
TARGET_LINUX_BANE   = $(BUILD_DIR)/banewfn
TARGET_LINUX_PACK   = $(BUILD_DIR)/bwpack
TARGET_WINDOWS_BANE = $(BUILD_WINDOWS_DIR)/banewfn.exe
TARGET_WINDOWS_PACK = $(BUILD_WINDOWS_DIR)/bwpack.exe

# Sources (shared + per-binary)
COMMON_SOURCES = src/bwpack_support.cpp src/citation.cpp src/config.cpp src/grep_dsl.cpp src/grep_engine.cpp src/input.cpp src/utils.cpp src/inline_conf.cpp
BANE_SOURCES   = src/banewfn.cpp src/ui.cpp
PACK_SOURCES   = src/bwpack.cpp

# Objects (Linux)
OBJECTS_LINUX_COMMON = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(COMMON_SOURCES))
OBJECTS_LINUX_BANE   = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(BANE_SOURCES)) $(OBJECTS_LINUX_COMMON)
OBJECTS_LINUX_PACK   = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(PACK_SOURCES)) $(OBJECTS_LINUX_COMMON)

# Objects (Windows)
OBJECTS_WINDOWS_COMMON = $(patsubst src/%.cpp,$(BUILD_WINDOWS_DIR)/%_win.o,$(COMMON_SOURCES))
OBJECTS_WINDOWS_BANE   = $(patsubst src/%.cpp,$(BUILD_WINDOWS_DIR)/%_win.o,$(BANE_SOURCES)) $(OBJECTS_WINDOWS_COMMON) $(BUILD_WINDOWS_DIR)/banewfn_win_res.o
OBJECTS_WINDOWS_PACK   = $(patsubst src/%.cpp,$(BUILD_WINDOWS_DIR)/%_win.o,$(PACK_SOURCES)) $(OBJECTS_WINDOWS_COMMON) $(BUILD_WINDOWS_DIR)/bwpack_win_res.o

# Default target (Linux)
all: linux

# Linux build
linux: $(TARGET_LINUX_BANE) $(TARGET_LINUX_PACK)

$(TARGET_LINUX_BANE): $(OBJECTS_LINUX_BANE) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET_LINUX_BANE) $(OBJECTS_LINUX_BANE) $(LDLIBS) $(FILESYSTEM_LIB)

$(TARGET_LINUX_PACK): $(OBJECTS_LINUX_PACK) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET_LINUX_PACK) $(OBJECTS_LINUX_PACK) $(LDLIBS) $(FILESYSTEM_LIB)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows cross-compilation
windows: $(TARGET_WINDOWS_BANE) $(TARGET_WINDOWS_PACK)

$(TARGET_WINDOWS_BANE): $(OBJECTS_WINDOWS_BANE) | $(BUILD_WINDOWS_DIR)
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -o $(TARGET_WINDOWS_BANE) $(OBJECTS_WINDOWS_BANE)

$(TARGET_WINDOWS_PACK): $(OBJECTS_WINDOWS_PACK) | $(BUILD_WINDOWS_DIR)
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -o $(TARGET_WINDOWS_PACK) $(OBJECTS_WINDOWS_PACK)

$(BUILD_WINDOWS_DIR)/%_win.o: src/%.cpp | $(BUILD_WINDOWS_DIR)
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -c $< -o $@

# Windows资源文件编译
$(BUILD_WINDOWS_DIR)/banewfn_win_res.o: src/banewfn.rc | $(BUILD_WINDOWS_DIR)
	$(MINGW_WINDRES) -O coff -i $< -o $@ -I src

$(BUILD_WINDOWS_DIR)/bwpack_win_res.o: src/bwpack.rc | $(BUILD_WINDOWS_DIR)
	$(MINGW_WINDRES) -O coff -i $< -o $@ -I src

# Build both platforms
both: linux windows
	cp conf/banewfn.rc $(BUILD_WINDOWS_DIR)/

# Install (Linux executables + config files)
install: linux
	mkdir -p $(INSTALL_BINDIR)
	cp $(TARGET_LINUX_BANE) $(INSTALL_BINDIR)/
	cp $(TARGET_LINUX_PACK) $(INSTALL_BINDIR)/
	chmod +x $(INSTALL_BINDIR)/banewfn
	chmod +x $(INSTALL_BINDIR)/bwpack
	mkdir -p $(INSTALL_CONFDIR)
	cp conf/* $(INSTALL_CONFDIR)/
	@printf 'Multiwfn_exec=%s\nconfpath=%s\ncores=%s\ncitations_output=references.bib\n' "$(MULTIWFN_EXEC)" "$(INSTALL_CONFDIR)" "$(INSTALL_CORES)" > "$(INSTALL_CONFDIR)/banewfn.rc"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_WINDOWS_DIR):
	mkdir -p $(BUILD_WINDOWS_DIR)

clean:
	rm -rf $(BUILD_DIR)/* $(BUILD_WINDOWS_DIR)/*

.PHONY: all linux windows both clean install
