#! /usr/bin/env sh

cmake -B out -S . \
    -DCMAKE_BUILD_TYPE=Debug \
    -D CMAKE_C_COMPILER=clang \
    -D CMAKE_CXX_COMPILER=clang++ \
    -D CMAKE_EXPORT_COMPILE_COMMANDS=True \
&&
cmake --build out