# Cross‐platform Makefile for CoolGL
BUILD_DIR := build

# Detect Windows (OS is set to Windows_NT in cmd/Powershell)
ifeq ($(OS),Windows_NT)
  VCPKG_TOOLCHAIN := \
    -DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-windows
endif

.PHONY: rebuild clean configure build

rebuild: clean configure build

clean:
	@echo "🧹 Cleaning $(BUILD_DIR)/..."
	cmake -E rm -rf $(BUILD_DIR)

configure:
	@echo "📂 Recreating $(BUILD_DIR)/..."
	cmake -E make_directory $(BUILD_DIR)
	@echo "⚙️  Configuring with CMake..."
	cmake -S . -B $(BUILD_DIR) $(VCPKG_TOOLCHAIN)

build:
	@echo "🏗️  Building..."
	cmake --build $(BUILD_DIR)
	@echo "✅ Done!"
