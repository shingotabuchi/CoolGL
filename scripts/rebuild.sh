#!/usr/bin/env bash
set -e

echo "🧹 Cleaning build/…"
rm -rf build

echo "📂 Recreating build/…"
mkdir -p build

echo "⚙️  Configuring with CMake…"
cmake -S . -B build

echo "🏗️  Building…"
cmake --build build

echo "✅ Done!"
