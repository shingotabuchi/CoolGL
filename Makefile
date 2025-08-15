# IMPORTANT: All indented lines in this file MUST start with a Tab character, not spaces.

BUILD_DIR := build
CONFIG ?= Debug

# Automatically discover demo targets from src/demo/*.cpp files
DEMO_SOURCES := $(wildcard src/demo/*.cpp)
TARGETS := $(basename $(notdir $(DEMO_SOURCES)))

# --- Platform-specific settings ---
# Default settings for Unix-like systems (macOS, Linux)
# Assumes a single-config generator where executables are in `build/`
EXE_EXT ?=
RUN_DIR := ./$(BUILD_DIR)

# Windows-specific overrides (OS is set to Windows_NT in cmd/Powershell)
ifeq ($(OS),Windows_NT)
  VCPKG_TOOLCHAIN := \
    -DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-windows
  EXE_EXT := .exe
  # Override RUN_DIR for Windows multi-config generators (e.g., Visual Studio)
  # where executables are in `build/Debug/` or `build/Release/`
  RUN_DIR := $(BUILD_DIR)/$(CONFIG)
endif

.PHONY: rebuild clean configure build list run-% $(TARGETS)

rebuild: clean configure build

clean:
	@echo "🧹 Cleaning $(BUILD_DIR)/..."
	@cmake -E rm -rf $(BUILD_DIR)

configure:
	@echo "📂 Recreating $(BUILD_DIR)/..."
	@cmake -E make_directory $(BUILD_DIR)
	@echo "⚙️  Configuring with CMake..."
	@cmake -S . -B $(BUILD_DIR) $(VCPKG_TOOLCHAIN)

build:
	@echo "🏗️  Building all targets..."
	@cmake --build $(BUILD_DIR) --config $(CONFIG)
	@echo "✅ Done!"

# Build specific targets
$(TARGETS): configure
	@echo "🏗️  Building target: $@..."
	@cmake --build $(BUILD_DIR) --target $@ --config $(CONFIG)
	@echo "✅ Target $@ built successfully!"

# Build and run a specific target, e.g. `make run-experiment`
run-%: %
	@echo "🚀 Running target: $*..."
	@$(RUN_DIR)/$*$(EXE_EXT)

# List available targets
list:
	@echo "📋 Available targets: $(TARGETS)"
	@echo ""
	@echo "Usage:"
	@echo "  make build          # Build all targets"
	@echo "  make <target>       # Build specific target"
	@echo "  make run-<target>   # Build and run specific target"
	@echo "  make list           # Show this help"
