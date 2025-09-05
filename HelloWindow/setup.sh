#!/usr/bin/env bash
set -e

cd "$(dirname "$0")"

rm -rf build
mkdir -p build
cd build

cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Debug

EXE="./Debug/ComputerGraphicsDemo.exe"
if [[ -f "$EXE" ]]; then
  "$EXE"
else
  echo "Executable not found: $EXE" >&2
  exit 1
fi